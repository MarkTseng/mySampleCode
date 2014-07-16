#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
#define LAMBO86_HW_INIT
#include <types.h>
#include <gsl/lambo86_hw.h>
#include <gsl/lambo86_mem.h>
#include <gsl/lambo86_hwop.h>
#include <gsl/lambo86_irq_ioctl.h>
#include "gl_common.h"

#define LAMBO86_HWOP_DEBUG
#ifdef LAMBO86_HWOP_DEBUG
#define HWOP_DEBUG(fmt, args...) printf(" " fmt, ## args)
#else
#define HWOP_DEBUG(fmt, args...)
#endif

#define FIXED_MAP

unsigned int heapA_cache_end;
unsigned int heapA_uncache_end;
unsigned int heapB_cache_end;
unsigned int heapB_uncache_end;

void itoa(int value, char *str)
{
    int i,j;

    for(i=1; value > 0; i++,value/=10)
        str[i] = value%10+'0';

    for(j=i-1,i=1; j-i>=1; j--,i++)
    {
        str[i] = str[i]^str[j];
        str[j] = str[i]^str[j];
        str[i] = str[i]^str[j];
    }

    for(i=0; str[i+1]!='\0'; i++)
        str[i] = str[i+1];
    str[i] = '\0';
}

int lambo86_irq_proc_open(int irq)
{
	int irq_fd;
	char irq_name[] = "/proc/lambo86-irq/irq00";

	irq_name[sizeof(irq_name) - 3] = '0' + (irq / 10);
	irq_name[sizeof(irq_name) - 2] = '0' + (irq % 10);
	irq_fd = open(irq_name, O_RDWR);
	if (irq_fd <= 0) {
		//HWOP_DEBUG("open %s failed\n", irq_name);
		return -1;
	}
	return irq_fd;
}

void lambo86_irq_proc_close(int fd)
{
    close(fd);
}

int lambo86_irq_request(int fd)
{
    int ret;
    ret = ioctl(fd, LAMBO86_IOC_IRQ_REQUEST, NULL);
    return ret;
}

int lambo86_irq_free(int fd)
{
    int ret;
    ret = ioctl(fd, LAMBO86_IOC_IRQ_FREE, NULL);
    return ret;
}

/* Set hardware bufferinfo.  */
int lambo86_hwbuf_init(struct lambo86_hwmem_info *info)
{
	int ret;

	ret = ioctl(lambo_hw_info.fd, LAMBO86_IOC_MEM_HWMAP_INIT, info);
	if (ret < 0) {
		HWOP_DEBUG("%s: lambo86 sdram mmap failed \n", __FUNCTION__);
		return -1;
	}

	return 0;
}

int lambo86_hw_open(void)
{
    lambo_hw_info.fd = open("/dev/lambo86-hw", O_RDWR);
    if(lambo_hw_info.fd == -1) {
        HWOP_DEBUG("open /dev/lambo86-hw failed\n");
        return -1;
    }
    HWOP_DEBUG("open /dev/lambo86-hw success\r\n");

	return 0;
}

static inline int lambo86_mem_ctrl(int fd, int cmd, struct lambo86_mem_addr *mem)
{
	return ioctl(fd, cmd, mem);
}

int lambo86_hw_init(void)
{
    int ret;

    /* Map registers to user space */
    extern volatile RegisterFile *regs0;
    regs0 = (volatile RegisterFile *)mmap(0,
                                 (MAP_SIZE * 2),
                                 PROT_READ|PROT_WRITE,
                                 MAP_SHARED,
                                 lambo_hw_info.fd,
                                 (REG_BASE - MAP_SIZE));
    if (regs0 == MAP_FAILED) {
        HWOP_DEBUG("%s: register mmap failed \n", __FUNCTION__);
        return -1;
    }

    lambo_hw_info.regs0 = regs0;
	rgst_offset = (void *)((unsigned int)regs0 + MAP_SIZE);	// used for assembly
    HWOP_DEBUG("regs0 = 0x%p, lambo_hw_info.regs0 = 0x%p, MAP_SIZE = %dKB\n",
               regs0, lambo_hw_info.regs0, (MAP_SIZE * 2) >> 10);

    /*
	 * Fetch memory layout information.
	 */
    ret = ioctl(lambo_hw_info.fd, LAMBO86_IOC_MEM_MAP, &lambo_hw_info.meminfo);
    if (ret != 0) {
        HWOP_DEBUG("%s: lambo86 sdram mmap failed \n", __FUNCTION__);
        return -1;
    }

	lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_A].size = GSL_HEAP_A_SIZE << 20;
	//lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_B].size = GSL_HEAP_B_SIZE << 20;

	/*
	 * Get total memory reserved for hardware buffer.
	 */
	{
		unsigned int size;
		unsigned int page_size;

		page_size = sysconf(_SC_PAGE_SIZE);

		size = MCF_BufSizeAll(0);
		size = (size + (page_size - 1)) & ~(page_size - 1);
		lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_HWBUF_A].size = size;

		//size = MCF_BufSizeAll(1);
		//size = (size + (page_size - 1)) & ~(page_size - 1);
		//lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_HWBUF_B].size = size;
	}

	/*
	 * Allocate physically continuous memory.
	 */
	{
		int fd;
		int ret = 0;

		fd = open("/dev/lambo86-mem", O_RDWR);
		if (fd < 0) {
			perror("open");
		}
		ret |= lambo86_mem_ctrl(fd, LAMBO86_MEM_ALLOC_A, &lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_A]);
		//ret |= lambo86_mem_ctrl(fd, LAMBO86_MEM_ALLOC_B, &lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_B]);
		ret |= lambo86_mem_ctrl(fd, LAMBO86_MEM_ALLOC_A, &lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_HWBUF_A]);
		//ret |= lambo86_mem_ctrl(fd, LAMBO86_MEM_ALLOC_B, &lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_HWBUF_B]);

		if (ret != 0) {
			gsl_err_printf("*** Not enough memory for GSL heap and hardware buffer.\n");
			gsl_err_printf("Memory requirement:\n");
			gsl_err_printf("GSL Heap A: %u\n", lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_A].size);
			//gsl_err_printf("GSL Heap B: %u\n", lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_B].size);
			gsl_err_printf("   HWBUF A: %u\n", lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_HWBUF_A].size);
			//gsl_err_printf("   HWBUF B: %u\n", lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_HWBUF_B].size);
			gsl_err_printf("Retrieve information ...\n");
			system("cat /sys/kernel/sunplus/spmem/status");
			exit(0);
		}

		close(fd);
	}
#if 1
	{
		struct lambo86_mem_addr *mem;

		HWOP_DEBUG("%6s %10s %10s %10s %10s\n", "Name", "Cache", "Uncache", "Phys", "Size");
		HWOP_DEBUG("====================================\n");
		mem = &lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_A];
		HWOP_DEBUG("sdramA 0x%08x 0x%08x 0x%08x 0x%08x\n", (unsigned int)mem->cache, (unsigned int)mem->uncache, (unsigned int)mem->phys, mem->size);
		//mem = &lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_B];
		//HWOP_DEBUG("sdramB 0x%08x 0x%08x 0x%08x 0x%08x\n", (unsigned int)mem->cache, (unsigned int)mem->uncache, (unsigned int)mem->phys, mem->size);
		mem = &lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_HWBUF_A];
		HWOP_DEBUG("hwbufA 0x%08x 0x%08x 0x%08x 0x%08x\n", (unsigned int)mem->cache, (unsigned int)mem->uncache, (unsigned int)mem->phys, mem->size);
		//mem = &lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_HWBUF_B];
		//HWOP_DEBUG("hwbufB 0x%08x 0x%08x 0x%08x 0x%08x\n", (unsigned int)mem->cache, (unsigned int)mem->uncache, (unsigned int)mem->phys, mem->size);
	}
#endif

#ifdef FIXED_MAP
	/* Initialize heap */
	lambo_hw_info.heapA.cache = lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_A].cache;
	//lambo_hw_info.heapB.cache = lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_B].cache;

	lambo_hw_info.heapA.uncache = lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_A].uncache;
	//lambo_hw_info.heapB.uncache = lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_B].uncache;

	lambo_hw_info.heapA.phys = lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_A].phys;
	//lambo_hw_info.heapB.phys = lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_B].phys;

	lambo_hw_info.heapA.size = lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_A].size;
	//lambo_hw_info.heapB.size = lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_B].size;

	heapA_cache_end = (unsigned int)lambo_hw_info.heapA.cache + lambo_hw_info.heapA.size;
	heapA_uncache_end = (unsigned int)lambo_hw_info.heapA.uncache + lambo_hw_info.heapA.size;
	//heapB_cache_end = (unsigned int)lambo_hw_info.heapB.cache + lambo_hw_info.heapB.size;
	//heapB_uncache_end = (unsigned int)lambo_hw_info.heapB.uncache + lambo_hw_info.heapB.size;

#if 0
    lambo_hw_info.dev_mem_info[MEM_ANCHOR_OSD].usr_addr = lambo_hw_info.sdramAB_info.sdramA_cache + 0x02000000;
    lambo_hw_info.dev_mem_info[MEM_ANCHOR_OSD].phy_addr = lambo_hw_info.sdramAB_info.sdramB_phys + 0x02000000;
    lambo_hw_info.dev_mem_info[MEM_ANCHOR_OSD].size = 8220 << 10;
    lambo_hw_info.dev_mem_info[MEM_ANCHOR_OSD].type = DRAM_B;

    lambo_hw_info.dev_mem_info[MEM_ANCHOR_GPU].usr_addr = lambo_hw_info.sdramAB_info.sdramA_cache + 0x03000000;
    lambo_hw_info.dev_mem_info[MEM_ANCHOR_GPU].phy_addr = lambo_hw_info.sdramAB_info.sdramB_phys + 0x03000000;
    lambo_hw_info.dev_mem_info[MEM_ANCHOR_GPU].size = 3600 << 10;
    lambo_hw_info.dev_mem_info[MEM_ANCHOR_GPU].type = DRAM_B;
#endif
    memset(lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_A].cache, 0x55, lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_A].size);
    //memset(lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_B].cache, 0x55, lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_B].size);
    memset(lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_A].uncache, 0x55, lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_A].size);
    //memset(lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_B].uncache, 0x55, lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_B].size);
#else
    lambo_hw_info.heapA_cache = (char *)MCF_BufAlloc(MEM_ANCHOR_HEAP_A);
    //lambo_hw_info.heapB_cache = (char *)MCF_BufAlloc(MEM_ANCHOR_HEAP_B);

    lambo_hw_info.heapA_uncache = (lambo_hw_info.heapA_cache - lambo_hw_info.sdramAB_info.sdramA_cache) + lambo_hw_info.sdramAB_info.sdramA_uncache;
    //lambo_hw_info.heapB_uncache = (lambo_hw_info.heapB_cache - lambo_hw_info.sdramAB_info.sdramB_cache) + lambo_hw_info.sdramAB_info.sdramB_uncache;

    lambo_hw_info.heapA_phys = (lambo_hw_info.heapA_cache - lambo_hw_info.sdramAB_info.sdramA_cache) + lambo_hw_info.sdramAB_info.sdramA_phys;
    //lambo_hw_info.heapB_phys = (lambo_hw_info.heapB_cache - lambo_hw_info.sdramAB_info.sdramB_cache) + lambo_hw_info.sdramAB_info.sdramB_phys;

    lambo_hw_info.heapA_size = (unsigned int)lambo_hw_info.sdramAB_info.sdramA_cache + lambo_hw_info.sdramAB_info.sdramA_size - (unsigned int)lambo_hw_info.heapA_cache;
    //lambo_hw_info.heapB_size = (unsigned int)lambo_hw_info.sdramAB_info.sdramB_cache + lambo_hw_info.sdramAB_info.sdramB_size - (unsigned int)lambo_hw_info.heapB_cache;

#endif
	{
		struct lambo86_mem_addr *mem;

		mem = &lambo_hw_info.heapA;
    	HWOP_DEBUG(" heapA 0x%08x 0x%08x 0x%08x 0x%08x\n", (unsigned int)mem->cache, (unsigned int)mem->uncache, (unsigned int)mem->phys, mem->size);
		//mem = &lambo_hw_info.heapB;
    	//HWOP_DEBUG(" heapB 0x%08x 0x%08x 0x%08x 0x%08x\n", (unsigned int)mem->cache, (unsigned int)mem->uncache, (unsigned int)mem->phys, mem->size);
    	HWOP_DEBUG("====================================\n");
	}

    return 0;
}

int lambo86_hw_release(void)
{
    munmap((void *)lambo_hw_info.regs0, MAP_SIZE);

	/*
	 * Free phsycally continuous memory.
	 */
	{
		int fd;

		fd = open("/dev/lambo86-mem", O_RDWR);
		if (fd < 0) {
			perror("open");
		}
		lambo86_mem_ctrl(fd, LAMBO86_MEM_FREE, &lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_A]);
		//lambo86_mem_ctrl(fd, LAMBO86_MEM_FREE, &lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_B]);
		lambo86_mem_ctrl(fd, LAMBO86_MEM_FREE, &lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_HWBUF_A]);
		//lambo86_mem_ctrl(fd, LAMBO86_MEM_FREE, &lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_HWBUF_B]);
		close(fd);
	}

    close(lambo_hw_info.fd);
    return 0;
}

void lambo86_hw_get_meminfo(struct lambo86_mem_info *info)
{
	if (NULL == info) {
		return;
	}

	memcpy(info, &lambo_hw_info.meminfo, sizeof(*info));
}

int lambo86_cache_flush(void *start, unsigned int size)
{
	struct cache_op_info info;

	if (!CHECK_IF_ADDR_IN_RANGE((unsigned int)start)) {
		HWOP_DEBUG("Address(0x%08X) isn't in range! ra = 0x%08X\n",
				   (unsigned int)start, (unsigned int)__builtin_return_address(0));
		return -1;
	}

	info.addr = start;
	info.size = size;

	return ioctl(lambo_hw_info.fd, LAMBO86_IOC_CACHE_FLUSH, &info);
}

int lambo86_cache_inva(void *start, unsigned int size)
{
	struct cache_op_info info;

	if (!CHECK_IF_ADDR_IN_RANGE((unsigned int)start)) {
		HWOP_DEBUG("Address(0x%08X) isn't in range! ra = 0x%08X\n",
					(unsigned int)start, (unsigned int)__builtin_return_address(0));
		return -1;
	}

	info.addr = start;
	info.size = size;

	return ioctl(lambo_hw_info.fd, LAMBO86_IOC_CACHE_INVA, &info);
}

int lambo86_cache_flush_inva(void *start, unsigned int size)
{
	struct cache_op_info info;

	if (!CHECK_IF_ADDR_IN_RANGE((unsigned int)start)) {
		HWOP_DEBUG("Address(0x%08X) isn't in range! ra = 0x%08X\n",
				   (unsigned int)start, (unsigned int)__builtin_return_address(0));
		return -1;
	}

	info.addr = start;
	info.size = size;

	return ioctl(lambo_hw_info.fd, LAMBO86_IOC_CACHE_FLUSH_INVA, &info);
}


static inline int addr_in_range(unsigned int start, unsigned int size, unsigned int addr)
{
	if ((start <= addr) && (addr < (start + size))) {
		return true;
	}
	return false;
}

static int addr_in_range_cache(struct lambo86_mem_addr *mem, unsigned int addr, unsigned int *offset)
{
	if (((unsigned int)mem->cache <= addr) && (addr < ((unsigned int)mem->cache + mem->size))) {
		*offset = addr - (unsigned int)mem->cache;
		return true;
	}
	return false;
}

static int addr_in_range_uncache(struct lambo86_mem_addr *mem, unsigned int addr, unsigned int *offset)
{
	if (((unsigned int)mem->uncache <= addr) && (addr < ((unsigned int)mem->uncache + mem->size))) {
		*offset = addr - (unsigned int)mem->uncache;
		return true;
	}
	return false;
}

static int addr_in_range_phys(struct lambo86_mem_addr *mem, unsigned int addr, unsigned int *offset)
{
	if (((unsigned int)mem->phys <= addr) && (addr < ((unsigned int)mem->phys + mem->size))) {
		*offset = addr - (unsigned int)mem->phys;
		return true;
	}
	return false;
}

void *lambo_memaddrswitch(void *addr, int intype, int outtype)
{
	struct lambo86_mem_addr *mem;
	int (*cmp_func)(struct lambo86_mem_addr *, unsigned int, unsigned int *);
	int memtype;
	unsigned int offset;
	char *retstartaddr;

	switch (intype) {
	case LAMBO_ADDR_TYPE_CACHE:
		cmp_func = addr_in_range_cache;
		break;
	case LAMBO_ADDR_TYPE_UNCACHE:
		cmp_func = addr_in_range_uncache;
		break;
	case LAMBO_ADDR_TYPE_PHYSICAL:
		cmp_func = addr_in_range_phys;
		break;
	default:
		return NULL;
	}

	for (memtype = LAMBO_MEM_ADDR_START; memtype < LAMBO_MEM_ADDR_END; ++memtype) {
		mem = &lambo_hw_info.meminfo.mem[memtype];
		if (true == cmp_func(mem, (unsigned int)addr, &offset)) {
			break;
		}
	}

	if (LAMBO_MEM_ADDR_END == memtype) {
		return NULL;
	}

	switch (outtype) {
	case LAMBO_ADDR_TYPE_CACHE:
		retstartaddr = lambo_hw_info.meminfo.mem[memtype].cache;
		break;
	case LAMBO_ADDR_TYPE_UNCACHE:
		retstartaddr = lambo_hw_info.meminfo.mem[memtype].uncache;
		break;
	case LAMBO_ADDR_TYPE_PHYSICAL:
		retstartaddr = lambo_hw_info.meminfo.mem[memtype].phys;
		break;
	default:
		return NULL;
	}

	return (void *)(retstartaddr + offset);
}

void *lambo_memaddrswitch_ex(void *addr, int outtype)
{
	struct lambo86_mem_addr *mem;
	int memtype;
	unsigned int offset;
	char *retstartaddr;

	for (memtype = LAMBO_MEM_ADDR_START; memtype < LAMBO_MEM_ADDR_END; ++memtype) {
		mem = &lambo_hw_info.meminfo.mem[memtype];
		if (addr_in_range((unsigned int)mem->cache, mem->size, (unsigned int)addr)) {
			offset = (unsigned int)addr - (unsigned int)mem->cache;
			break;
		} else if (addr_in_range((unsigned int)mem->uncache, mem->size, (unsigned int)addr)) {
			offset = (unsigned int)addr - (unsigned int)mem->uncache;
			break;
		} else if (addr_in_range((unsigned int)mem->phys, mem->size, (unsigned int)addr)) {
			offset = (unsigned int)addr - (unsigned int)mem->phys;
			break;
		}
	}

	if (LAMBO_MEM_ADDR_END == memtype) {
		return NULL;
	}

	switch (outtype) {
	case LAMBO_ADDR_TYPE_CACHE:
		retstartaddr = lambo_hw_info.meminfo.mem[memtype].cache;
		break;
	case LAMBO_ADDR_TYPE_UNCACHE:
		retstartaddr = lambo_hw_info.meminfo.mem[memtype].uncache;
		break;
	case LAMBO_ADDR_TYPE_PHYSICAL:
		retstartaddr = lambo_hw_info.meminfo.mem[memtype].phys;
		break;
	default:
		return NULL;
	}

	return (void *)(retstartaddr + offset);
}

int lambo86_memaddr_check(void *addr, int intype)
{
	struct lambo86_mem_addr *mem;
	int (*cmp_func)(struct lambo86_mem_addr *, unsigned int, unsigned int *);
	int memtype;
	unsigned int dummy;

	switch (intype) {
	case LAMBO_ADDR_TYPE_CACHE:
		cmp_func = addr_in_range_cache;
		break;
	case LAMBO_ADDR_TYPE_UNCACHE:
		cmp_func = addr_in_range_uncache;
		break;
	case LAMBO_ADDR_TYPE_PHYSICAL:
		cmp_func = addr_in_range_phys;
		break;
	default:
		return false;
	}

	for (memtype = LAMBO_MEM_ADDR_START; memtype < LAMBO_MEM_ADDR_END; ++memtype) {
		mem = &lambo_hw_info.meminfo.mem[memtype];
		if (true == cmp_func(mem, (unsigned int)addr, &dummy)) {
			return true;
		}
	}
	return false;
}
