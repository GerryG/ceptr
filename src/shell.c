/**
 * @ingroup receptor
 *
 * @{
 * @file shell.c
 * @brief implementation of a command line shell receptor
 *
 * @copyright Copyright (C) 2013-2016, The MetaCurrency Project (Eric Harris-Braun, Arthur Brock, et. al).  This file is part of the Ceptr platform and is released under the terms of the license contained in the file LICENSE (GPLv3).
 */

#include "ceptr.h"
#include "shell.h"

void addCommand(Receptor *ceptr, ReceptorAddress out_xaddr, char *command,
                char *desc, TreeNode *code, TreeNode *bindings_handler) {

    TreeNode *expect = _t_new_root(PATTERN); 
    _t_new_string(
      _t_new_node(
        _t_new_node(
          _sl(_t_new_sym(expect , SEMTREX_GROUP, SHELL_COMMAND), SHELL_COMMAND),
           SEMTREX_VALUE_LITERAL
        ), SEMTREX_VALUE_SET
      ), VERB, command
    );

    TreeNode *say = _t_new_root(SAY);
    __r_make_addr(say, TO_ADDRESS, out_xaddr);

    _t_new_sym(say, ASPECT_IDENT, DEFAULT_ASPECT);
    _t_new_sym(say, CARRIER, NULL_SYMBOL);

    // if code is actually an INITIATE then we will have a bindings handler
    // to which we want to add the SAY command as the ACTUAL_PROCESS
    // and we will replace the p with code which does the proper protocol
    // initiation.  Kinda weird, I know...
    if (bindings_handler) {
        char proc_name[255] = "handle ";
        strcpy(&proc_name[7], command);
        int pt1[] = {2, 1, TREE_PATH_TERMINATOR};
        _t_new(say, PARAM_REF, pt1, sizeof(int)*3);
        Process proc = _r_define_process(ceptr, say, proc_name, "long desc...", NULL, NULL);
        _t_new_sym(bindings_handler, ACTUAL_PROCESS, proc);
        say = code;
    }
    else {
        _t_add(say, code);
    }

    Process proc = _r_define_process(ceptr, say, desc, "long desc...", NULL, NULL);
    TreeNode *act = _t_newp(0, ACTION, proc);

    _r_add_expectation(ceptr, DEFAULT_ASPECT, SHELL_COMMAND, expect, act, 0, 0, NULL, NULL);
}

/*
 * vm         - the host
 * input      - file handle of stdin
 * output     - file handle of stdout
 * error      - should have stderr equiv
 * in_ceptr   - output ptr for in-ceptr
 * out_ceptr  - output ptr for out-ceptr
 * in_stream  - output ptr for the input stream
 * out_stream - output ptr for the output stream
 */
void makeShell(VMHost *vm, FILE *input, FILE *output, Receptor **in_ceptr,
               Receptor **out_ceptr, Stream **in_stream, Stream **out_stream) {
    // define and then create the shell receptor
    Symbol shell = _d_define_receptor(vm->ceptr->sem, "shell", __r_make_definitions(),
      DEV_COMPOSITORY_CONTEXT);
    Receptor *shell_ceptr = _r_new(vm->sem, shell);
    _v_activate(vm, _v_register_ceptr(vm, vm->ceptr, shell, shell_ceptr));

    // create edge ceptrs for stdin/out (store streams)
    *out_stream = _st_new_unix_stream(output, 0);
    *in_stream = _st_new_unix_stream(input, 1);

    // create and store edge ceptr - in
    *in_ceptr = _r_makeStreamEdgeReceptor(vm->sem);
    _r_addReader(*in_ceptr, *in_stream, shell_ceptr->addr, DEFAULT_ASPECT, parse_line, LINE, false);
    Xaddr in_xaddr = _v_register_ceptr(vm, vm->ceptr, STREAM_EDGE, *in_ceptr);
    _v_activate(vm, in_xaddr);

    *out_ceptr = _r_makeStreamEdgeReceptor(vm->sem);
    _r_addWriter(*out_ceptr, *out_stream, DEFAULT_ASPECT);
    Xaddr out_xaddr = _v_register_ceptr(vm, vm->ceptr, STREAM_EDGE, *out_ceptr);
    _v_activate(vm, out_xaddr);

    // set up shell to express the line parsing protocol when it receives LINES from the stream reader
    Protocol clp;
    __sem_get_by_label(vm->sem, "PARSE_COMMAND_FROM_LINE", &clp, DEV_COMPOSITORY_CONTEXT);
    T *bindings = _t_new_root(PROTOCOL_BINDINGS);
    T *res = _t_new_node(bindings, RESOLUTION);
    T *which_ceptr = _t_new_node(res, WHICH_RECEPTOR);
    _t_new_sym(which_ceptr, ROLE, LINE_SENDER);
    __r_make_addr(which_ceptr, ACTUAL_RECEPTOR, (*in_ceptr)->addr);
    res = _t_new_node(bindings, RESOLUTION);
    which_ceptr = _t_new_node(res, WHICH_RECEPTOR);
    _t_new_sym(which_ceptr, ROLE, COMMAND_RECEIVER);
    __r_make_addr(which_ceptr,ACTUAL_RECEPTOR, shell_ceptr->addr);
    res = _t_new_node(bindings, RESOLUTION);
    which_ceptr = _t_new_node(res, WHICH_SYMBOL);
    _t_new_sym(which_ceptr, USAGE, COMMAND_TYPE);
    _t_new_sym(which_ceptr, ACTUAL_SYMBOL, SHELL_COMMAND);

    _o_express_role(shell_ceptr, clp, COMMAND_RECEIVER, DEFAULT_ASPECT, bindings);
    _t_free(bindings);

    // set up shell to use the CLOCK TELL_TIME protocol for the time command
    Protocol time;
    __sem_get_by_label(vm->sem, "time", &time, CLOCK_CONTEXT);
    T *code = _t_new_root(INITIATE_PROTOCOL);
    _t_new_sym(code, PNAME, time);
    _t_new_sym(code, WHICH_INTERACTION, tell_time);
    bindings = _t_new_node(code, PROTOCOL_BINDINGS);
    res = _t_new_node(bindings, RESOLUTION);
    which_ceptr = _t_new_node(res, WHICH_RECEPTOR);
    _t_new_sym(which_ceptr, ROLE, TIME_HEARER);
    __r_make_addr(which_ceptr, ACTUAL_RECEPTOR, shell_ceptr->addr);
    res = _t_new_node(bindings, RESOLUTION);
    which_ceptr = _t_new_node(res, WHICH_RECEPTOR);
    _t_new_sym(which_ceptr, ROLE, TIME_TELLER);
    ReceptorAddress clock_addr = {3}; // @todo bogus!!! fix getting clock address somehow
    __r_make_addr(which_ceptr, ACTUAL_RECEPTOR, clock_addr);
    res = _t_new_node(bindings, RESOLUTION);
    which_ceptr = _t_new_node(res, WHICH_PROCESS);
    _t_new_sym(which_ceptr, GOAL, RESPONSE_HANDLER);

    ReceptorAddress out_addr = (*out_ceptr)->addr;
    addCommand(shell_ceptr, out_addr, "time", "get time", code, which_ceptr);

    // (expect (on flux SHELL_COMMAND:receptor) action (send std_out (convert_to_lines (send vmhost receptor-list))))

    code = _t_new_int(0, MAGIC, MagicReceptors);
    addCommand(shell_ceptr, out_addr, "receptors", "get receptor list", code, NULL);

    // (expect (on flux SHELL_COMMAND:receptor) action (send std_out (convert_to_lines (send vmhost shutdown)))
    code = _t_new_int(0, MAGIC, MagicQuit);
    addCommand(shell_ceptr, out_addr, "quit", "shut down the vmhost", code, NULL);

    // (expect (on flux SHELL_COMMAND:debug) action (send std_out (convert_to_lines (magic toggle debug)))
    code = _t_new_int(0, MAGIC, MagicDebug);
    addCommand(shell_ceptr, out_addr, "debug", "toggle debug mode", code, NULL);

}

/** @}*/
