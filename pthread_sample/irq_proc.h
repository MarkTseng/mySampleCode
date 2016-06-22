#ifndef _H_IRQ_PROC_H_
#define _H_IRQ_PROC_H_

/* User-mode isr iotctl.  */
#define LAMBO86_IRQ_IOC_MAGIC 'I'
#define LAMBO86_IOC_IRQ_REQUEST			_IOWR(LAMBO86_IRQ_IOC_MAGIC, 0, int)
#define LAMBO86_IOC_IRQ_FREE			_IOWR(LAMBO86_IRQ_IOC_MAGIC, 1, int)

#define LAMBO86_IRQ_IOC_MAXNR 2

int gl_irq_proc_init(void);
void gl_irq_proc_exit(void);

#endif // _H_IRQ_PROC_H_

