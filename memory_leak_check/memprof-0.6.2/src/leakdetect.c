/* -*- mode: C; c-file-style: "linux" -*- */

/* MemProf -- memory profiler and leak detector
 * Copyright 1999, 2000, 2001, Red Hat, Inc.
 * Copyright 2002, Kristian Rietveld
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
/*====*/

#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>
#include <glib/gi18n.h>

#include "memprof.h"
#include "process.h"

static void
prepare_block (Block *block, gpointer data)
{
	GPtrArray *arr = data;

	g_assert (!(block->flags & BLOCK_IS_ROOT));
	block->flags &= ~BLOCK_MARKED;
	block->refcount++;
	g_ptr_array_add (arr, block);
}

static gint
compare_blocks (const void *a, const void *b)
{
	Block * const *b1 = a;
	Block * const *b2 = b;

	return ((*b1)->addr < (*b2)->addr) ? -1 : 
		((*b1)->addr == (*b2)->addr ? 0 : 1);
}

static gboolean
read_proc_stat (int pid, char *status, gsize *start_stack, gsize *end_stack)
{
	gchar *fname;
	gulong tstart_stack;
	gulong tend_stack;
	char tstatus;
	FILE *in;
  
	fname = g_strdup_printf ("/proc/%d/stat", pid);
	in = fopen (fname, "r");
	if (!in) {
		g_warning ("Can't open %s\n", fname);
		return FALSE;
	}
	g_free (fname);

/* The fields in proc/stat are:
 * 		pid 
 * 		tsk->comm 
 * 		state 
 * 		tsk->p_pptr->pid 
 * 		tsk->pgrp 
 * 		tsk->session 
 * 	        tsk->tty ? kdev_t_to_nr(tsk->tty->device) : 0 
 * 		tty_pgrp 
 * 		tsk->flags 
 * 		tsk->min_flt 

 * 		tsk->cmin_flt 
 * 		tsk->maj_flt 
 * 		tsk->cmaj_flt 
 * 		tsk->utime 
 * 		tsk->stime 
 * 		tsk->cutime 
 * 		tsk->cstime 
 * 		priority 
 * 		nice 
 * 		tsk->timeout 

 * 		tsk->it_real_value 
 * 		tsk->start_time 
 * 		vsize 
 * 		tsk->mm ? tsk->mm->rss : 0 
 * 		tsk->rlim ? tsk->rlim[RLIMIT_RSS].rlim_cur : 0 
 * 		tsk->mm ? tsk->mm->start_code : 0 
 * 		tsk->mm ? tsk->mm->end_code : 0 
 * 		tsk->mm ? tsk->mm->start_stack : 0 
 * 		esp 
 * 		eip 

 * 		tsk->signal 
 * 		tsk->blocked 
 * 		sigignore 
 * 		sigcatch 
 * 		wchan 
 * 		tsk->nswap 
 * 		tsk->cnswap); 
 if (fscanf(in,"%*d %*s %c %*d %*d %*d %*d %*d %*lu %*lu \
 %*lu %*lu %*lu %*lu %*lu %*ld %*ld %*ld %*ld %*ld \
 %*ld %*lu %*lu %*ld %*lu %*lu %*lu %lu %lu %*lu \
 %*lu %*lu %*lu %*lu %*lu %*lu %*lu",
*/
	if (fscanf(in,"%*d %*s %c %*d %*d %*d %*d %*d %*u %*u \
%*u %*u %*u %*u %*u %*d %*d %*d %*d %*d \
%*d %*u %*u %*d %*u %*u %*u %lu %lu %*u \
%*u %*u %*u %*u %*u %*u %*u",
		   &tstatus,
		   &tstart_stack,
		   &tend_stack) != 3) {
		g_warning ("Error parsing /proc/%d/stat\n", pid);
		return FALSE;
	}

	if (status)
		*status = tstatus;
	if (start_stack)
		*start_stack = tstart_stack;
	if (end_stack)
		*end_stack = tend_stack;

	fclose (in);

	return TRUE;
}

static char
process_status (int pid)
{
	char c = 'T';		/* Avoid infinite loop in caller */

	read_proc_stat (pid, &c, NULL, NULL);

	return c;
}

/* FIXME: merge with read_process_maps */
static GList *
read_stack_maps (MPProcess *process)
{
	GList *result = NULL;
	gchar buffer[1024];
	FILE *in;
	gchar perms[26];
	gchar file[256];
	gsize start, end, major, minor, inode;
  
	snprintf (buffer, 1023, "/proc/%d/maps", process->pid);

	in = fopen (buffer, "r");
	if (!in)
		return NULL;

	while (fgets(buffer, 1023, in)) {
		int count = sscanf (buffer, "%x-%x %15s %*x %x:%x %u %255s",
				    &start, &end, perms, &major, &minor, &inode, file);
		if (count >= 6)	{
			if (strcmp (perms, "rwxp") == 0) {
				Map *map;

				map = g_new (Map, 1);
				map->start = start;
				map->size = end - start;
				map->name = NULL;
				map->offset = 0;

				result = g_list_prepend (result, map);
			}
		}
	}

	fclose (in);

	return result;
}

static GSList *
add_stack_root (MPProcess *process, GSList *block_list,
		GList *map_list)
{
	GList *tmp_list;
	gsize start_stack, end_stack;

	tmp_list = map_list;

	if (!read_proc_stat (process->pid, NULL, &start_stack, &end_stack))
		return block_list;

	while (tmp_list) {
		Map *map = tmp_list->data;

		if (end_stack >= map->start &&
		    end_stack < map->start + map->size) {
			Block *block;
			
			block = g_new (Block, 1);
			block->refcount = 1;
			block->flags = BLOCK_IS_ROOT;
			block->addr = (void *)end_stack;
			if (start_stack > map->start &&
			    start_stack < map->start + map->size)
				block->size = start_stack - end_stack;
			else
				block->size = map->start + map->size - end_stack;
			block->stack = NULL;

			return g_slist_prepend (block_list, block);
		}

		tmp_list = tmp_list->next;
	}

	return block_list;
}

static GSList *
add_stack_roots (MPProcess *process, GSList *block_list)
{
	GList *clones, *tmp_list;
	GList *stack_maps;

	clones = process_get_clones (process);
	stack_maps = read_stack_maps (process);

	tmp_list = clones;
	while (tmp_list) {
		block_list = add_stack_root (tmp_list->data, block_list, stack_maps);
		tmp_list = tmp_list->next;
	}
	g_list_free (clones);

	g_list_foreach (stack_maps, (GFunc)g_free, NULL);
	g_list_free (stack_maps);

	return block_list;
}

void
process_data_root (void *addr, guint size, gpointer user_data)
{
	GSList **block_list = user_data;
	Block *block;
  
	block = g_new (Block, 1);
	block->refcount = 1;
	block->flags = BLOCK_IS_ROOT;
	block->addr = addr;
	block->size = size;
	block->stack = NULL;

	*block_list = g_slist_prepend (*block_list, block);
}

static GSList *
add_data_roots (MPProcess *process, GSList *block_list)
{
	process_sections (process, process_data_root, &block_list);

	return block_list;
}

static Block *
find_block (GPtrArray *block_arr, void *addr)
{
	Block **data;
	guint first, middle, last;

	if (block_arr->len == 0)
		return NULL;
	
	first = 0;
	last = block_arr->len - 1;
	middle = last;

	data = (Block **)block_arr->pdata;

	if (addr < data[first]->addr) {
		return NULL;
	}
	else if (addr < data[last]->addr) {
		/* Invariant: data[first].addr <= val < data[last].addr */

		while (first < last - 1) {
			middle = (first + last) / 2;
			if (addr < data[middle]->addr) 
				last = middle;
			else
				first = middle;
		}
		if (addr < data[first]->addr + data[first]->size)
			return data[first];
		else
			return NULL;
	} else {
		if (addr < data[last]->addr + data[last]->size)
			return data[last];
		else
			return NULL;
	}
}

static GSList *
scan_block_contents (GPtrArray *block_arr,
		     GSList *block_list,
		     Block *block, void **mem)
{
	int i;
  
	for (i=0; i < block->size / sizeof(void *); i++) {
		Block *new_block;

		new_block = find_block (block_arr, mem[i]);

		g_assert (!new_block || !(new_block->flags & BLOCK_IS_ROOT));

		if (new_block && !(new_block->flags & BLOCK_MARKED)) {
			block_list = g_slist_prepend (block_list, new_block);
			new_block->flags |= BLOCK_MARKED;
		}
	}

	return block_list;
}

static GSList *
scan_block (pid_t pid, int memfd, GSList *block_list,
	    GPtrArray *block_arr, Block *block)
{
	void **mem;
	gsize i;
	void *addr;
	gsize length = (block->size + 3) / 4;

	addr = g_new (void *, length);
	mem = (void **)addr;

	for (i = 0; i < length; i++) {
		mem[i] = (void *)ptrace (PTRACE_PEEKDATA, pid,
					 block->addr+i*sizeof(void *),
					 &mem[i]);
		if (errno)
		{
			g_warning ("Cannot read word %zd/%zd in block %p: %s\n",
				   i, length, block->addr, g_strerror (errno));
			g_free (addr);
			return block_list;
		}
	}
  
	block_list = scan_block_contents (block_arr, block_list,
					  block, mem);

	g_free (addr);
	return block_list;
}

GSList *
leaks_find (MPProcess *process)
{
	int i;
	GPtrArray *block_arr;
	GSList *block_list = NULL;
	GSList *result = NULL;
	int memfd;
	gchar *fname;
	GList *clones, *tmp_list;

	clones = process_get_clones (process);

	ptrace (PTRACE_ATTACH, process->pid, 0, 0);

	tmp_list = clones;
	while (tmp_list) {
		MPProcess *clone = tmp_list->data;
	  
		kill (clone->pid, SIGSTOP);

		/* Wait for the processes we are tracing to actually stop */

		/* waitpid(clone->pid, &status, WUNTRACED); */
		while (process_status (clone->pid) != 'T') {
			usleep(50000);
		}

		tmp_list = tmp_list->next;
	}

	fname = g_strdup_printf ("/proc/%d/mem", process->pid);
	memfd = open (fname, O_RDONLY);
	if (memfd < 0) {
		g_warning ("Can't open %s\n", fname);
		return NULL;
	}
  
	g_free (fname);

	/* Mark all blocks as untouched, add them to list of blocks
	 */

	block_arr = g_ptr_array_new ();
	process_block_foreach (process, prepare_block, block_arr);

	qsort (block_arr->pdata, block_arr->len, sizeof (Block *),
	       compare_blocks);
  
	/* Locate all the roots
	 */

	block_list = add_stack_roots (process, block_list);
	block_list = add_data_roots (process, block_list);

	/* While there are blocks to check, scan each block,
	 * and add each new-found block to the global list.
	 */

	while (block_list) {
		GSList *tmp_list;
		Block *block = block_list->data;

		tmp_list = block_list->next;
		g_slist_free_1 (block_list);
		block_list = tmp_list;

		block_list = scan_block (process->pid, memfd, block_list, block_arr, block);

#if 0
		if (block->flags & BLOCK_IS_ROOT)
			g_free (block);
#endif  
	}

	close (memfd);

	/* Look for leaked blocks
	 */

	for (i=0; i<block_arr->len; i++) {
		Block *block = block_arr->pdata[i];
		if (!(block->flags & BLOCK_MARKED)) {
			result = g_slist_prepend (result, block);
		} else {
			block_unref(block);
		}
	}

	tmp_list = clones;
	while (tmp_list) {
		MPProcess *clone = tmp_list->data;
	  
		kill (clone->pid, SIGCONT);

		tmp_list = tmp_list->next;
	}

	/* Clean up
	 */
	g_list_free (clones);
  
	g_ptr_array_free (block_arr, FALSE);

	close (memfd);
	if (ptrace (PTRACE_DETACH, process->pid, 0, 0) == -1)
		g_warning ("Detach failed %s (%d)\n", g_strerror (errno), errno);

	return result;
}

void
leaks_print (MPProcess *process, GSList *blocks, gchar *outfile)
{
	GSList *tmp_list = blocks;
	FILE *out;

	out = fopen (outfile, "w");
	if (!out)
	{
		show_error (NULL, ERROR_MODAL,
			    _("Cannot open output file: %s\n"),
			    g_strerror (errno));
		return;
	}

	while (tmp_list)
	{
		Block *block = tmp_list->data;
      
		fprintf (out, "Leaked %p (%u bytes)\n", block->addr, block->size);
		process_dump_stack (process, out, block->stack);

		tmp_list = tmp_list->next;
	}
  
	fclose (out);
}
