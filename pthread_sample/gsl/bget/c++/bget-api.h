/******************************************
 * bget api for sphe8600
 *
 *****************************************/
#ifndef __BGET_API_H__
#define __BGET_API_H__

typedef int bget_handle_t;
typedef long bget_bufsize_t;
#define BGET_MAX_POOL_NO 4
#define BGET_FAIL (-1) // return value when bget_create_pool fail


#ifdef __cplusplus
extern "C"
{
#endif

bget_handle_t bget_create_pool(void *buffer, bget_bufsize_t len);
void bget_delete_pool(bget_handle_t handle);
void *bget_get(bget_handle_t handle, bget_bufsize_t size); // alloc buffer from pool indicated by handle
void *bget_getz(bget_handle_t handle, bget_bufsize_t size); // alloc buffer from pool indicated by handle, and set 0
void *bget_getr(bget_handle_t handle, void *buffer, bget_bufsize_t newsize); //realloc buffer from pool indicated by handle
void bget_brel(bget_handle_t handle, void *buf);
void bget_bstats(bget_handle_t handle, bget_bufsize_t *curalloc, bget_bufsize_t *totfree, bget_bufsize_t *maxfree, long *nget, long *nrel);
void bget_bpoold(bget_handle_t handle, void *pool, int dumpalloc, int dumpfree);
int bget_test(bget_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif

