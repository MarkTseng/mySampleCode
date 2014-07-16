################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../buf_uart.c \
../gl_barrier.c \
../gl_bit_op.c \
../gl_bitop.c \
../gl_cache.c \
../gl_common_dbg.c \
../gl_cond.c \
../gl_dma.c \
../gl_dmls.c \
../gl_error.c \
../gl_except.c \
../gl_flag.c \
../gl_i2c.c \
../gl_intr.c \
../gl_kmempool.c \
../gl_list.c \
../gl_malloc.c \
../gl_mem_fence.c \
../gl_mempool.c \
../gl_modctrl.c \
../gl_mutex.c \
../gl_perfcnt.c \
../gl_queue.c \
../gl_sema.c \
../gl_task.c \
../gl_timer.c \
../gl_timer_var.c \
../gl_times.c \
../gsl_os.c \
../gsl_test.c \
../os_mon.c \
../profile.c \
../stack_check.c \
../taskpool_impl.c \
../timestamp_diag.c 

S_UPPER_SRCS += \
../gl_endian.S \
../gl_time.S 

OBJS += \
./buf_uart.o \
./gl_barrier.o \
./gl_bit_op.o \
./gl_bitop.o \
./gl_cache.o \
./gl_common_dbg.o \
./gl_cond.o \
./gl_dma.o \
./gl_dmls.o \
./gl_endian.o \
./gl_error.o \
./gl_except.o \
./gl_flag.o \
./gl_i2c.o \
./gl_intr.o \
./gl_kmempool.o \
./gl_list.o \
./gl_malloc.o \
./gl_mem_fence.o \
./gl_mempool.o \
./gl_modctrl.o \
./gl_mutex.o \
./gl_perfcnt.o \
./gl_queue.o \
./gl_sema.o \
./gl_task.o \
./gl_time.o \
./gl_timer.o \
./gl_timer_var.o \
./gl_times.o \
./gsl_os.o \
./gsl_test.o \
./os_mon.o \
./profile.o \
./stack_check.o \
./taskpool_impl.o \
./timestamp_diag.o 

C_DEPS += \
./buf_uart.d \
./gl_barrier.d \
./gl_bit_op.d \
./gl_bitop.d \
./gl_cache.d \
./gl_common_dbg.d \
./gl_cond.d \
./gl_dma.d \
./gl_dmls.d \
./gl_error.d \
./gl_except.d \
./gl_flag.d \
./gl_i2c.d \
./gl_intr.d \
./gl_kmempool.d \
./gl_list.d \
./gl_malloc.d \
./gl_mem_fence.d \
./gl_mempool.d \
./gl_modctrl.d \
./gl_mutex.d \
./gl_perfcnt.d \
./gl_queue.d \
./gl_sema.d \
./gl_task.d \
./gl_timer.d \
./gl_timer_var.d \
./gl_times.d \
./gsl_os.d \
./gsl_test.d \
./os_mon.d \
./profile.d \
./stack_check.d \
./taskpool_impl.d \
./timestamp_diag.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler'
	as  -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


