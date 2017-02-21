/**
 * @defgroup tree Semantic Trees
 *
 * @brief Semantic trees are the basic data building block for ceptr
 *
 * Everything in ceptr is built out of semantic trees.  In ceptr all data is assumed to be
 * meaningfull, not just structured.
 *
 * @{
 * @file tree.h
 * @brief semantic trees header file
 *
 * @copyright Copyright (C) 2013-2016, The MetaCurrency Project (Eric Harris-Braun, Arthur Brock, et. al).  This file is part of the Ceptr platform and is released under the terms of the license contained in the file LICENSE (GPLv3).
 *
 */

#ifndef _CEPTR_TREE_H
#define _CEPTR_TREE_H

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "ceptr.h"
#include "hashfn.h"

#define TREE_CHILDREN_BLOCK 5
#define TREE_PATH_TERMINATOR 0xFFFFFFFF

enum TreeSurfaceFlags {TFLAG_ALLOCATED=0x0001, TFLAG_SURFACE_IS_TREE=0x0002, TFLAG_SURFACE_IS_RECEPTOR = 0x0004, TFLAG_SURFACE_IS_SCAPE=0x0008, TFLAG_SURFACE_IS_CPTR=0x0010, TFLAG_DELETED=0x0020, TFLAG_RUN_NODE=0x0040, TFLAG_REFERENCE=0x8000};

/*****************  Node creation and deletion*/
TreeNode *__t_new(TreeNode *t, Symbol symbol, void *surface, size_t size, bool is_run_node);
#define _t_new(p, sy, su, s) __t_new(p, sy, su, s, 0)
TreeNode *__t_newc(TreeNode *t, Symbol symbol, char c, bool is_run_node);
#define _t_newc(parent, symbol, c) __t_newc(parent, symbol, c, 0)
TreeNode *__t_newi(TreeNode *parent, Symbol symbol, int surface, bool is_run_node);
#define _t_newi(p, sy, su) __t_newi(p, sy, su, 0)
TreeNode *__t_newi64(TreeNode *parent, Symbol symbol, long surface, bool is_run_node);
#define _t_newi64(p, sy, su) __t_newi64(p, sy, su, 0)
TreeNode *__t_news(TreeNode *parent, Symbol symbol, SemanticID surface, bool is_run_node);
#define _t_news(parent, symbol, surface) __t_news(parent, symbol, surface, 0)
TreeNode *_t_newt(TreeNode *parent, Symbol symbol, TreeNode *t);
#define _t_new_str(parent, symbol, str) __t_new_str(parent, symbol, str, 0)
TreeNode *__t_new_str(TreeNode *parent, Symbol symbol, char *str, bool is_run_node);
TreeNode *_t_new_root(Symbol symbol);
#define _t_newr(p, s) __t_newr(p, s, 0)
TreeNode *__t_newr(TreeNode *parent, Symbol symbol, bool is_run_node);
TreeNode *_t_new_receptor(TreeNode *parent, Symbol symbol, Receptor *r);
TreeNode *_t_new_scape(TreeNode *parent, Symbol symbol, Scape *s);
TreeNode *_t_new_cptr(TreeNode *parent, Symbol symbol, void *s);
TreeNode *_t_newp(TreeNode *parent, Symbol symbol, Process surface);

void _t_add(TreeNode *t, TreeNode *c);
void _t_detach_by_ptr(TreeNode *t, TreeNode *c);
TreeNode *_t_detach_by_idx(TreeNode *t, int i);
void _t_replace(TreeNode *t, int i, TreeNode *r);
void _t_replace_node(TreeNode *t, TreeNode *r);
TreeNode *_t_swap(TreeNode *t, int i, TreeNode *r);
void _t_insert_at(TreeNode *t, int *path, TreeNode *i);
void _t_morph(TreeNode *dst, TreeNode *src);
void __t_morph(TreeNode *t, Symbol s, void *surface, size_t length, int allocate);
void __t_free_children(TreeNode *t);
void __t_free(TreeNode *t);
void _t_free(TreeNode *t);
TreeNode *_t_clone(TreeNode *t);
TreeNode *_t_rclone(TreeNode *t);

TreeNode *_t_build(SemTable *sem, TreeNode *t, ...);
TreeNode *_t_build2(SemTable *sem, TreeNode *t, ...);
TreeNode *__t_tokenize(char *s);
TreeNode *_t_parse(SemTable *sem, TreeNode *parent, char *s, ...);

#define _t_fill_template(t, i) __t_fill_template(t, i, false)
bool __t_fill_template(TreeNode *template, TreeNode *items, bool as_run_node);

/******************** Node data accessors */
int _t_children(TreeNode *t);
void * _t_surface(TreeNode *t);
Symbol _t_symbol(TreeNode *t);
size_t _t_size(TreeNode *t);

/*****************  Tree navigation */
TreeNode * _t_parent(TreeNode *t);
TreeNode *_t_child(TreeNode *t, int i);
TreeNode * _t_root(TreeNode *t);
TreeNode * _t_next_sibling(TreeNode *t);
int _t_node_index(TreeNode *t);
#define _t_find(t, sym) __t_find(t, sym, 1)
TreeNode *__t_find(TreeNode *t, Symbol sym, int start_child);

/*****************  Tree path based accesses */
int _t_path_equal(int *p1, int *p2);
int _t_path_depth(int *p);
void _t_pathcpy(int *dst_p, int *src_p);
TreeNode * _t_get(TreeNode *t, int *p);
TreeNode * _t_getv(TreeNode *t, ...);
int *_t_get_path(TreeNode *t);
void * _t_get_surface(TreeNode *t, int *p);
char * _t_sprint_path(int *fp, char *buf);
TreeNode * _t_path_walk(TreeNode *t, int **pathP, int *lenP );

/*****************  Tree hashing utilities */
TreeHash _t_hash(SemTable *sem, TreeNode *t);
int _t_hash_equal(TreeHash h1, TreeHash h2);

/*****************  UUID utilities */
UUIDt __uuid_gen();
int __uuid_equal(UUIDt *u1, UUIDt *u2);

/*****************  Tree serialization */
size_t __t_serialize(SemTable *sem, TreeNode *t, void **bufferP, size_t offset, size_t current_size, int compact);
void _t_serialize(SemTable *sem, TreeNode *t, void **surfaceP, size_t *sizeP);
TreeNode * _t_unserialize(SemTable *sem, void **surfaceP, size_t *lengthP, TreeNode *t);

char * _t2rawjson(SemTable *sem, TreeNode *t, int level, char *buf);
char * _t2json(SemTable *sem, TreeNode *t, int level, char *buf);

/*****************  Misc... */

int _t_write(SemTable *sem, TreeNode *t, Stream *stream);

#define DO_KIDS(t, x) {int i, _c=_t_children(t);for(i=1;i<=_c;i++){x;}}

#define root_check(c) if (c->structure.parent != 0) {raise_error("can't add a node that isn't a root!");}

#endif
/** @}*/
