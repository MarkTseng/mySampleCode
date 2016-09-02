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

#include <glib.h>
#include <gtk/gtkwidget.h>
#include <stdio.h>
#include <sys/types.h>
#include "bfd.h"
#include "stackstash.h"
#include "binfile.h"

#ifndef __MEMPROF_H__
#define __MEMPROF_H__

typedef enum {
  BLOCK_MARKED  = 1 << 0,
  BLOCK_IS_ROOT = 1 << 1
} BlockFlags;

typedef struct {
  guint flags;
  void *addr;
  guint size;
  StackNode *stack;

  gint refcount;
} Block;

typedef struct {
	/* Initial members of this struct must be identical to that of Symbol */
	guint start;
	guint size;
	gchar *name;
	gulong offset;
	gboolean prepared;
	
	BinFile *binfile;
} Map;

gboolean symbol_equal (gconstpointer s1, gconstpointer s2);
guint    symbol_hash  (gconstpointer symbol);
char *   symbol_copy  (const char *symbol);
void     symbol_free  (char *symbol);

typedef void (*SectionFunc) (void *addr, guint size, gpointer user_data);

typedef enum {
  ERROR_WARNING,
  ERROR_MODAL,
  ERROR_FATAL
} ErrorType;

void show_error	(GtkWidget *parent_window,
		 ErrorType error,
		 const gchar *format,
		 ...) G_GNUC_PRINTF (3, 4);

void block_unref (Block *block);

void     process_map_sections (Map           *map,
			       SectionFunc    func,
			       gpointer       user_data);
char *   demangle             (Map           *map,
			       const char    *name);
gboolean read_bfd             (Map           *map);
gboolean find_line            (Map           *map,
			       bfd_vma        addr,
			       const char   **filename,
			       char         **functionname,
			       unsigned int  *line);


/* Convert a device / inode pair into a file. Not needed for kernel-2.2 or greater.
 */
gchar *locate_inode (dev_t        device,
		     ino_t        inode);
void   read_inode   (const gchar *path);

#endif /* __MEMPROF_H__ */

