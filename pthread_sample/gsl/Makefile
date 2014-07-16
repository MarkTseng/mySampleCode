TOP ?= ..

include $(TOP)/make.include

#---------------------------------------------------------------------------
# Target definition (User)
TARGET  = $(LIB_DIR)/libgsl


#---------------------------------------------------------------------------
# Source section (User)
#---------------------------------------------------------------------------
SOURCES		+= lambo86_hwop.c
SOURCES		+= gl_common_dbg.c
SOURCES		+= gl_sema.c
SOURCES		+= gl_mutex.c
SOURCES		+= gl_queue.c
SOURCES		+= gl_list.c
SOURCES		+= gl_cond.c
SOURCES		+= gl_flag.c
SOURCES		+= bget/bget_malloc.c
SOURCES		+= gl_malloc.c
SOURCES		+= gl_mempool.c
SOURCES		+= gl_kmempool.c
SOURCES		+= gl_task.c
SOURCES		+= gsl_os.c
SOURCES		+= posix/gl_pthread.c
SOURCES		+= gl_times.c
SOURCES		+= gl_barrier.c

SOURCES		+= gl_time.S

SOURCES		+= gl_timer.c
SOURCES		+= timestamp_diag.c
SOURCES		+= gl_cache.c
SOURCES		+= gl_dma.c
SOURCES		+= gl_modctrl.c
SOURCES		+= gl_endian.S
SOURCES		+= gl_intr.c
SOURCES		+= gl_proc.c
SOURCES		+= gl_except.c
SOURCES		+= gl_systime.c
SOURCES		+= gl_tset.c
SOURCES		+= gl_bw.c

ifdef CONFIG_MEMORY_WATCH
SOURCES		+= memwatch.c
endif

### Not support ###
SOURCES		+= gl_i2c.c
#SOURCES		+= gl_timer_var.c
#SOURCES		+= buf_uart.c
#SOURCES		+= gl_bit_op.c
#SOURCES		+= gl_bitop.c
SOURCES		+= gl_dmls.c
#SOURCES		+= gl_perfcnt.c
#SOURCES		+= os_mon.c
#SOURCES		+= profile.c
#SOURCES		+= stack_check.c
#SOURCES		+= gl_mem_fence.c
#SOURCES		+= taskpool_impl.c

SOURCES		+= bget/c++/bget.cpp


SHARED_LIB=1
include $(TOP)/Makefile.in

