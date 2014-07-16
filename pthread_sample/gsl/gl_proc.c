#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bfd.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <ucontext.h>
#include <gsl/gl_proc.h>

#define	DUMPALLLIBADDR
#define MAXIMUM_BACK_STACK_COUNT	100

typedef struct function_table FUN_TABLE;

struct function_table {
	char name[80];
	unsigned long addr;
};

static FUN_TABLE *fun_table;
static int table_count = 0;   

int compare_function(const void *a, const void *b)
{
	FUN_TABLE *aa = (FUN_TABLE *) a, *bb = (FUN_TABLE *) b;
	if (aa->addr > bb->addr)
		return 1;
	else if (aa->addr < bb->addr)
		return -1;
	return 0;  
}

void add_function_table(char *name, unsigned long address)
{
	strncpy(fun_table[table_count].name, name, 80);
	fun_table[table_count].addr = address;
	table_count++;  
}

void dump_function_table(unsigned long offset)
{
	int i;
#ifndef DUMPALLLIBADDR
	unsigned long addr = offset;
	int counter = 0, libindex = 0;
#endif

#ifdef DUMPALLLIBADDR
	for (i = 0; i < table_count; i++) {
		if(fun_table[i].addr != 0x00)
		{
			printf("%-30s 0x%lx\n", fun_table[i].name, fun_table[i].addr);
		}
	}
#else
	while(1)
	{
		if(counter > MAXIMUM_BACK_STACK_COUNT)
			break;

		for (i = 0; i < table_count; i++) 
		{
			if(fun_table[i].addr == addr)
			{	
				libindex = i;
				break;
			}
		}

		addr-=0x04;
		counter++;
	}
	printf("        exception at [%s] + 0x%lx\n", fun_table[libindex].name, offset - fun_table[libindex].addr);
#endif
}

int openlib(char *libname, unsigned long offset)
{
	bfd *abfd;
	long storage_needed;
	asymbol **symbol_table;
	long number_of_symbols;
	long i;
	char **matching;
	sec_ptr section;
	char *symbol_name;
	long symbol_offset, section_vma, symbol_address;

	bfd_init();
	abfd = bfd_openr(libname,NULL);
	if (abfd == (bfd *) 0) {
		bfd_perror("bfd_openr");
		return -1;
	}
	if (!bfd_check_format_matches(abfd, bfd_object, &matching)) {
		return -1;
	}      
	if (!(bfd_get_file_flags (abfd) & HAS_SYMS)) {
		printf("ERROR flag!\n");
		return -1;
	}
	if ((storage_needed = bfd_get_symtab_upper_bound(abfd)) < 0)
		return -1;
	symbol_table = (asymbol **) malloc(storage_needed);
	number_of_symbols = bfd_canonicalize_symtab(abfd, symbol_table);
	if (number_of_symbols < 0)
		return -1;
	fun_table = (FUN_TABLE *) malloc(sizeof(FUN_TABLE)*number_of_symbols);
	bzero(fun_table, sizeof(FUN_TABLE)*number_of_symbols);

	for (i = 0; i < number_of_symbols; i++) {
		if (symbol_table[i]->flags & (BSF_FUNCTION|BSF_GLOBAL)) {
			section = symbol_table[i]->section;
			section_vma = bfd_get_section_vma(abfd, section);
			symbol_name = (char *)symbol_table[i]->name;
			symbol_offset = symbol_table[i]->value;
			symbol_address = section_vma + symbol_offset;
			if (section->flags & SEC_CODE) {
				add_function_table(symbol_name,
				                   symbol_address);
			}
		}
	}
	bfd_close(abfd);
	qsort(fun_table, table_count, sizeof(FUN_TABLE), compare_function);
	dump_function_table(offset);
	return 0;
}

struct gsl_proc_maps *GL_ProcMapsParsePid(pid_t pid)
{
	char buf[256];
	struct gsl_proc_maps *map;
	struct gsl_proc_maps *head = NULL;
	FILE *fp;

	snprintf(buf, sizeof(buf), "/proc/%u/maps", (unsigned int)pid);

	fp = fopen(buf, "r");
	if (!fp) {
		printf("Error: open file \"%s\" failed\n", buf);
		return NULL;
	}
	map = NULL;
	while (0 != fgets(buf, sizeof(buf), fp)) {
		head = (struct gsl_proc_maps *)malloc(sizeof(*head));
		if (!head) {
			printf("%s:%d: insufficient memory.\n", __FUNCTION__, __LINE__);
			goto __exit;
		}
		head->next = map;
		map = head;
		sscanf(buf, "%x-%x %4s %x %hx:%hx %u %s",
			   &map->vm_start, &map->vm_end, &map->perm_str[0], &map->offset,
			   &map->dev.major, &map->dev.minor, &map->inode, &map->name[0]);
	}
__exit:
	fclose(fp);

	return head;
}

void GL_ProcMapsFree(struct gsl_proc_maps *map)
{
	struct gsl_proc_maps *tmp;

	while (NULL != map) {
		tmp = map->next;
		free(map);
		map = tmp;
	}
}

void GL_ProcMapsDumpMatches(pid_t pid, int n, ...)
{
	int i;
	va_list ap;
	char buf[256];
	unsigned int addr[8];
	struct gsl_proc_maps entry;
	struct gsl_proc_maps *map;
	FILE *fp;

	if ((unsigned int)n >= (sizeof(addr) / sizeof(addr[0]))) {
		return;
	}

	va_start(ap, n);
	for (i = 0; i < n; ++i) {
		addr[i] = va_arg(ap, unsigned int);
	}

	snprintf(buf, sizeof(buf), "/proc/%u/maps", (unsigned int)pid);
	fp = fopen(buf, "r");
	if (!fp) {
		printf("Error: open file \"%s\" failed\n", buf);
		return;
	}
	map = &entry;
	while (0 != fgets(buf, sizeof(buf), fp)) {
		int flag = 0;

		sscanf(buf, "%x-%x %4s %x %hx:%hx %u %s",
			   &map->vm_start, &map->vm_end, &map->perm_str[0], &map->offset,
			   &map->dev.major, &map->dev.minor, &map->inode, &map->name[0]);

		for (i = 0; i < n; ++i) {
			if ((addr[i] >= map->vm_start) && (addr[i] < map->vm_end)) {
				printf("--> ");
				flag = 1;
				break;
			}
		}
		if (!flag) {
			printf("    ");
		}
		printf("%08x-%08x %4s %08x %02hx:%02hx %8u %s\n", map->vm_start, map->vm_end, map->perm_str, map->offset,
				map->dev.major, map->dev.minor, map->inode, map->name);
		if (flag) {
			printf("        offset = 0x%08X\n", addr[i] - map->vm_start);
			openlib(map->name, addr[i] - map->vm_start);
		}
	}
	fclose(fp);
	va_end(ap);
}

struct gsl_proc_maps const *GL_ProcMapsFindAddr(struct gsl_proc_maps const *map, unsigned int addr)
{
	struct gsl_proc_maps const *m = NULL;

	while (NULL != map) {
		if ((addr >= map->vm_start) && (addr < map->vm_end)) {
			m = map;
			break;
		}
		map = map->next;
	}

	return m;
}

