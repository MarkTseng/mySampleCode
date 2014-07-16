/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * atm_svr_dce.c --
 *
 * Main module for ATM DCE server example
 */

#include <stdio.h>
#include <stdlib.h>
#include <dce/exc_handling.h>
#include <dce/rpc.h>
#include <dce/dce_error.h>
#include "atm_dce_interface.h"
#include "atm_dce.h"

#define SINTERFACE_SPEC            atm_v1_0_s_ifspec
#define MAX_CONC_CALLS_PROTSEQ     1
#define MAX_CONC_CALLS_TOTAL       1


main(argc, argv)
int argc;
char **argv;
{
  unsigned32               status;
  rpc_binding_vector_t     *binding_vector_p;

  
  DPRINTF(("SERVER: about to register interface\n"));

  /* register with runtime */
  rpc_server_register_if(SINTERFACE_SPEC, NULL, NULL, &status);
  if (status != rpc_s_ok) {
    fprintf(stderr, "ERROR: rpc_server_register_if\n");
    exit(1);
  }
  DPRINTF(("SERVER: interface registered\n"));

  /* designate a protocol sequence */
  rpc_server_use_all_protseqs(MAX_CONC_CALLS_PROTSEQ, &status);
  if (status != rpc_s_ok) {
    fprintf(stderr, "ERROR: rpc_server_use_all_protseq\n");
    exit(1);
  }
  DPRINTF(("SERVER: protocol sequence specified\n"));

  /* obtain binding vector from runtime */
  rpc_server_inq_bindings(&binding_vector_p, &status);
  if (status != rpc_s_ok) {
    fprintf(stderr, "ERROR: rpc_server_inq_bindings\n");
    exit(1);
  }
  DPRINTF(("SERVER: have binding vector from runtime\n"));
  

  /* register bindings with endpoint mapper */
  rpc_ep_register(SINTERFACE_SPEC, binding_vector_p, NULL, 
		  (unsigned_char_t *)EP_ANNO, &status);
  if (status != rpc_s_ok) {
    fprintf(stderr, "ERROR: rpc_ep_register\n");
    exit(1);
  }
  DPRINTF(("SERVER: endpoints registered\n"));

  /* export binding information to name service */
  rpc_ns_binding_export(rpc_c_ns_syntax_dce, (unsigned_char_t *)SERVER_NAME, 
			SINTERFACE_SPEC, 
			binding_vector_p, NULL, &status);
 if (status != rpc_s_ok) {
   REPORT_DCE_ERROR(status)
   exit(1);
  }
  DPRINTF(("SERVER: bindings exported\n"));


  atm_server_init(argc, argv);
  
  /* jump to runtime listening loop */
  DPRINTF(("SERVER: entering listen\n"));
  rpc_server_listen(MAX_CONC_CALLS_TOTAL, &status);
  if (status != rpc_s_ok) {
    fprintf(stderr, "ERROR: rpc_server_listen\n");
    exit(1);
  }
  
  DPRINTF(("SERVER: out of listen\n"));
}






