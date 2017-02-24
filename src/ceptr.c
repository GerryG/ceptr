/**
 * @file ceptr.c
 * @copyright Copyright (C) 2013-2016, The MetaCurrency Project (Eric Harris-Braun, Arthur Brock, et. al).  This file is part of the Ceptr platform and is released under the terms of the license contained in the file LICENSE (GPLv3).
 *
 * @brief implements main() entry point for the ceptr node
 */

#include "sys_defs.h"
#include "shell.h"

void setupHTTP(VMHost *vm) {

    Symbol HTTP;
    _sem_get_by_label(vm->sem, "HTTP", &HTTP);

    // create empty edge receptor
    Receptor *edge_ceptr = _r_makeStreamEdgeReceptor(vm->sem);
    // instantiate it in the vmhost
    Xaddr edge = _v_register_ceptr(vm, vm->ceptr, STREAM_EDGE, edge_ceptr);
    // set up a socket listener that will transcode ascii to HTTP_REQUEST and send all the received requests to an HTTP aspect on the same receptor
    TreeNode *code = _t_parse( edge_ceptr->sem, 0,
      "(CONVERSE (SCOPE (ITERATE (PARAMS) (STREAM_ALIVE (PARAM_REF:/2/1)) (INITIATE_PROTOCOL (PNAME:HTTP) (WHICH_INTERACTION:backnforth) (PROTOCOL_BINDINGS (RESOLUTION (WHICH_RECEPTOR (ROLE:HTTP_CLIENT) %)) (RESOLUTION (WHICH_RECEPTOR (ROLE:HTTP_SERVER) %)) (RESOLUTION (WHICH_PROCESS (GOAL:RESPONSE_HANDLER) (ACTUAL_PROCESS:echo2stream))) (RESOLUTION (WHICH_USAGE (USAGE:RESPONSE_HANDLER_PARAMETERS) (ACTUAL_VALUE (PARAM_REF:/2/1)))) (RESOLUTION (WHICH_VALUE (ACTUAL_SYMBOL:HTTP_REQUEST) (ACTUAL_VALUE (STREAM_READ (PARAM_REF:/2/1) (RESULT_SYMBOL:HTTP_REQUEST)))))) ) ) (STREAM_CLOSE (PARAM_REF:/2/1))) (BOOLEAN:1))",
       __r_make_addr(0, ACTUAL_RECEPTOR, edge_ceptr->addr),
       __r_make_addr(0, ACTUAL_RECEPTOR, edge_ceptr->addr) );
    // add an error handler that just completes the iteration
    TreeNode *err_handler = _t_parse(edge_ceptr->sem, 0,
      "(CONTINUE (POP_PATH (PARAM_REF:/4/1/1) (RESULT_SYMBOL:CONTINUE_LOCATION) (POP_COUNT:5)) (CONTINUE_VALUE (BOOLEAN:0)))");

    // var not used SocketListener *listener =
    _r_addListener(edge_ceptr, 8888, code, 0, err_handler, DELIM_CRLF);
    _v_activate(vm, edge);

    TreeNode *bindings = _t_new_root(PROTOCOL_BINDINGS);
    TreeNode *res = _t_new_node(bindings, RESOLUTION);
    TreeNode *which_ceptr = _t_new_node(res, WHICH_RECEPTOR);
    _t_new_sym(which_ceptr, ROLE, HTTP_SERVER);
    __r_make_addr(which_ceptr, ACTUAL_RECEPTOR, edge_ceptr->addr);
    res = _t_new_node(bindings, RESOLUTION);
    which_ceptr = _t_new_node(res, WHICH_PROCESS);
    _t_new_sym(which_ceptr, GOAL, HTTP_REQUEST_HANDLER);
    _t_new_sym(which_ceptr, ACTUAL_PROCESS, fill_i_am);

    _o_express_role(edge_ceptr, HTTP, HTTP_SERVER, HTTP_ASPECT, bindings);
    _t_free(bindings);
}

int main(int argc, const char **argv) {

    G_sem = def_sys();
    //char *dname = "tmp/shell_vm";
    //_a_boot(dname);

    debug_enable(D_SIGNALS+D_BOOT+D_STEP+D_STREAM);
    // set up the vmhost
    G_vm = _v_new();
    _v_instantiate_builtins(G_vm);

    Stream *output_stream, *input_stream;
    Receptor *i_r,*o_r;

    makeShell(G_vm,stdin,stdout,&i_r,&o_r,&input_stream,&output_stream);
    output_stream->flags &= ~StreamCloseOnFree; // don't close the stdout on free...
    input_stream->flags &= ~StreamCloseOnFree; // don't close the stdin on free...
    input_stream->flags |= StreamLoadByLine;   // don't wait for EOF when loading from stdin!!

    setupHTTP(G_vm);

    _v_start_vmhost(G_vm);

    while (G_vm->ceptr->state == Alive) {
        sleepms(100);
    };

    _v_join_thread(&G_vm->clock_thread);
    _v_join_thread(&G_vm->vm_thread);

    sys_free(G_sem);
    return 0;
}
