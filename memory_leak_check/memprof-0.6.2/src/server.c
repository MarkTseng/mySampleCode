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

#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include <glib-object.h>
#include <glib/gi18n.h>

#include "memintercept.h"
#include "server.h"

/* If USE_SOCKET_DIRECTORY is defined, then the temporary sockets will
 * be created as /tmp/memprof.UID/server.PID. Otherwise, they will
 * be created as /tmp/memprof.XXXXXX. Despite calling mktemp(), the
 * latter should be completely safe, because unix domain socket creation
 * will fail with EADDRINUSE if the file already exists.
 */

#undef USE_SOCKET_DIRECTORY

#define SOCKET_TEMPLATE "memprof.XXXXXX"

static void   mp_server_class_init  (MPServerClass *class);
static void   mp_server_init        (MPServer      *server);
static void   mp_server_finalize    (GObject       *object);

static char *   find_lib_location     (MPServer     *server);
static void     create_control_socket (MPServer     *server);
static gboolean control_func          (GIOChannel   *source,
				       GIOCondition  condition,
				       gpointer      data);
struct _MPServer
{
	GObject parent_instance;
	
	char *socket_path;

	int socket_fd;

        /* It would be faster to implement this as a flat array with a
	 * binary search, but this is easier as a first pass
	 */
	GHashTable *pid_table;
	guint control_watch;

	MPProfileType profile_type;
	guint interval;		/* In usecs */
};

struct _MPServerClass {
	GObjectClass parent_class;

        void (*process_created) (MPServer *server, MPProcess *process);
};

/* Global list of socket paths so we can cleanup in an atexit() function
 */
static GSList *socket_paths;
static int terminate_pipe[2];

enum {
	PROCESS_CREATED,
	LAST_SIGNAL
};
static guint server_signals[LAST_SIGNAL] = { 0 };

static void
fatal (const char *format, ...)
{
	va_list va;

	va_start (va, format);
	vfprintf (stderr, format, va);

	exit (1);
}

GType
mp_server_get_type (void)
{
	static GType server_type = 0;

	if (!server_type) {
		static const GTypeInfo server_info = {
			sizeof (MPServerClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) mp_server_class_init,
			NULL, /* class_finalize */
			NULL, /* class_data */
			sizeof (MPServer),
			0, /* n_preallocs */
			(GInstanceInitFunc) mp_server_init
		};

		server_type = g_type_register_static (G_TYPE_OBJECT,
						      "MPServer",
						      &server_info, 0);
	}

	return server_type;
}

static void
mp_server_class_init (MPServerClass *class)
{
	static gboolean initialized = FALSE;

	GObjectClass *o_class = G_OBJECT_CLASS (class);

	o_class->finalize = mp_server_finalize;
	class->process_created = NULL;

	if (!initialized) {
		server_signals[PROCESS_CREATED] =
			g_signal_new ("process_created",
				      MP_TYPE_SERVER,
				      G_SIGNAL_RUN_LAST,
				      G_STRUCT_OFFSET (MPServerClass, process_created),
				      NULL, NULL,
				      g_cclosure_marshal_VOID__OBJECT,
				      G_TYPE_NONE, 1, MP_TYPE_PROCESS);

		initialized = TRUE;
	}
}

static void
mp_server_init (MPServer *server)
{
	GIOChannel *channel;

	server->profile_type = MP_PROFILE_MEMORY;
	server->interval = 10000;
	
	server->pid_table = NULL;
	create_control_socket (server);

	channel = g_io_channel_unix_new (server->socket_fd);
	g_io_channel_set_encoding (channel, NULL, NULL);
	
	server->control_watch = g_io_add_watch (channel, G_IO_IN | G_IO_PRI | G_IO_ERR | G_IO_HUP | G_IO_NVAL,
											control_func, server);
	g_io_channel_unref (channel);

	g_object_ref (G_OBJECT (server));
}

static void
mp_server_finalize (GObject *object)
{
	MPServer *server = MP_SERVER (object);
	
	if (server->control_watch)
		g_source_remove (server->control_watch);
	if (server->pid_table)
		g_hash_table_destroy (server->pid_table);
	close (server->socket_fd);

	socket_paths = g_slist_remove (socket_paths, server->socket_path);
	g_free (server->socket_path);
}

MPServer *
mp_server_new (void)
{
	MPServer *server = g_object_new (MP_TYPE_SERVER, NULL);

	return server;
}

void
mp_server_set_profile_type (MPServer       *server,
			    MPProfileType   type)
{

	g_return_if_fail (MP_IS_SERVER (server));
	server->profile_type = type;
}

void
mp_server_set_interval (MPServer *server,
			guint     usecs)
{
	g_return_if_fail (MP_IS_SERVER (server));
	
	server->interval = usecs;
}

int
mp_server_instrument (MPServer *server, const char *program, char **args)
{
	int pid;
	char *lib_location = find_lib_location (server);

	pid = fork();
	if (pid < 0)
		show_error (NULL, ERROR_FATAL, "Cannot fork: %s\n",
			    g_strerror (errno));

	if (pid == 0) {		/* Child  */
		gchar *envstr;
      
		envstr = g_strdup_printf ("%s=%s", "_MEMPROF_SOCKET", server->socket_path);
		putenv (envstr);

		envstr = g_strdup_printf ("%s=%s", "LD_PRELOAD", lib_location);
		putenv (envstr);

		if (server->profile_type != MP_PROFILE_MEMORY) {
			envstr = g_strdup_printf ("%s=%d", "_MEMPROF_INTERVAL", server->interval);
			putenv (envstr);
			
			envstr = g_strdup_printf ("%s=%s", "_MEMPROF_SPEED_TYPE",
						  server->profile_type == MP_PROFILE_CYCLES ?
						  "cycles" : "time");
			putenv (envstr);
		}

		execvp (program, args);

		g_warning ("Cannot run program: %s", g_strerror (errno));
		_exit(1);
	}

	g_free (lib_location);

	return pid;
}


static char *
find_lib_location (MPServer *server)
{
	const char **dirname;
	const char *basename;

	static const char *directories[] = {
		".libs",
		".",
		PKGLIBDIR,
		NULL
	};

	char *lib_location;
	
	if (server->profile_type == MP_PROFILE_MEMORY)
		basename = "libmemintercept.so";
	else
		basename = "libspeedintercept.so";
	
	lib_location = NULL;
	for (dirname = directories; *dirname; dirname++) {
		char *path = g_build_filename (*dirname, basename, NULL);
		if (!access (path, R_OK)) {
			lib_location = path;
			break;
		}
		g_free (path);
	}

	if (!lib_location)
		show_error (NULL, ERROR_FATAL,
			    _("Cannot find %s"), basename);

	/* Make lib_location absolute */

	if (lib_location[0] != '/') {
		char *wd = g_get_current_dir();
		char *newloc = g_strconcat (wd, "/", lib_location, NULL);
		g_free (lib_location);
		lib_location = newloc;
		g_free (wd);
	}

	return lib_location;
}

static void
cleanup_socket (void)
{
	g_slist_foreach (socket_paths, (GFunc)unlink, NULL);

#ifdef USE_SOCKET_DIRECTORY
	/* We try to remove the directory ; if there are other copies of memprof running, we expect
	 * this to fail
	 */
	{
		char *tmpdir = tmpdir = g_strdup_printf ("%s/memprof.%d", g_get_tmp_dir(), getuid());
		if (rmdir (tmpdir) != 0) {
			if (errno != ENOTEMPTY)
				g_warning ("Unlinking %s failed with error: %s\n", tmpdir, g_strerror (errno));
		}
		
		g_free (tmpdir);
	}
#endif
}

static void
term_handler (int signum)
{
	static int terminated = 0;
	char c = signum;
	int ret;

	if (terminated)
		exit(1);	/* Impatient user, risk reentrancy problems  */
	
	terminated = 1;

	ret = write (terminate_pipe[1], &c, 1);
}

static gboolean
terminate_io_handler (GIOChannel  *channel,
		      GIOCondition condition,
		      gpointer     data)
{
	char c;
	int ret;
	
	ret = read (terminate_pipe[0], &c, 1);

	fprintf (stderr, "memprof: Caught signal %d (%s), cleaning up\n", c, g_strsignal(c));

	exit (1);
}

static void
ensure_cleanup ()
{
	static gboolean added_cleanup;
	GIOChannel *channel;

	if (!added_cleanup) {
		g_atexit (cleanup_socket);

		signal (SIGINT, term_handler);
		signal (SIGTERM, term_handler);
		if (pipe (terminate_pipe) < 0)
			fatal ("bind: %s\n", g_strerror (errno));

		channel = g_io_channel_unix_new (terminate_pipe[0]);
		g_io_channel_set_encoding (channel, NULL, NULL);
		
		g_io_add_watch (channel, G_IO_IN | G_IO_PRI, terminate_io_handler, NULL);
		g_io_channel_unref (channel);
		
		added_cleanup = TRUE;
	}
}

static void
create_control_socket (MPServer *server)
{
	struct sockaddr_un addr;
	int addrlen;
	int retry_count = 5;
	mode_t old_mask = umask (077);

#ifdef USE_SOCKET_DIRECTORY
	char *tmpdir = g_strdup_printf ("%s/memprof.%d", g_get_tmp_dir(), getuid());
	struct stat st_buf;
#endif
	
	memset (&addr, 0, sizeof(addr));

	addr.sun_family = AF_UNIX;

	server->socket_fd = socket (PF_UNIX, SOCK_STREAM, 0);
	if (server->socket_fd < 0)
		g_error ("socket: %s\n", g_strerror (errno));

 retry:
	if (retry_count-- == 0)
		fatal ("Too many retries while creating control socket\n");

	if (server->socket_path)
		g_free (server->socket_path);

#ifdef USE_SOCKET_DIRECTORY	
	if (stat (tmpdir, &st_buf) == 0) {
		if (!S_ISDIR(st_buf.st_mode) || st_buf.st_uid != getuid())
			fatal ("memprof: %s not owned by user or is not a directory\n", tmpdir);

		if (chmod (tmpdir, 0700) != 0) {
			if (errno == ENOENT) {
				g_warning ("%s vanished, retrying\n", tmpdir);
				goto retry;
			} else
				fatal ("memprof: cannot set permissions on %s: %s\n", tmpdir, g_strerror (errno));
		}

	} else if (errno == ENOENT) {
		if (mkdir (tmpdir, 0700) != 0) { 
			if (errno == EEXIST)
				goto retry;
			else
				fatal ("memprof: Cannot create %s, %d", tmpdir, g_strerror (errno));
		}
	} else
		fatal ("memprof: error calling stat() on %s: %s\n", tmpdir, g_strerror (errno));

	server->socket_path = g_strdup_printf ("%s/server.%d", tmpdir, getpid());
	if (g_file_exists (server->socket_path)) {
		g_warning ("Stale memprof socket %s, removing\n", server->socket_path);
		unlink (server->socket_path);
	}
		
#else  /* !USE_SOCKET_DIRECTORY */
	server->socket_path = g_build_filename (g_get_tmp_dir(), SOCKET_TEMPLATE, NULL);
	if (strlen(mktemp (server->socket_path)) == 0)
		fatal ("mktemp: %s\n", g_strerror (errno));
#endif /* USE_SOCKET_DIRECTORY */

	strncpy (addr.sun_path, server->socket_path, sizeof (addr.sun_path));
	addrlen = sizeof(addr.sun_family) + strlen (addr.sun_path);
	if (addrlen > sizeof (addr))
		addrlen = sizeof(addr);

	if (bind (server->socket_fd, (struct sockaddr *)&addr, addrlen) < 0) {
		if (errno == EADDRINUSE)
			goto retry;
		else
			fatal ("bind: %s\n", g_strerror (errno));
	}

	ensure_cleanup ();

	socket_paths = g_slist_prepend (socket_paths, server->socket_path);

	if (listen (server->socket_fd, 8) < 0)
		fatal ("listen: %s\n", g_strerror (errno));

	umask (old_mask);
	
#ifdef USE_SOCKET_DIRECTORY	
	g_free (tmpdir);
#endif	
}

static void
mp_server_process_created (MPServer *server, MPProcess *process)
{
	g_signal_emit_by_name (server, "process_created", process);
}

/* Input func to receive new process connections */
static gboolean 
control_func (GIOChannel  *source,
	      GIOCondition condition,
	      gpointer     data)
{
	int newfd;
	MIInfo info;

	MPServer *server = data;
	
	MPProcess *process = NULL;
	MPProcess *parent_process;
	int count;
	char response = 0;

	newfd = accept (server->socket_fd, NULL, 0);
	if (newfd < 0) {
		g_warning ("accept: %s\n", g_strerror (errno));
		goto out;
	}

	count = read (newfd, &info, sizeof(info));
	if (count < sizeof(info)) {
		g_warning ("short read from new process\n");
		goto out;
	}

	switch (info.operation) {
	case MI_FORK:
		parent_process = mp_server_find_process (server, info.fork.pid);
		if (parent_process && !parent_process->follow_fork) {
			goto out; /* Return negative response */
		}

		/* Fall through */
	case MI_NEW:
		process = mp_server_find_process (server, info.fork.new_pid);
		if (process) {
			if (process->follow_exec) {
				MPProcess *old_process = process;
				
				process = process_new (server);
				process->pid = old_process->pid;
				process->parent = old_process->parent;
				
				mp_server_add_process (server, process); /* Overwrites old process */
				mp_server_process_created (server, process);

			} else
				process_exec_reset (process);
		}

		if (!process) {
			parent_process = mp_server_find_process (server, info.fork.pid);
			if (!parent_process) {
				/* This is a child of a forked child we aren't tracing, ignore it */
				goto out;
			}
			
			process = process_duplicate (parent_process);
			process->pid = info.fork.new_pid;
			mp_server_add_process (server, process);
			mp_server_process_created (server, process);
		}
		
		process_set_status (process, MP_PROCESS_RUNNING);
		
		break;
	case MI_CLONE:
		parent_process = mp_server_find_process (server, info.fork.pid);
		if (!parent_process)
			g_error ("Clone of nonexistant parent process\n");
		
		process = process_new (server);
		process_set_status (process, MP_PROCESS_RUNNING);
		
		process->clone_of = parent_process;
		process->pid = info.fork.new_pid;

		mp_server_add_process (server, process);

		while (parent_process->clone_of)
			parent_process = parent_process->clone_of;
			
		break;
		
	case MI_TIME:
	case MI_MALLOC:
	case MI_REALLOC:
	case MI_FREE:
	case MI_EXEC:
	case MI_EXIT:
		g_assert_not_reached ();
	}

	if (process) {
		process->input_channel = g_io_channel_unix_new (newfd);
		g_io_channel_set_encoding (process->input_channel, NULL, NULL);
		
		process_start_input (process);
		response = 1;
	}

 out:
	if (newfd >= 0) {
		int ret = write (newfd, &response, 1);
		if (!response || ret < 0)
			close (newfd);
	}

	return TRUE;
}

/************************************************************
 * Code to keep track of current processes
 ************************************************************/

MPProcess *
mp_server_find_process (MPServer *server, pid_t pid)
{
	if (server->pid_table) {
		return g_hash_table_lookup (server->pid_table, GUINT_TO_POINTER (pid));
	} else {
		return NULL;
	}
}

void
mp_server_add_process (MPServer *server, MPProcess *process)
{
	if (!server->pid_table)
		server->pid_table = g_hash_table_new (g_direct_hash, NULL);

	g_hash_table_insert (server->pid_table, GUINT_TO_POINTER (process->pid), process);
}

void
mp_server_remove_process (MPServer *server, MPProcess *process)
{
	MPProcess *in_table;
	
	g_return_if_fail (server->pid_table != NULL);

	/* In the case of --follow-exec, the process in the table could have been overwritten
	 */
	in_table = g_hash_table_lookup (server->pid_table, GUINT_TO_POINTER (process->pid));
	if (in_table == process)
		g_hash_table_remove (server->pid_table, GUINT_TO_POINTER (process->pid));
}

typedef struct {
	MPProcess *parent;
	GList *result;
} CloneInfo;

static void
add_clone (gpointer key, MPProcess *process, CloneInfo *ci)
{
	MPProcess *parent = process;
	while (parent->clone_of)
		parent = parent->clone_of;

	if (parent == ci->parent)
		ci->result = g_list_prepend (ci->result, process);
}

GList *
mp_server_get_process_clones (MPServer *server, MPProcess *process)
{
	CloneInfo ci;

	ci.parent = process;
	ci.result = NULL;
	
	g_hash_table_foreach (server->pid_table, (GHFunc)add_clone, &ci);

	return ci.result;
}
     
