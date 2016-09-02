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

#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <stdio.h>
#include <glib.h>
#include <unistd.h>

#include <glib-object.h>

#include "memprof.h"

#define MP_TYPE_PROCESS            (mp_process_get_type ())
#define MP_PROCESS(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MP_TYPE_PROCESS, MPProcess))
#define MP_PROCESS_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MP_TYPE_PROCESS, MPProcessClass))
#define MP_IS_PROCESS(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MP_TYPE_PROCESS))
#define MP_IS_PROCESS_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE ((obj), MP_TYPE_PROCESS))
#define MP_PROCESS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MP_TYPE_PROCESS, MPProcessClass))

/* forward declaration */
typedef struct _MPServer MPServer;

typedef struct _MPProcess MPProcess;
typedef struct _MPProcessClass MPProcessClass;

typedef enum {
	MP_PROCESS_INIT,	/* Newly created */
	MP_PROCESS_STARTING,	/* Run child, waiting for it to connect */
	MP_PROCESS_RUNNING,	/* Child is running */
	MP_PROCESS_EXITING,	/* _exit() has been called in child */
	MP_PROCESS_DEFUNCT,	/* child no longer exists */
	MP_PROCESS_DETACHED	/* we now ignore this child */
} MPProcessStatus;

typedef void (*MPProcessBlockForeachFunc) (Block    *block,
					   gpointer  data);

struct _MPProcess
{
	GObject parent_instance;

	MPProcessStatus status;
	
	pid_t pid;
	gchar *program_name;

	MPServer *server;
	
	MPProcess *clone_of;
	MPProcess *parent;
	guint seqno;	
	
	guint bytes_used;
	guint n_allocations;
	
	gint input_tag;
	GIOChannel *input_channel;
	
	GList *map_list;
	GList *bad_pages;
	GHashTable *block_table;

	StackStash *stack_stash;
	
	GList *command_queue;
	
	gboolean follow_fork;
	gboolean follow_exec;
};

struct _MPProcessClass {
	GObjectClass parent_class;

	void (*status_changed) (MPProcess *process);
	void (*reset)          (MPProcess *process);
};

GType       mp_process_get_type     (void);
MPProcess * process_new             (MPServer           *server);
MPProcess * process_duplicate       (MPProcess          *process);
void        process_set_follow_fork (MPProcess          *process,
				     gboolean            follow_fork);
void        process_set_follow_exec (MPProcess          *process,
				     gboolean            follow_exec);

void  process_run             (MPProcess        *process,
			       const char       *path,
			       char            **args);
void  process_detach          (MPProcess        *process);
void  process_kill            (MPProcess        *process);
void  process_exec_reset      (MPProcess        *process);
void  process_set_status      (MPProcess        *process,
			       MPProcessStatus   status);
char *process_get_status_text (MPProcess        *process);
char *process_get_cmdline     (MPProcess        *process);

void        process_sections        (MPProcess          *process,
				     SectionFunc         func,
				     gpointer            user_data);
GList *     process_get_clones      (MPProcess          *process);
void        process_start_input     (MPProcess          *process);
void        process_stop_input      (MPProcess          *process);
void	    process_clear_input	    (MPProcess		*process);
void        process_dump_stack      (MPProcess          *process,
				     FILE               *out,
				     StackNode          *stack);
const char *  process_locate_symbol (MPProcess          *process,
				     gsize               addr);

char **     process_parse_exec      (const char         *exec_string);
char *      process_find_exec       (char              **args);

void  process_block_foreach (MPProcess                *process,
			     MPProcessBlockForeachFunc foreach_func,
			     gpointer                  data);
gboolean    process_find_line (MPProcess *process, void *address,
			       const char **filename, char **functionname,
			       unsigned int *line);

#endif /* __PROCESS_H__ */
