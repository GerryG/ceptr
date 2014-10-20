/**
 * @ingroup def
 *
 * @{
 * @file sys_defs.c
 * @brief definition of system semantic elements for bootstrapping
 *
 * @copyright Copyright (C) 2013-2014, The MetaCurrency Project (Eric Harris-Braun, Arthur Brock, et. al).  This file is part of the Ceptr platform and is released under the terms of the license contained in the file LICENSE (GPLv3).
 */

#include "tree.h"
#include "def.h"

#define sT(name,num,...) name = _d_define_structure(G_sys_defs.structures,"" #name "",SYS_CONTEXT,num,__VA_ARGS__)
#define sY(name,str) name = _d_declare_symbol(G_sys_defs.symbols,str,"" #name "",SYS_CONTEXT)
#define sP(name,intention,in,out) name = _d_code_process(G_sys_defs.processes,0,"" #name "",intention,in,out,SYS_CONTEXT)
#define sYt(name,str) name = _d_declare_symbol(G_sys_defs.symbols,str,"" #name "",TEST_CONTEXT)

Symbol NULL_SYMBOL = {0,SEM_TYPE_SYMBOL,0};
Symbol DEFINITIONS = {SYS_CONTEXT,SEM_TYPE_SYMBOL,DEFINITIONS_ID};
Symbol STRUCTURES = {SYS_CONTEXT,SEM_TYPE_SYMBOL,STRUCTURES_ID};
Symbol STRUCTURE_DEFINITION = {SYS_CONTEXT,SEM_TYPE_SYMBOL,STRUCTURE_DEFINITION_ID};
Symbol STRUCTURE_LABEL = {SYS_CONTEXT,SEM_TYPE_SYMBOL,STRUCTURE_LABEL_ID};
Symbol STRUCTURE_PARTS = {SYS_CONTEXT,SEM_TYPE_SYMBOL,STRUCTURE_PARTS_ID};
Symbol STRUCTURE_PART = {SYS_CONTEXT,SEM_TYPE_SYMBOL,STRUCTURE_PART_ID};
Symbol SYMBOLS = {SYS_CONTEXT,SEM_TYPE_SYMBOL,SYMBOLS_ID};
Symbol PROCESSES = {SYS_CONTEXT,SEM_TYPE_SYMBOL,PROCESSES_ID};
Symbol SYMBOL_DECLARATION = {SYS_CONTEXT,SEM_TYPE_SYMBOL,SYMBOL_DECLARATION_ID};
Symbol SYMBOL_STRUCTURE = {SYS_CONTEXT,SEM_TYPE_SYMBOL,SYMBOL_STRUCTURE_ID};
Symbol SYMBOL_LABEL = {SYS_CONTEXT,SEM_TYPE_SYMBOL,SYMBOL_LABEL_ID};

Structure NULL_STRUCTURE = {0,SEM_TYPE_STRUCTURE,0};
Structure BOOLEAN = {SYS_CONTEXT,SEM_TYPE_STRUCTURE,BOOLEAN_ID};
Structure INTEGER = {SYS_CONTEXT,SEM_TYPE_STRUCTURE,INTEGER_ID};
Structure FLOAT = {SYS_CONTEXT,SEM_TYPE_STRUCTURE,FLOAT_ID};
Structure CHAR = {SYS_CONTEXT,SEM_TYPE_STRUCTURE,CHAR_ID};
Structure CSTRING = {SYS_CONTEXT,SEM_TYPE_STRUCTURE,CSTRING_ID};
Structure SYMBOL = {SYS_CONTEXT,SEM_TYPE_STRUCTURE,SYMBOL_ID};
Structure ENUM = {SYS_CONTEXT,SEM_TYPE_STRUCTURE,ENUM_ID};
Structure TREE_PATH = {SYS_CONTEXT,SEM_TYPE_STRUCTURE,TREE_PATH_ID};
Structure XADDR = {SYS_CONTEXT,SEM_TYPE_STRUCTURE,XADDR_ID};
Structure LIST = {SYS_CONTEXT,SEM_TYPE_STRUCTURE,LIST_ID};
Structure SURFACE = {SYS_CONTEXT,SEM_TYPE_STRUCTURE,SURFACE_ID};
Structure TREE = {SYS_CONTEXT,SEM_TYPE_STRUCTURE,TREE_ID};
Structure RECEPTOR = {SYS_CONTEXT,SEM_TYPE_STRUCTURE,RECEPTOR_ID};
Structure PROCESS = {SYS_CONTEXT,SEM_TYPE_STRUCTURE,PROCESS_ID};
Structure STRUCTURE = {SYS_CONTEXT,SEM_TYPE_STRUCTURE,STRUCTURE_ID};
Structure SCAPE = {SYS_CONTEXT,SEM_TYPE_STRUCTURE,SCAPE_ID};

Process NULL_PROCESS = {0,SEM_TYPE_PROCESS,0};
Process IF = {SYS_CONTEXT,SEM_TYPE_PROCESS,IF_ID};
Process ADD_INT = {SYS_CONTEXT,SEM_TYPE_PROCESS,ADD_INT_ID};
Process SUB_INT = {SYS_CONTEXT,SEM_TYPE_PROCESS,SUB_INT_ID};
Process MULT_INT = {SYS_CONTEXT,SEM_TYPE_PROCESS,MULT_INT_ID};
Process DIV_INT = {SYS_CONTEXT,SEM_TYPE_PROCESS,DIV_INT_ID};
Process MOD_INT = {SYS_CONTEXT,SEM_TYPE_PROCESS,MOD_INT_ID};
Process EQ_INT = {SYS_CONTEXT,SEM_TYPE_PROCESS,EQ_INT_ID};
Process LT_INT = {SYS_CONTEXT,SEM_TYPE_PROCESS,LT_INT_ID};
Process GT_INT = {SYS_CONTEXT,SEM_TYPE_PROCESS,GT_INT_ID};
Process LTE_INT = {SYS_CONTEXT,SEM_TYPE_PROCESS,LTE_INT_ID};
Process GTE_INT = {SYS_CONTEXT,SEM_TYPE_PROCESS,GTE_INT_ID};
Process RESPOND = {SYS_CONTEXT,SEM_TYPE_PROCESS,RESPOND_ID};
Process INTERPOLATE_FROM_MATCH = {SYS_CONTEXT,SEM_TYPE_PROCESS,INTERPOLATE_FROM_MATCH_ID};

void def_sys() {
    G_sys_root = _t_new_root(DEFINITIONS);
    G_sys_defs.symbols = _t_newr(G_sys_root,SYMBOLS);
    G_sys_defs.structures = _t_newr(G_sys_root,STRUCTURES);
    G_sys_defs.processes = _t_newr(G_sys_root,PROCESSES);


    sT(BOOLEAN,1,NULL_SYMBOL);
    sT(INTEGER,1,NULL_SYMBOL);
    sT(FLOAT,1,NULL_SYMBOL);
    sT(CHAR,1,NULL_SYMBOL);
    sT(CSTRING,1,NULL_SYMBOL);
    sT(SYMBOL,1,NULL_SYMBOL);
    sT(ENUM,1,NULL_SYMBOL);
    sT(TREE_PATH,1,NULL_SYMBOL);
    sT(XADDR,1,NULL_SYMBOL);
    sT(LIST,1,NULL_SYMBOL);
    sT(SURFACE,1,NULL_SYMBOL);
    sT(TREE,1,NULL_SYMBOL);
    sT(RECEPTOR,1,NULL_SYMBOL);
    sT(PROCESS,1,NULL_SYMBOL);
    sT(STRUCTURE,1,NULL_SYMBOL);
    sT(SCAPE,1,NULL_SYMBOL);

    sY(DEFINITIONS,LIST);
    sY(STRUCTURES,LIST);
    sY(STRUCTURE_DEFINITION,TREE);
    sY(STRUCTURE_LABEL,CSTRING);
    sY(STRUCTURE_PARTS,LIST);
    sY(STRUCTURE_PART,SYMBOL);
    sY(SYMBOLS,LIST);
    sY(SYMBOL_DECLARATION,TREE);
    sY(SYMBOL_STRUCTURE,STRUCTURE);
    sY(SYMBOL_LABEL,CSTRING);

    sY(TRUE_FALSE,BOOLEAN);
    sY(SEMTREX_MATCHED_PATH,TREE_PATH);
    sY(SEMTREX_SYMBOL_LITERAL,SYMBOL);
    sY(SEMTREX_SEQUENCE,LIST);
    sY(SEMTREX_OR,NULL_STRUCTURE);
    sY(SEMTREX_SYMBOL_ANY,NULL_STRUCTURE);
    sY(SEMTREX_SYMBOL_EXCEPT,SYMBOL);
    sY(SEMTREX_ZERO_OR_MORE,NULL_STRUCTURE);
    sY(SEMTREX_ONE_OR_MORE,NULL_STRUCTURE);
    sY(SEMTREX_ZERO_OR_ONE,NULL_STRUCTURE);
    sY(SEMTREX_VALUE_LITERAL,NULL_STRUCTURE);
    sY(SEMTREX_GROUP,SYMBOL);
    sY(SEMTREX_DESCEND,NULL_STRUCTURE);
    sY(SEMTREX_WALK,NULL_STRUCTURE);
    sY(SEMTREX_MATCH,INTEGER);
    sY(SEMTREX_MATCH_RESULTS,NULL_STRUCTURE);
    sY(SEMTREX_MATCH_SYMBOL,SYMBOL);
    sY(SEMTREX_MATCH_SIBLINGS_COUNT,INTEGER);

    sY(ASCII_CHAR,CHAR);
    sY(ASCII_CHARS,LIST);

    sY(RECEPTOR_XADDR,XADDR);
    sY(FLUX,LIST);
    sY(SCAPE_SPEC,TREE);
    sY(ASPECTS,LIST);
    sY(ASPECT_DEF,TREE);
    sY(ASPECT_TYPE,BOOLEAN);
    sY(CARRIER,SYMBOL);
    sY(ASPECT,INTEGER);
    sY(SIGNALS,LIST);
    sY(SIGNAL,TREE);
    sY(ENVELOPE,LIST);
    sY(BODY,TREE);
    sY(LISTENERS,LIST);
    sY(LISTENER,SYMBOL);
    sY(EXPECTATION,NULL_STRUCTURE);
    sY(ACTION,PROCESS);
    sY(INTERPOLATE_SYMBOL,SYMBOL);
    sY(PROCESSES,LIST);
    sY(PROCESS_CODING,TREE);
    sY(PROCESS_NAME,CSTRING);
    sY(PROCESS_INTENTION,CSTRING);
    sY(INPUT,LIST);
    sY(INPUT_SIGNATURE,TREE);
    sY(INPUT_LABEL,CSTRING);
    sY(SIGNATURE_STRUCTURE,STRUCTURE);
    sY(OUTPUT_SIGNATURE,NULL_STRUCTURE);
    sY(RUN_TREE,TREE);
    sY(PARAM_REF,TREE_PATH);
    sY(PARAMS,LIST);
    sY(PROTOCOLS,LIST);
    sY(PROTOCOL,TREE);
    sY(ROLES,LIST);
    sY(ROLE,CSTRING);
    sY(INTERACTIONS,LIST);
    sY(INTERACTION,LIST);
    sY(STEP,CSTRING);
    sY(FROM_ROLE,ENUM);
    sY(TO_ROLE,ENUM);
    sY(RESPONSE_STEPS,LIST);
    sY(SCAPES,LIST);
    sY(VM_HOST_RECEPTOR,RECEPTOR);
    sY(COMPOSITORY,RECEPTOR);
    sY(MANIFEST,TREE);
    sY(MANIFEST_PAIR,LIST);
    sY(MANIFEST_LABEL,CSTRING);
    sY(MANIFEST_SPEC,SYMBOL);
    sY(RECEPTOR_PACKAGE,TREE);
    sY(RECEPTOR_IDENTIFIER,INTEGER);
    sY(INSTALLED_RECEPTOR,TREE);
    sY(ACTIVE_RECEPTORS,LIST);
    sY(PENDING_SIGNALS,LIST);
    sY(BINDINGS,TREE);
    sY(BINDING_PAIR,LIST);

    sP(IF,"if",0,0);
    sP(ADD_INT,"addition",0,0);
    sP(SUB_INT,"subtract",0,0);
    sP(MULT_INT,"multiply",0,0);
    sP(DIV_INT,"divide",0,0);
    sP(MOD_INT,"modulo",0,0);
    sP(EQ_INT,"test equality",0,0);
    sP(LT_INT,"test less than",0,0);
    sP(GT_INT,"test greater than",0,0);
    sP(LTE_INT,"test less than or equal",0,0);
    sP(GTE_INT,"test greater than or equal",0,0);
    sP(RESPOND,"respond to signal",0,0);
    sP(INTERPOLATE_FROM_MATCH,"interploate match result in place",0,0);

    sYt(TEST_INT_SYMBOL,INTEGER);
    sYt(TEST_INT_SYMBOL2,INTEGER);
    sYt(TEST_STR_SYMBOL,CSTRING);
    sYt(TEST_TREE_SYMBOL,TREE);
    sYt(TEST_TREE_SYMBOL2,TREE);
    sYt(TEST_NAME_SYMBOL,TREE);
    sYt(TEST_FIRST_NAME_SYMBOL,CSTRING);
    sYt(TEST_RECEPTOR_SYMBOL,RECEPTOR);
    sYt(TEST_ALPHABETIZE_SCAPE_SYMBOL,SCAPE);

    sY(STX_TOKENS,LIST);
    sY(STX_SL,NULL_STRUCTURE);
    sY(STX_OP,NULL_STRUCTURE);
    sY(STX_CP,NULL_STRUCTURE);
    sY(STX_LABEL,CSTRING);
    sY(STX_OG,CSTRING);
    sY(STX_CG,NULL_STRUCTURE);
    sY(STX_EQ,NULL_STRUCTURE);
    sY(STX_WALK,NULL_STRUCTURE);
    sY(STX_STAR,NULL_STRUCTURE);
    sY(STX_PLUS,NULL_STRUCTURE);
    sY(STX_Q,NULL_STRUCTURE);
    sY(STX_OR,NULL_STRUCTURE);
    sY(STX_COMMA,NULL_STRUCTURE);
    sY(STX_NOT,CSTRING);

    sY(STX_SIBS,LIST);
    sY(STX_CHILD,TREE);
    sY(STX_POSTFIX,TREE);

}
void sys_free() {
    _t_free(G_sys_root);
}