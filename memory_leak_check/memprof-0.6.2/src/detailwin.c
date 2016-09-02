/* -*- mode: C; c-file-style: "linux" -*- */

/* MemProf -- memory profiler and leak detector
 * Copyright 2006 Carsten Haitzler
 * Copyright 2009 Holger Hans Peter Freyther
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

#include "config.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <regex.h>
#include <signal.h>
#include <sys/wait.h>


#include <glade/glade.h>
#include <gtk/gtk.h>

#include "gui.h"
#include "memprof.h"

static void
dw_draw_memstats(ProcessWindow *pwin)
{
   GtkWidget *widget;
   GdkPixmap *pixmap;
   gint64 i, j, x, y, w, h, hh;

   widget = pwin->time_graph;
   w = widget->allocation.width;
   h = widget->allocation.height;
   if (!widget->window) return;
   pixmap = gdk_pixmap_new(widget->window, w, h, -1);
   gdk_draw_rectangle(pixmap,
		      widget->style->base_gc[GTK_STATE_NORMAL],
		      TRUE,
		      0, 0, w, h);
   for (i = 0; i < MEMSTATS; i++)
     {
	x = w - i;
	if (x < 0) break;
	if (pwin->usage_high > 0)
	  hh = (h * pwin->memstats[i][1]) / pwin->usage_high;
	else
	  hh = 0;
	y = h - hh;
	gdk_draw_rectangle(pixmap,
			   widget->style->base_gc[GTK_STATE_SELECTED],
			   TRUE,
			   x, y, 1, hh);
	if (pwin->usage_high > 0)
	  hh = (h * pwin->memstats[i][0]) / pwin->usage_high;
	else
	  hh = 0;
	y = h - hh;
	gdk_draw_rectangle(pixmap,
			   widget->style->text_gc[GTK_STATE_NORMAL],
			   TRUE,
			   x, y, 1, hh);
     }
   if (pwin->usage_high > 0)
     {
	GdkGC *gc;

	gc = gdk_gc_new(pixmap);
	gdk_gc_copy(gc, widget->style->dark_gc[GTK_STATE_NORMAL]);
	gdk_gc_set_line_attributes(gc, 0, GDK_LINE_ON_OFF_DASH,
				   GDK_CAP_BUTT, GDK_JOIN_MITER);
	for (j = 0, i = 0; i < pwin->usage_high; i += (256 * 1024), j++)
	  {
	     if (j > 3) j = 0;
	     y = h - ((i * h) / pwin->usage_high);
	     if (j == 0)
	       gdk_draw_line(pixmap, widget->style->dark_gc[GTK_STATE_NORMAL],
			     0, y, w, y);
	     else
	       gdk_draw_line(pixmap, gc,
			     0, y, w, y);
	  }
	gdk_gc_unref(gc);
     }
   gdk_draw_pixmap(widget->window,
		   widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
		   pixmap, 0, 0, 0, 0, w, h);
   gdk_pixmap_unref(pixmap);
}

typedef struct _Mem
{
   GtkWidget *widget;
   GdkPixmap *pixmap;
   guint w, h, bpp;
   gulong mtotal;
   GList *regs;
} Mem;

typedef struct _MemReg
{
   void *start;
   unsigned long size;
   int y, h;
} MemReg;

static void
dw_draw_memmap_foreach(gpointer key, gpointer value, gpointer data)
{
   Mem *mem;
   Block *block;
   guint x, y, w, i;
   GdkPixmap *pixmap;
   GdkGC *gc;
   gulong addr;
   guint bpp, memw;
   GList *l;
   MemReg *reg;


   mem = data;
   block = value;
   bpp = mem->bpp;
   addr = (gulong)block->addr;
   for (i = 0, l = mem->regs; l; l = l->next, i++)
     {
	reg = l->data;
	if ((addr >= (gulong)reg->start) &&
	    (addr < (gulong)(reg->start + reg->size)))
	  break;
	reg = NULL;
     }
   if (!reg) return;
   addr = addr - (gulong)reg->start;
   w = (gulong)(addr + block->size + (bpp / 2)) / bpp;
   x = (gulong)(addr) / bpp;
   w -= x;
   memw = mem->w;
   y = reg->y + (x / memw);
   x -= (y * memw);
   pixmap = mem->pixmap;
   if (i & 0x1)
     gc = mem->widget->style->fg_gc[GTK_STATE_PRELIGHT];
   else
     gc = mem->widget->style->fg_gc[GTK_STATE_NORMAL];
   gdk_draw_rectangle(pixmap, gc, TRUE, x, y, w, 1);
   if ((x + w) > mem->w)
     gdk_draw_rectangle(pixmap, gc, TRUE, 0, y + 1, w - (memw - x), 1);
}

static void
dw_draw_memmap(ProcessWindow *pwin)
{
   GtkWidget *widget;
   GdkPixmap *pixmap;
   guint w, h, y, bpl, i;
   Mem mem;
   GList *l;

   widget = pwin->mem_map;
   w = widget->allocation.width;
   h = widget->allocation.height;
   if (!widget->window) return;
   pixmap = gdk_pixmap_new(widget->window, w, h, -1);
   gdk_draw_rectangle(pixmap,
		      widget->style->base_gc[GTK_STATE_NORMAL],
		      TRUE,
		      0, 0, w, h);
   mem.regs = NULL;
   mem.widget = widget;
   mem.pixmap = pixmap;
   mem.w = w;
   mem.h = h;
   mem.mtotal = 0;
     {
        gchar buffer[1024];
	FILE *in;
	gchar perms[26];
	gchar file[256];
	gulong start, end;
	guint major, minor, inode;

	snprintf(buffer, 1023, "/proc/%d/maps", pwin->process->pid);

	in = fopen(buffer, "r");
	if (!in)
	  {
	     g_warning("Failed to open: '%s'", buffer);
	     return;
	  }

        while (fgets(buffer, 1023, in))
	  {
	     file[0] = 0;
	     int count = sscanf(buffer, "%lx-%lx %15s %*x %u:%u %u %255s",
				&start, &end, perms, &major, &minor, &inode,
				file);
	     if (count >= 5)
	       {
		  if ((!strcmp(perms, "rw-p")) && (inode == 0))
		    {
		       MemReg *reg;

		       reg = g_malloc(sizeof(MemReg));
		       reg->start = GSIZE_TO_POINTER(start);
		       reg->size = end - start;
		       mem.mtotal += reg->size;
		       mem.regs = g_list_append(mem.regs, reg);
		    }
	       }
	  }
        fclose (in);
     }
   bpl = (mem.mtotal + h - 1) / h;
   y = 0;
   for (i = 0, l = mem.regs; l; l = l->next, i++)
     {
	MemReg *reg;

	reg = l->data;
	reg->h = ((reg->size * h) + bpl - 1) / mem.mtotal;
	reg->y = y;
	y += reg->h;
	if (i & 0x1)
	  gdk_draw_rectangle(pixmap,
			     widget->style->base_gc[GTK_STATE_INSENSITIVE],
			     TRUE,
			     0, reg->y, w, reg->h);
     }
   mem.bpp = (bpl + w - 1) / w;
   if (mem.bpp < 1) mem.bpp = 1;

   g_hash_table_foreach(pwin->process->block_table,
			dw_draw_memmap_foreach,
			&mem);

   for (l = mem.regs; l; l = l->next) g_free(l->data);
   g_list_free(mem.regs);

   gdk_draw_pixmap(widget->window,
		   widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
		   pixmap, 0, 0, 0, 0, w, h);
   gdk_pixmap_unref(pixmap);
}

gboolean
time_graph_expose_event(GtkWidget *widget, GdkEventExpose  *event, ProcessWindow *pwin)
{
   if (!pwin->process) return FALSE;
   dw_draw_memstats(pwin);
   return FALSE;
}

gboolean
mem_map_expose_event(GtkWidget *widget, GdkEventExpose  *event, ProcessWindow *pwin)
{
   if (!pwin->process) return FALSE;
   dw_draw_memmap(pwin);
   return FALSE;
}

void
dw_update(ProcessWindow *pwin)
{
   guint i;
   if (!pwin->process) return;
   for (i = MEMSTATS - 1; i > 0; i--)
     {
	pwin->memstats[i][0] = pwin->memstats[i - 1][0];
	pwin->memstats[i][1] = pwin->memstats[i - 1][1];
     }
   pwin->memstats[0][0] = pwin->process->bytes_used;
   pwin->memstats[0][1] = pwin->usage_high;
   dw_draw_memstats(pwin);
   dw_draw_memmap(pwin);
}
