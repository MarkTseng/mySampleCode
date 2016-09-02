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

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/sysmacros.h>

#include <glib-object.h>

#include "memintercept.h"
#include "memprof.h"
#include "process.h"
#include "server.h"
#include <glib/gi18n.h>

enum {
	STATUS_CHANGED,
	RESET,
	LAST_SIGNAL
};
static guint process_signals[LAST_SIGNAL] = { 0 };

static void mp_process_class_init (MPProcessClass *class);
static void mp_process_init (MPProcess *process);
static void mp_process_finalize (GObject *object);
static void process_reinit (MPProcess *process);

#define MP_PAGE_SIZE 4096

/* Code to keep a queue of commands read
 */
typedef struct {
	MIInfo info;
	StackNode *stack;
} Command;

static gint
queue_compare (gconstpointer a, gconstpointer b)
{
	const Command *commanda = a;
	const Command *commandb = b;

	return (commanda->info.any.seqno < commandb->info.any.seqno ? -1 :
		(commanda->info.any.seqno > commandb->info.any.seqno  ? 1 : 0));
}

static void
queue_command (MPProcess *process, MIInfo *info, StackNode *stack)
{
	Command *command = g_new (Command, 1);
	command->info = *info;
	command->stack = stack;

	process->command_queue = g_list_insert_sorted (process->command_queue, command, queue_compare);
}

static gboolean
unqueue_command (MPProcess *process, MIInfo *info, StackNode **stack)
{
	Command *command = process->command_queue->data;
	GList *tmp_list;
	
	if (command->info.any.seqno == process->seqno) {
		*stack = command->stack;
		*info = command->info;

		tmp_list = process->command_queue;
		process->command_queue = g_list_remove_link (process->command_queue, tmp_list);
		g_free (command);
		g_list_free_1 (tmp_list);

		return TRUE;
	} else
		return FALSE;
}

/************************************************************
 * Block Manipulation
 ************************************************************/

void
block_unref (Block *block)
{
	block->refcount--;
	if (block->refcount == 0)
		g_free (block);
}
 
/************************************************************
 * Code to map addresses to object files
 ************************************************************/
typedef struct {
  guint addr;
  guint size;
  gchar *name;
} Symbol;

void
prepare_map (Map *map)
{
	g_return_if_fail (!map->prepared);

	map->binfile = bin_file_new (map->name);
	
	map->prepared = TRUE;
}

static void
process_read_maps (MPProcess *process)
{
	gchar buffer[1024];
	FILE *in;
  
	snprintf (buffer, 1023, "/proc/%d/maps", process->pid);

	in = fopen (buffer, "r");

	if (process->map_list) {
		g_list_foreach (process->map_list, (GFunc)g_free, NULL);
		g_list_free (process->map_list);
      
		process->map_list = NULL;
	}

	while (fgets(buffer, 1023, in)) {
		char file[256];
		int count;
		gulong start;
		gulong end;
		gulong offset;
		gulong inode;
		
		count = sscanf (
			buffer, "%lx-%lx %*15s %lx %*x:%*x %lu %255s", 
			&start, &end, &offset, &inode, file);
		if (count == 5)
		{
			Map *map;
			
			map = g_new (Map, 1);
			map->prepared = FALSE;
			map->start = start;
			map->size = end - start;
			
			map->offset = offset;
			
			map->name = g_strdup (file);
			
			process->map_list = g_list_prepend (process->map_list, map);
		}
	}

	fclose (in);
}

static Map *
real_locate_map (MPProcess *process, guint addr)
{
	GList *tmp_list = process->map_list;

	while (tmp_list)
	{
		Map *tmp_map = tmp_list->data;
      
		if ((addr >= tmp_map->start) &&
		    (addr < tmp_map->start + tmp_map->size))
			return tmp_map;
      
		tmp_list = tmp_list->next;
	}

	return NULL;
}

Map *
locate_map (MPProcess *process, guint addr)
{
	Map *map = real_locate_map (process, addr);
	if (!map)
	{
		gpointer page_addr = (gpointer) (addr - addr % MP_PAGE_SIZE);
		if (g_list_find (process->bad_pages, page_addr))
			return NULL;

		process_read_maps (process);

		map = real_locate_map (process, addr);

		if (!map) {
			process->bad_pages = g_list_prepend (process->bad_pages, page_addr);
			return NULL;
		}
	}

	if (!map->prepared)
		prepare_map (map);

	return map;
}

const char *
process_locate_symbol (MPProcess *process, gsize addr)
{
	Map *map = locate_map (process, addr);
	const BinSymbol *symbol;

	if (!map)
		return "<unknown map>";

	addr -= map->start;
	addr += map->offset;
	
	symbol = bin_file_lookup_symbol (map->binfile, addr);

	return bin_symbol_get_name (map->binfile, symbol);
}

/**
 * The string assigned to functionname is owned/shared
 * by the system and must not be freed.
 */
gboolean 
process_find_line (MPProcess *process, void *address,
		   const char **filename, char **functionname,
		   unsigned int *line)
{
	const char *s = process_locate_symbol (process, GPOINTER_TO_SIZE (address));

	if (s)
	{
		*filename = NULL;
		*functionname = (char*)s;
		*line = -1;
	}

	return TRUE;
}

void
process_dump_stack (MPProcess *process, FILE *out, StackNode *stack)
{
	for (; stack != NULL; stack = stack->parent)
	{
		const char *filename;
		char *functionname;
		unsigned int line;
      
		if (process_find_line (process, stack->address,
				       &filename, &functionname, &line)) {
			if (filename)
				fprintf(out, "\t%s(): %s:%u\n", functionname, filename, line);
			else
				fprintf(out, "\t%s()\n", functionname);
		} else
			fprintf(out, "\t[%p]\n", stack->address);
	}
}


void 
process_map_sections (Map *map, SectionFunc func, gpointer user_data)
{
	/* FIXME: this should be reinstated */
	
#if 0
	asection *section;

	if (map->abfd)
		for (section = map->abfd->sections; section; section = section->next) {
			if (strcmp (section->name, ".bss") == 0 ||
			    strcmp (section->name, ".data") == 0) {
				void *addr = (void *)section->vma;
				if (map->do_offset)
					addr += map->addr;
	    
				/* bfd_section_size() gives 0 for old versions of binutils, so peek
				 * into the internals instead. :-(
				 */
				(*func) (addr, bfd_section_size (map->abfd, section), user_data);
//				(*func) (addr, section->_cooked_size, user_data);
			}
		}
#endif
}

void 
process_sections (MPProcess *process, SectionFunc func, gpointer user_data)
{
	GList *tmp_list;

	process_read_maps (process);

	tmp_list = process->map_list;

	while (tmp_list) {
		Map *map = tmp_list->data;

		if (!map->prepared)
			prepare_map (map);

		process_map_sections (map, func, user_data);
		tmp_list = tmp_list->next;
	}
}

/************************************************************
 * Communication with subprocess
 ************************************************************/

static GHashTable *
get_block_table (MPProcess *process)
{
	if (!process->block_table)
		process->block_table = g_hash_table_new (g_direct_hash, NULL);

	return process->block_table;
		
}

static void
process_free_block (gpointer key, gpointer value, gpointer data)
{
	block_unref (value);
}

static void
process_duplicate_block (gpointer key, gpointer value, gpointer data)
{
	GHashTable *new_table = data;
	Block *block = value;

	block->refcount++;

	g_hash_table_insert (new_table, key, value);
}

MPProcess *
process_duplicate (MPProcess *process)
{
	MPProcess *new_process = process_new (process->server);

	if (process->block_table) {
		GHashTable *new_block_table = get_block_table(new_process);

		g_hash_table_foreach (process->block_table,
				      process_duplicate_block,
				      new_block_table);
	}

	new_process->bytes_used = process->bytes_used;
	new_process->n_allocations = process->n_allocations;
	new_process->seqno = process->seqno;

	new_process->parent = process;

	return new_process;
}

static void
process_reinit (MPProcess *process)
{
	process_stop_input (process);

	if (process->input_channel) {
		g_io_channel_unref (process->input_channel);
		process->input_channel = NULL;
	}

	process->bytes_used = 0;
	process->n_allocations = 0;
	process->seqno = 0;

	if (process->map_list) {
		g_list_foreach (process->map_list, (GFunc)g_free, NULL);
		g_list_free (process->map_list);
      
		process->map_list = NULL;
	}

	if (process->bad_pages) {
		g_list_free (process->bad_pages);
		process->bad_pages = NULL;
	}

	if (process->block_table) {
		g_hash_table_foreach (process->block_table, process_free_block, NULL);
		g_hash_table_destroy (process->block_table);
		process->block_table = NULL;
	}

	if (process->stack_stash) {
		stack_stash_unref (process->stack_stash);
		process->stack_stash = NULL;
	}

	/* FIXME: leak */
	process->command_queue = NULL;
}

static StackStash *
get_stack_stash (MPProcess *process)
{
	if (!process->stack_stash)
		process->stack_stash = stack_stash_new (NULL);

	return process->stack_stash;
}

void
process_exec_reset (MPProcess *process)
{
	process_reinit (process);
	g_signal_emit_by_name (process, "reset");
}

static void
process_command (MPProcess *process, MIInfo *info, StackNode *stack)
{
	GHashTable *block_table;
	Block *block;

	if (info->any.seqno != process->seqno) {
		queue_command (process, info, stack);
		return;
	}

	process->seqno++;

	block = NULL;
	
	switch (info->operation) {
	case MI_NEW:
	case MI_FORK:
	case MI_CLONE:
	case MI_EXEC:
		g_assert_not_reached ();
		break;
		
	case MI_EXIT:
		/* Handled before, ignore */
		break;

	case MI_TIME:
		info->alloc.old_ptr = NULL;
		info->alloc.new_ptr = (void *)process->seqno;
		info->alloc.size = 1;
		/* Fall through */
		
	default: /* MALLOC / REALLOC / FREE */
		block_table = get_block_table (process);
		
		if (info->alloc.old_ptr != NULL) {
			block = g_hash_table_lookup (block_table, info->alloc.old_ptr);
			if (!block) {
				g_warning ("Block %p not found (pid=%d)!\n", info->alloc.old_ptr, process->pid);
				process_dump_stack (process, stderr, stack);
			}
			else {
				g_hash_table_remove (block_table, info->alloc.old_ptr);
				process->bytes_used -= block->size;
				block_unref (block);
				
				process->n_allocations--;
			}
		}

		/* We need to lookup before inserting, because realloc() can call malloc(), so we
		 * see the same block twice. The same problem comes upduring malloc initialization
		 * where __libc_malloc() is called twice for the same block. We could optimize
		 * things a bit by using g_hash_table_new_full() to catch the replacement when
		 * it happens and free the old block, but that would make keeping track of
		 * process->n_allocations/bytes_used a little difficult.
		 */

		if (info->alloc.new_ptr && !g_hash_table_lookup (block_table, info->alloc.new_ptr)) {
			block = g_new (Block, 1);
			block->refcount = 1;
			
			block->flags = 0;
			block->addr = info->alloc.new_ptr;
			block->size = info->alloc.size;
			block->stack = stack;
			
			process->n_allocations++;
			process->bytes_used += info->alloc.size;

			g_hash_table_insert (block_table, info->alloc.new_ptr, block);
		}
	}

	while (process->command_queue && unqueue_command (process, info, &stack))
		process_command (process, info, stack);
}

static gboolean 
input_func (GIOChannel  *source,
	    GIOCondition condition,
	    gpointer     data)
{
	MIInfo info;
	guint count;
	MPProcess *input_process = data;
	MPProcess *process = NULL;
  
	do {
		memset (&info, 0, sizeof (info));
		info.operation = -1;
		count = 0;
		if (g_io_channel_get_flags(source) & G_IO_FLAG_IS_READABLE)
			g_io_channel_read_chars (source, (char *)&info, sizeof(info), &count, NULL);

		if (count == 0) {
			g_io_channel_unref (input_process->input_channel);
			input_process->input_channel = NULL;

			mp_server_remove_process (input_process->server, input_process);
			process_set_status (input_process, MP_PROCESS_DEFUNCT);
		
			return FALSE;
		} else {
			StackNode *stack = NULL;

//			fprintf (stderr, "Read size %d op 0x%x\n", count, info.operation);

			if (info.operation == MI_MALLOC ||
			    info.operation == MI_REALLOC ||
			    info.operation == MI_FREE ||
			    info.operation == MI_TIME) {
				void **stack_buffer = NULL;
				StackStash *stash = get_stack_stash (input_process);

				g_assert (count >= sizeof (MIInfoAlloc));

				stack_buffer = g_alloca (sizeof (void *) * info.alloc.stack_size);
				g_io_channel_read_chars (source, (char *)stack_buffer, sizeof(void *) * info.alloc.stack_size, &count, NULL);
				stack = stack_stash_add_trace (stash, stack_buffer, info.alloc.stack_size, -1);

			} else if (info.operation == MI_EXIT) {
				process_set_status (input_process, MP_PROCESS_EXITING);
				if (input_process->clone_of)
					process_detach (input_process);
			}
		
			process = input_process;
			while (process->clone_of)
				process = process->clone_of;
		
			process_command (process, &info, stack);


/*		if (info.any.pid != input_process->pid)
		g_warning ("Ow! Ow! Ow: %d %d %d!", info.any.pid, input_process->pid, g_io_channel_unix_get_fd (input_process->input_channel)); */

		}
	} while (info.operation != MI_EXIT && (g_io_channel_get_buffer_condition(source) & G_IO_IN));
  
	return TRUE;
}

void
process_stop_input (MPProcess *process)
{
	g_return_if_fail (process != NULL);
  
	if (process->input_tag) {
		g_source_remove (process->input_tag);
		process->input_tag = 0;
	}
}

void
process_start_input (MPProcess *process)
{
	g_return_if_fail (process != NULL);
  
	if (!process->input_tag && process->input_channel)
		process->input_tag = g_io_add_watch_full (process->input_channel,
							  G_PRIORITY_LOW,
							  G_IO_IN | G_IO_PRI | G_IO_ERR | G_IO_HUP | G_IO_NVAL,
							  input_func, process, NULL);
}

void
process_clear_input (MPProcess *process)
{
	g_return_if_fail (process != NULL);
  
	process->bytes_used = 0;
	process->n_allocations = 0;

	if (process->map_list) {
		g_list_foreach (process->map_list, (GFunc)g_free, NULL);
		g_list_free (process->map_list);
      
		process->map_list = NULL;
	}

	if (process->bad_pages) {
		g_list_free (process->bad_pages);
		process->bad_pages = NULL;
	}

	if (process->block_table) {
		g_hash_table_foreach (process->block_table, process_free_block, NULL);
		g_hash_table_destroy (process->block_table);
		process->block_table = NULL;
	}

	if (process->stack_stash) {
		stack_stash_unref (process->stack_stash);
		process->stack_stash = NULL;
	}
}

char *
process_find_exec (char **args)
{
	int i;
  
	if (g_file_test(args[0], G_FILE_TEST_EXISTS)) {
		if (!g_path_is_absolute (args[0]))
			return g_strconcat ("./", args[0], NULL);
		else
			return g_strdup (args[0]);
	} else {
		char **paths;
		char *path = NULL;
		char *pathenv = getenv ("PATH");
		if (pathenv)
		{
			paths = g_strsplit (pathenv, ":", -1);
			for (i=0; paths[i]; i++) {
				path = g_build_filename (paths[i], args[0], NULL);
				if (g_file_test (path, G_FILE_TEST_EXISTS))
					break;
				else {
					g_free (path);
					path = NULL;
				}
			}

			g_strfreev (paths);
		}

		return path;
	}
}

char **
process_parse_exec (const char *exec_string)
{
	return g_strsplit (exec_string, " ", -1);
}

GType
mp_process_get_type (void)
{
	static GType process_type = 0;

	if (!process_type) {
		static const GTypeInfo process_info = {
			sizeof (MPProcessClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) mp_process_class_init,
			NULL, /* class_finalize */
			NULL, /* class_data */
			sizeof (MPProcess),
			0, /* n_preallocs */
			(GInstanceInitFunc) mp_process_init
		};

		process_type = g_type_register_static (G_TYPE_OBJECT,
						       "MPProcess",
						       &process_info, 0);
	}

	return process_type;
}

static void
mp_process_class_init (MPProcessClass *class)
{
	static gboolean initialized = FALSE;

	GObjectClass *o_class = G_OBJECT_CLASS (class);

	o_class->finalize = mp_process_finalize;

	if (!initialized) {
		process_signals[STATUS_CHANGED] =
			g_signal_new ("status_changed",
			              MP_TYPE_PROCESS,
			              G_SIGNAL_RUN_LAST,
			              G_STRUCT_OFFSET (MPProcessClass, status_changed),
			              NULL, NULL,
			              g_cclosure_marshal_VOID__VOID,
			              G_TYPE_NONE, 0);
		
		process_signals[RESET] =
			g_signal_new ("reset",
			              MP_TYPE_PROCESS,
			              G_SIGNAL_RUN_LAST,
			              G_STRUCT_OFFSET (MPProcessClass, reset),
				      NULL, NULL,
			              g_cclosure_marshal_VOID__VOID,
			              G_TYPE_NONE, 0);

		initialized = TRUE;
	}
}

static void
mp_process_init (MPProcess *process)
{
	process->status = MP_PROCESS_INIT;
	process->pid = 0;
	process->clone_of = NULL;
  
	process->bytes_used = 0;
	process->n_allocations = 0;
	process->block_table = NULL;
	process->stack_stash = NULL;

	process->program_name = NULL;
	process->input_channel = NULL;

	process->seqno = 0;

	process->command_queue = NULL;

	process->follow_fork = FALSE;
	process->follow_exec = FALSE;

	g_object_ref (G_OBJECT (process));
}

static void 
mp_process_finalize (GObject *object)
{
	MPProcess *process = MP_PROCESS (object);

	process_reinit (process);
	
	g_free (process->program_name);
}


MPProcess *
process_new (MPServer *server)
{
	MPProcess *process;

	process = g_object_new (MP_TYPE_PROCESS, NULL);

	process->server = server;

	return process;
}

void
process_set_follow_fork (MPProcess *process,
			 gboolean   follow_fork)
{
	process->follow_fork = follow_fork;
}

void
process_set_follow_exec (MPProcess *process,
			 gboolean   follow_exec)
{
	process->follow_exec = follow_exec;
}

void
process_run (MPProcess *process, const char *path, char **args)
{
	process->program_name = g_strdup (path);
	read_inode (path);

	process->pid = mp_server_instrument (process->server, path, args);
	mp_server_add_process (process->server, process);

	process_set_status (process, MP_PROCESS_STARTING);
  
	process_start_input (process);
}

GList *
process_get_clones (MPProcess *process)
{
	return mp_server_get_process_clones (process->server, process);
}

void
process_set_status (MPProcess *process, MPProcessStatus status)
{
	if (process->status != status) {
		process->status = status;
		g_signal_emit_by_name (process, "status_changed", NULL);
	}
}

char *
process_get_status_text (MPProcess *process)
{
	char *status = "";
	
	switch (process->status) {
	case MP_PROCESS_INIT:
		status = _("Initial");
		break;
	case MP_PROCESS_STARTING:
		status = _("Starting");
		break;
	case MP_PROCESS_RUNNING:
		status = _("Running");
		break;
	case MP_PROCESS_EXITING:
		status = _("Exiting");
		break;
	case MP_PROCESS_DEFUNCT:
		status = _("Defunct");
		break;
	case MP_PROCESS_DETACHED:
		status = _("Defunct");
		break;
	}

	return g_strdup (status);
}

char *
process_get_cmdline (MPProcess *process)
{
	char *fname;
	char *result;
	char *tmp = NULL;
	size_t n = 0;
	FILE *in = NULL;

	if (process->status == MP_PROCESS_DEFUNCT)
		return g_strdup ("");
	
	fname = g_strdup_printf ("/proc/%d/cmdline", process->pid);
	in = fopen (fname, "r");
	if (!in) {
		g_warning ("Can't open %s\n", fname);
		return g_strdup ("");
	}
	g_free (fname);

	if (getline (&tmp, &n, in) == -1)
		result = g_strdup ("");
	else {
		result = g_strdup (tmp);
		free (tmp);
	}

	fclose (in);

	return result;
}

void
process_detach (MPProcess *process)
{
	int ret;

	if (process->status != MP_PROCESS_DEFUNCT) {
		int fd = g_io_channel_unix_get_fd (process->input_channel);

		if (process->status == MP_PROCESS_EXITING) {
			char response = 0;
			ret = write (fd, &response, 1);
		} else {
			g_io_channel_shutdown (process->input_channel, TRUE, NULL);
			process_set_status (process, MP_PROCESS_DETACHED);
		}
	}
}

void
process_kill (MPProcess *process)
{
	if (process->status == MP_PROCESS_EXITING) {
		process_detach (process);
	} else if (process->status != MP_PROCESS_DEFUNCT &&
		   process->status != MP_PROCESS_INIT) {
		kill (process->pid, SIGTERM);
	}
}

typedef struct {
	MPProcessBlockForeachFunc foreach_func;
	gpointer data;
} BlockForeachInfo;

static void
block_table_foreach_func (gpointer key,
			  gpointer value,
			  gpointer data)
{
	BlockForeachInfo *info = data;

	info->foreach_func (value, info->data);
}

void
process_block_foreach (MPProcess                 *process,
		       MPProcessBlockForeachFunc  foreach_func,
		       gpointer                   data)
{
	GHashTable *block_table = get_block_table (process);
	BlockForeachInfo info;

	info.foreach_func = foreach_func;
	info.data = data;

	g_hash_table_foreach (block_table, block_table_foreach_func, &info);
}

gboolean
symbol_equal (gconstpointer s1, gconstpointer s2)
{
	return s1 == s2;
}

guint
symbol_hash  (gconstpointer s)
{
	const char *symbol = s;

	return g_str_hash (symbol);
}

char *
symbol_copy  (const char *orig)
{
	if (!orig)
		return NULL;

	return (char *) orig;
}

void
symbol_free  (char *symbol)
{
	if (!symbol)
		return;
}

