/**
 * @file sys_defs.h
 * @brief Defines types and vars for system defined semantics
 *
 * @copyright Copyright (C) 2013-2015, The MetaCurrency Project (Eric Harris-Braun, Arthur Brock, et. al).  This file is part of the Ceptr platform and is released under the terms of the license contained in the file LICENSE (GPLv3).
 * @ingroup def
 */

#ifndef _CEPTR_SYS_DEFS_H
#define _CEPTR_SYS_DEFS_H

#include "ceptr_types.h"
#include "semtable.h"

SemTable *G_sem;

#define is_sys_symbol(s) (s.context == SYS_CONTEXT)
#define is_sys_process(s) (is_process(s) && (s.context == SYS_CONTEXT))
#define is_sys_structure(s) (is_structure(s) && (s.context == SYS_CONTEXT))

Structure NULL_SYMBOL;
Process NULL_PROCESS;
Structure NULL_STRUCTURE;

typedef struct ContextStore1 {
    T *root;
    Structure *structures;  ///< pointer for quick access to structures
    Symbol *symbols;     ///< pointer for quick access to symbols
    Process *processes;   ///< pointer for quick access to processes
} ContextStore1;

ContextStore1 G_contexts[_NUM_DEFAULT_CONTEXTS];
Context G_ctx;
char * G_label;
// helper macros for setting up the base system definitions, these macros are
// used in the code generated by base_defs.pl.
// NOte: symbols that are defined via a Declare line in the source file, cannot be
// redefined, and thus are just set using the sYs macro.
#define sT(ctx,name,num,...) name = _d_define_structure_v(sem,"" #name "",ctx,num,__VA_ARGS__)
#define sTs(ctx,name,def) G_ctx = ctx;G_label=""#name"";name = _d_define_structure(sem,"" #name "",def,ctx);
#define sY(ctx,name,str) name = _d_define_symbol(sem,str,"" #name "",ctx)
#define sYs(ctx,sym,str) __d_set_symbol_structure(__sem_get_defs(sem,SEM_TYPE_SYMBOL,ctx),sym,str)
#define sP(ctx,name,intention,...) name = _d_define_process(sem,0,"" #name "",intention,__p_make_signature(__VA_ARGS__),ctx)

#define sT_SET(...) sT_(sem,STRUCTURE_SYMBOL_SET,__VA_ARGS__)
#define sT_SEQ(...) sT_(sem,STRUCTURE_SEQUENCE,__VA_ARGS__)
#define sT_STAR(...) sT_(sem,STRUCTURE_ZERO_OR_MORE,1,__VA_ARGS__)
#define sT_PLUS(...) sT_(sem,STRUCTURE_ONE_OR_MORE,1,__VA_ARGS__)
#define sT_QMRK(...) sT_(sem,STRUCTURE_ZERO_OR_ONE,1,__VA_ARGS__)
#define sT_SYM(sym) _t_news(0,STRUCTURE_SYMBOL,sym)
#define sT_PCNT(str) _t_news(0,SYMBOL_OF_STRUCTURE,str)
#define sT_OR(x,y) sT_(sem,STRUCTURE_OR,2,x,y)
#define sT_BANG _t_newr(0,STRUCTURE_ANYTHING)

T *sT_(SemTable *sem,Symbol sym,int num_params,...);

#endif
