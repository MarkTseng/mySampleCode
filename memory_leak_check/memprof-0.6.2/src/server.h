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

#include <glib-object.h>
#include "process.h"

#define MP_TYPE_SERVER            (mp_server_get_type ())
#define MP_SERVER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MP_TYPE_SERVER, MPServer))
#define MP_SERVER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MP_TYPE_SERVER, MPServerClass))
#define MP_IS_SERVER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MP_TYPE_SERVER))
#define MP_IS_SERVER_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE ((obj), MP_TYPE_SERVER))
#define MP_SERVER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MP_TYPE_SERVER, MPServerClass))

typedef enum {
	MP_PROFILE_MEMORY,
	MP_PROFILE_CYCLES,
	MP_PROFILE_TIME
} MPProfileType;

typedef struct _MPServerClass MPServerClass;

GType   mp_server_get_type     (void);
MPServer *mp_server_new        (void);

void mp_server_set_profile_type (MPServer       *server,
				 MPProfileType   type);
void mp_server_set_interval     (MPServer       *server,
				 guint           usecs);
int  mp_server_instrument       (MPServer       *server,
				 const char     *program,
				 char          **args);

MPProcess *mp_server_find_process       (MPServer  *server,
					 pid_t      pid);
void       mp_server_add_process        (MPServer  *server,
					 MPProcess *process);
void       mp_server_remove_process     (MPServer  *server,
					 MPProcess *process);
GList *    mp_server_get_process_clones (MPServer  *server,
					 MPProcess *process);

