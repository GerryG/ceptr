#include "tree.h"
#include "ceptr_error.h"

/*****************  Node creation */
int __t_append_child(Tnode *t,Tnode *c) {
    if (t->structure.child_count == 0) {
	t->structure.children = malloc(sizeof(Tnode *)*TREE_CHILDREN_BLOCK);
    } else if (!(t->structure.child_count % TREE_CHILDREN_BLOCK)){
	int b = t->structure.child_count/TREE_CHILDREN_BLOCK + 1;
	t->structure.children = realloc(t->structure.children,sizeof(Tnode *)*(TREE_CHILDREN_BLOCK*b));
    }

    t->structure.children[t->structure.child_count++] = c;
}

void __t_init(Tnode *t,Tnode *parent,Symbol symbol) {
    t->structure.child_count = 0;
    t->structure.parent = parent;
    t->contents.symbol = symbol;
    t->context.flags = 0;
    if (parent != NULL) {
	__t_append_child(parent,t);
    }
}

Tnode * _t_new(Tnode *parent,Symbol symbol,void *surface,size_t size) {
    Tnode *t = malloc(sizeof(Tnode));
    __t_init(t,parent,symbol);
    if (size) {
	t->context.flags |= TFLAG_ALLOCATED;
	t->contents.surface = malloc(size);
	if (surface)
	    memcpy(t->contents.surface,surface,size);
    }
    t->contents.size = size;
    return t;
}

Tnode * _t_newi(Tnode *parent,Symbol symbol,int surface) {
    Tnode *t = malloc(sizeof(Tnode));
    *((int *)&t->contents.surface) = surface;
    t->contents.size = sizeof(int);
    __t_init(t,parent,symbol);
    return t;
}

/*****************  Node deletion */

void __t_free_children(Tnode *t) {
    int c = t->structure.child_count;
    if (c > 0) {
	while(--c>=0) {
	    _t_free(t->structure.children[c]);
	}
	free(t->structure.children);
    }
    t->structure.child_count = 0;
}

//TODO: make this remove the child from the parent's child-list?
void _t_free(Tnode *t) {
    __t_free_children(t);
    if (t->context.flags & TFLAG_ALLOCATED)
	free(t->contents.surface);
    free(t);
}

/******************** Node data accessors */
int _t_children(Tnode *t) {
    return t->structure.child_count;
}

void * _t_surface(Tnode *t) {
    if (t->context.flags & TFLAG_ALLOCATED)
	return t->contents.surface;
    else
	return &t->contents.surface;
}

Symbol _t_symbol(Tnode *t) {
    return t->contents.symbol;
}

size_t _t_size(Tnode *t) {
    return t->contents.size;
}

/*****************  Tree navigation */

Tnode *_t_child(Tnode *t,int i) {
    if (i>t->structure.child_count || i < 1) return 0;
    return t->structure.children[i-1];
}

Tnode * _t_parent(Tnode *t) {
    return t->structure.parent;
}

Tnode * _t_root(Tnode *t) {
    Tnode *p;
    while ((p = _t_parent(t)) != 0) t = p;
    return t;
}

//TODO: this is very expensive if called all the time!!!
Tnode * _t_next_sibling(Tnode *t) {
    Tnode *p = _t_parent(t);
    if (p==0) return 0;
    int c = _t_children(p);
    for(int i=0;i<c;i++) {
	if (p->structure.children[i] == t) {
	    i++;
	    return i<c ? p->structure.children[i] : 0;
	}
    }
    return 0;
}

/*****************  Tree path based accesses */
int _t_path_equal(int *p1,int *p2){
    while(*p1 != TREE_PATH_TERMINATOR && *p2 != TREE_PATH_TERMINATOR)
	if (*(p1++) != *(p2++)) return 0;
    return *p1 == TREE_PATH_TERMINATOR && *p2 == TREE_PATH_TERMINATOR;
}

int _t_path_depth(int *p) {
    int i=0;
    while(*p++ != TREE_PATH_TERMINATOR) i++;
    return i;
}

void _t_pathcpy(int *dst_p,int *src_p) {
    while(*src_p != TREE_PATH_TERMINATOR) {
	*dst_p++ = *src_p++;
    }
    *dst_p = TREE_PATH_TERMINATOR;
}

void _t_path_parent(int *n,int *p) {
    if (*p == TREE_PATH_TERMINATOR) {
	raise_error0("unable to take parent of root\n");
    }
    while(*p != TREE_PATH_TERMINATOR) {
	*n++ = *p++;
    }
    *--n = TREE_PATH_TERMINATOR;
}

Tnode * _t_get(Tnode *t,int *p) {
    int i = *p++;
    Tnode *c;
    if (i == TREE_PATH_TERMINATOR)
	return t;
    else if (i == 0)
	//TODO: semantic check to make sure surface is a tree?
	c = *(Tnode **)(_t_surface(t));
    else
	c = _t_child(t,i);
    if (c == NULL ) return NULL;
    if (*p == TREE_PATH_TERMINATOR) return c;
    return _t_get(c,p);
}

void * _t_get_surface(Tnode *t,int *p) {
    Tnode *c = _t_get(t,p);
    if (c == NULL) return NULL;
    return _t_surface(c);
}

char * _t_sprint_path(int *fp,char *buf) {
    char *b = buf;
    int d=_t_path_depth(fp);
    for(int i=0;i<d;i++) {
	sprintf(b,"/%d",fp[i]);
	b += strlen(b);
    }
    return buf;
}
