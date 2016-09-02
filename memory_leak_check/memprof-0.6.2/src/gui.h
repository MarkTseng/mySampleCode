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

#ifndef GUI_H

#include "leakdetect.h"
#include "profile.h"
#include "process.h"
#include "server.h"

#define MEMSTATS 4096

typedef struct _ProcessWindow ProcessWindow;

struct _ProcessWindow {
	MPProcess *process;
	Profile *profile;
	GSList *leaks;

	GtkWidget *main_window;
	GtkWidget *main_notebook;
	GtkWidget *n_allocations_label;
	GtkWidget *profile_status_label;
	GtkWidget *bytes_per_label;
	GtkWidget *total_bytes_label;

	GtkWidget *profile_func_tree_view;
	GtkWidget *profile_caller_tree_view;
	GtkWidget *profile_descendants_tree_view;

	GtkWidget *leak_block_tree_view;
	GtkWidget *leak_stack_tree_view;

	GtkWidget *usage_max_label;
	GtkWidget *usage_area;

	guint usage_max;
	guint usage_high;
	guint usage_leaked;

	guint status_update_timeout;

	GtkWidget *time_graph;
	GtkWidget *mem_map;
	guint memstats[MEMSTATS][2];
};

void tree_window_show   (void);
void tree_window_add    (ProcessWindow *window);
void tree_window_remove (ProcessWindow *window);

MPProcess * process_window_get_process  (ProcessWindow *pwin);
gboolean    process_window_visible      (ProcessWindow *pwin);
MPProcess * process_window_get_process  (ProcessWindow *pwin);
void        process_window_show         (ProcessWindow *pwin);
void        process_window_hide         (ProcessWindow *pwin);

void        process_window_maybe_kill   (ProcessWindow *pwin);
void        process_window_maybe_detach (ProcessWindow *pwin);

gboolean hide_and_check_quit (GtkWidget *window);
void     check_quit          (void);

gboolean time_graph_expose_event(GtkWidget *widget, GdkEventExpose *event, ProcessWindow *pwin);
gboolean mem_map_expose_event(GtkWidget *widget, GdkEventExpose *event, ProcessWindow *pwin);
void dw_update(ProcessWindow *pwin);

#endif
