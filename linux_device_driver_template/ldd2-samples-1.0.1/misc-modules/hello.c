/*                                                     
 * $Id: hello.c,v 1.10 2001/07/17 10:30:02 rubini Exp $ 
 */                                                    
#define MODULE
#include <linux/module.h>

/*                                                        
 * These lines, although not shown in the book,           
 * are needed to make hello.c run properly even when      
 * your kernel has version support enabled                
 */                                                       
                                                          
int init_module(void)      { printk("<1>Hello, world\n"); return 0; }
void cleanup_module(void)  { printk("<1>Goodbye cruel world\n"); }
