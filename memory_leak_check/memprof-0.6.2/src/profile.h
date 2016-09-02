/* MemProf -- memory profiler and leak detector
 * Copyright 2002, Soeren Sandmann (sandmann@daimi.au.dk)
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

#include "memprof.h"
#include "process.h"
#include <glib.h>

typedef struct ProfileNode			ProfileNode;
typedef struct Profile				Profile;
typedef struct ProfileFunc			ProfileFunc;
typedef struct ProfileDescendantTree		ProfileDescendantTree;
typedef struct ProfileDescendantTreeNode	ProfileDescendantTreeNode;

struct ProfileNode {
    char *		symbol;

    guint		total;
    guint		self;
    gboolean		toplevel;	/* whether the node is the first for this function
					 * in the stack trace
					 */

    ProfileNode *	next;		/* chains together nodes with matching symbols */

    GPtrArray *		children;
    ProfileNode *	parent;
};

struct ProfileDescendantTreeNode
{
    char *			symbol;

    guint			self;
    guint			non_recursion;
    guint			total;
    
    GPtrArray *			children;
    ProfileDescendantTreeNode *parent;
    
    gint			marked_non_recursive;
    gboolean			marked_total;
};

struct ProfileDescendantTree {
    GPtrArray *	roots;
};

struct ProfileFunc {
    guint		total;		/* sum of all toplevel totals */
    guint		self;		/* sum of all selfs */

    ProfileNode       *node;
};

struct Profile {
    GPtrArray *		roots;		/* root nodes of call tree */
    GPtrArray *		functions;
    guint		n_bytes;	/* total number of bytes profiled */
    
    /* private */
    MPProcess *	process;
    GHashTable *nodes_by_symbol;
};

Profile *              profile_create                      (MPProcess             *process,
							    GSList                *skip_funcs);
void                   profile_free                        (Profile               *profile);
ProfileDescendantTree *profile_func_create_descendant_tree (ProfileFunc           *func);
void                   profile_descendant_tree_free        (ProfileDescendantTree *descendant_tree);
GPtrArray *            profile_func_create_caller_list     (ProfileFunc           *func);
void                   profile_caller_list_free            (GPtrArray             *caller_list);
void                   profile_write                       (Profile               *profile, const gchar *outfile);
