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

#include <glade/glade.h>
#include <glib/gi18n.h>

#include "gui.h"
#include "treeviewutils.h"

static ProcessWindow *iter_get_process_window (GtkTreeIter *iter);

enum {
	PID_COLUMN,
	CMDLINE_COLUMN,
	STATUS_COLUMN,
	PWIN_COLUMN
};

static GtkWidget *tree_window;
static GtkWidget *tree_view;
static GtkTreeStore *store;

extern char *glade_file;

static GtkWidget *
make_menu_item (const char *label, GCallback cb)
{
	GtkWidget *menu_item = gtk_menu_item_new_with_label (label);
	g_signal_connect (menu_item, "activate", cb, NULL);
	gtk_widget_show (menu_item);
	
	return menu_item;
}

static ProcessWindow *
get_process_window (GtkWidget *menu_item)
{
	return g_object_get_data (G_OBJECT (menu_item->parent), "process-window");
}

static void
show_cb (GtkWidget *widget)
{
	ProcessWindow *pwin = get_process_window (widget);

	process_window_show (pwin);
}

static void
hide_cb (GtkWidget *widget)
{
	ProcessWindow *pwin = get_process_window (widget);

	process_window_hide (pwin);
}

static void
tree_detach_cb (GtkWidget *widget)
{
	ProcessWindow *pwin = get_process_window (widget);

	process_window_maybe_detach (pwin);
}

static void
tree_kill_cb (GtkWidget *widget)
{
	ProcessWindow *pwin = get_process_window (widget);

	process_window_maybe_kill (pwin);
}

static void
popup_menu (ProcessWindow *pwin, gint button, guint32 time)
{
	static GtkWidget *menu = NULL;
	static GtkWidget *show_item = NULL;
	static GtkWidget *hide_item = NULL;

	if (!menu) {
		menu = gtk_menu_new ();

		show_item = make_menu_item (_("Show"), G_CALLBACK (show_cb));
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), show_item);

		hide_item = make_menu_item (_("Hide"), G_CALLBACK (hide_cb));
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), hide_item);
		
		gtk_menu_shell_append (GTK_MENU_SHELL (menu),
				       make_menu_item (_("Kill"), G_CALLBACK (tree_kill_cb)));

		gtk_menu_shell_append (GTK_MENU_SHELL (menu),
				       make_menu_item (_("Detach"), G_CALLBACK (tree_detach_cb)));
	}

	if (process_window_visible (pwin)) {
		gtk_widget_set_sensitive (show_item, FALSE);
		gtk_widget_set_sensitive (hide_item, TRUE);
	} else {
		gtk_widget_set_sensitive (show_item, TRUE);
		gtk_widget_set_sensitive (hide_item, FALSE);
	}
	
	g_object_set_data (G_OBJECT (menu), "process-window", pwin);
	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, button, time);
}

static gboolean
button_press_cb (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	GtkTreeView *tree_view = GTK_TREE_VIEW (widget);
	GtkTreeModel *model = GTK_TREE_MODEL (store);
	GtkTreePath *path;

	if (event->button == 3 &&
	    event->window == gtk_tree_view_get_bin_window (tree_view) &&
	    gtk_tree_view_get_path_at_pos (tree_view,
					   event->x, event->y,
					   &path, NULL, NULL, NULL)) {
		GtkTreeIter iter;
		
		gtk_tree_model_get_iter (model, &iter, path);
		popup_menu (iter_get_process_window (&iter), event->button, event->time);
	}

	return FALSE;
}

static void
row_activated_cb (GtkTreeView       *tree_view,
		  GtkTreePath       *path,
		  GtkTreeViewColumn *column)
{
	GtkTreeModel *model = GTK_TREE_MODEL (store);
	GtkTreeIter iter;

	gtk_tree_model_get_iter (model, &iter, path);
	process_window_show (iter_get_process_window (&iter));
}

static void
ensure_tree_window (void)
{
	if (!tree_window) {
		GladeXML *xml = glade_xml_new (glade_file, "TreeWindow", NULL);
		
		tree_window = glade_xml_get_widget (xml, "TreeWindow");
		gtk_window_set_default_size (GTK_WINDOW (tree_window), 400, 300);
		g_signal_connect (tree_window, "delete_event",
				  G_CALLBACK (hide_and_check_quit), NULL);
		
		tree_view = glade_xml_get_widget (xml, "TreeWindow-tree-view");

		store = gtk_tree_store_new (4,
					    G_TYPE_INT,     /* PID_COLUMN */
					    G_TYPE_STRING,  /* CMDLINE_COLUMN */
					    G_TYPE_STRING,  /* STATUS_COLUMN */
					    G_TYPE_POINTER  /* PWIN_COLUMN */);
		gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), GTK_TREE_MODEL (store));

		add_plain_text_column (GTK_TREE_VIEW (tree_view), _("PID"), PID_COLUMN);
		add_plain_text_column (GTK_TREE_VIEW (tree_view), _("Command Line"), CMDLINE_COLUMN);
		add_plain_text_column (GTK_TREE_VIEW (tree_view), _("Status"), STATUS_COLUMN);

		g_signal_connect (tree_view, "button_press_event",
				  G_CALLBACK (button_press_cb), NULL);
		g_signal_connect (tree_view, "row_activated",
				  G_CALLBACK (row_activated_cb), NULL);
	}
}

void
tree_window_show (void)
{
	ensure_tree_window ();
	
	gtk_widget_show (tree_window);
	gdk_window_show (tree_window->window); /* Raise */
}

static ProcessWindow *
iter_get_process_window (GtkTreeIter *iter)
{
	GtkTreeModel *model = GTK_TREE_MODEL (store);
	ProcessWindow *pwin;
	
	gtk_tree_model_get (model, iter, PWIN_COLUMN, &pwin, -1);

	return pwin;
}

static void
iter_set_process_window (GtkTreeIter   *iter,
			 ProcessWindow *pwin)
{
	gtk_tree_store_set (store, iter, PWIN_COLUMN, pwin, -1);
}

static gboolean
find_by_process_recurse (MPProcess   *process,
			 GtkTreeIter *iter,
			 GtkTreeIter *parent)
{
	GtkTreeModel *model = GTK_TREE_MODEL (store);
	GtkTreeIter children;
	
	if (parent) {
		ProcessWindow *pwin = iter_get_process_window (parent);
		MPProcess *row_process = process_window_get_process (pwin);

		if (row_process == process) {
			*iter = *parent;
			return TRUE;
		}
	}

	if (gtk_tree_model_iter_children (model, &children, parent)) {
		do {
			if (find_by_process_recurse (process, iter, &children))
				return TRUE;
		} while (gtk_tree_model_iter_next (model, &children));
	}

	return FALSE;
		
}

static gboolean
find_by_process (MPProcess   *process,
		 GtkTreeIter *iter)
{
	return find_by_process_recurse (process, iter, NULL);
}

static void
update_process (GtkTreeIter *iter)
{
	ProcessWindow *pwin = iter_get_process_window (iter);
	MPProcess *process = process_window_get_process (pwin);
	char *cmdline = process_get_cmdline (process);
	char *status = process_get_status_text (process);

	gtk_tree_store_set (store, iter,
			    PID_COLUMN,     (int)process->pid,
			    CMDLINE_COLUMN, cmdline,
			    STATUS_COLUMN,  status,
			    -1);
	
	g_free (cmdline);
	g_free (status);
}

static void
status_changed_cb (MPProcess *process)
{
	GtkTreeIter iter;
	
	if (find_by_process (process, &iter))
		update_process (&iter);
}

void
tree_window_add (ProcessWindow *window)
{
	MPProcess *process;
	GtkTreeIter *parent = NULL;
	GtkTreeIter tmp_parent;
	GtkTreeIter new;
	
	ensure_tree_window ();

	process = process_window_get_process (window);
	if (process->parent) {
		if (find_by_process (process, &tmp_parent))
			parent = &tmp_parent;
	}

	gtk_tree_store_append (store, &new, parent);
	iter_set_process_window (&new, window);
	update_process (&new);

	g_signal_connect (G_OBJECT (process), "status_changed",
			  G_CALLBACK (status_changed_cb), NULL);
}

static void
copy_subtree (GtkTreeIter  *subtree,
	      GtkTreeIter  *new_parent)
{
	GtkTreeModel *model = GTK_TREE_MODEL (store);
	GtkTreeIter new;
	GtkTreeIter children;

	gtk_tree_store_append (store, &new, new_parent);
	iter_set_process_window (&new, iter_get_process_window (subtree));
	update_process (&new);

	if (gtk_tree_model_iter_children (model, &children, subtree)) {
		do
			copy_subtree (&children, &new);
		while (gtk_tree_model_iter_next (model, &children));
	}
}

static void
copy_children_to_grandparent (GtkTreeIter  *parent)
{
	GtkTreeModel *model = GTK_TREE_MODEL (store);
	GtkTreeIter tmp_parent;
	GtkTreeIter children;
	GtkTreeIter *grandparent;
	
	if (gtk_tree_model_iter_parent (model, &tmp_parent, parent))
		grandparent = &tmp_parent;
	else
		grandparent = NULL;
	
	if (gtk_tree_model_iter_children (model, &children, parent)) {
		do {
			copy_subtree (&children, grandparent);
		} while (gtk_tree_model_iter_next (model, &children));
	}
}

void
tree_window_remove (ProcessWindow *window)
{
	GtkTreeIter iter;
	MPProcess *process;
	
	ensure_tree_window ();

	process = process_window_get_process (window);
	find_by_process (process, &iter);

	copy_children_to_grandparent (&iter);
	gtk_tree_store_remove (store, &iter);

	g_signal_handlers_disconnect_by_func (G_OBJECT (process), G_CALLBACK (status_changed_cb), NULL);
}
