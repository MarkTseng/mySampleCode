/*                                                     
 * $Id: hello.c,v 1.7 2000/08/17 11:11:48 rubini Exp $
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
int helloi;
