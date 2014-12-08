/**
 * @ingroup tree
 *
 * @{
 *
 * @file mtree.c
 * @brief semantic tree matrix implementation
 *
 * @copyright Copyright (C) 2013-2014, The MetaCurrency Project (Eric Harris-Braun, Arthur Brock, et. al).  This file is part of the Ceptr platform and is released under the terms of the license contained in the file LICENSE (GPLv3).
 */

#include "mtree.h"
#include "ceptr_error.h"
#include "hashfn.h"
#include "def.h"

const H null_H = {0,{NULL_ADDR,NULL_ADDR}};

void __m_add_level(M *m) {
    if (!m->levels++) {
	m->lP = malloc(sizeof(L));
    }
    else {
	m->lP = realloc(m->lP,sizeof(L)*m->levels);
    }
    int i = m->levels-1;
    m->lP[i].nodes = 0;
}
#define _GET_LEVEL(h,l) (&(h).m->lP[l])
#define _GET_NODE(h,l,i) (&(l)->nP[i])
#define GET_LEVEL(h) _GET_LEVEL(h,(h).a.l)
#define GET_NODE(h,l) _GET_NODE(h,l,(h).a.i)
/**
 * Create a new tree node
 *
 * @param[in] parent handle to parent node
 * @param[in] symbol semantic symbol for the node to be create
 * @param[in] surface pointer to node's data
 * @param[in] size size in bytes of the surface
 * @returns updated handle
 */
H _m_new(H parent,Symbol symbol,void *surface,size_t size) {
    H h;
    L *l = 0;

    if (parent.m) {
	h.m = parent.m;
	h.a.l = parent.a.l+1;

	if (parent.a.l >= parent.m->levels) {
	    raise_error0("address too deep!");
	}
	else if (parent.a.l == parent.m->levels-1) {
	    h.a.i = 0;
	    __m_add_level(h.m);
	}
	else {
	    l = GET_LEVEL(h);
	    h.a.i = l->nodes;
	}
    }
    else {
	h.m  = malloc(sizeof(M));
	h.m->magic = matrixImpl;
	h.m->levels = 0;
	h.a.l = 0;
	h.a.i = 0;
	__m_add_level(h.m);
    }
    if (!l) l = GET_LEVEL(h);

    // add a node
    // @todo make this not realloc each time!!
    N *n,*nl;
    if (!l->nodes++) {
    	l->nP = malloc(sizeof(N));
	n = GET_NODE(h,l);
    }
    else {
    	l->nP = realloc(l->nP,sizeof(N)*l->nodes);
	n = _GET_NODE(h,l,l->nodes-1);
    }
    n->symbol = symbol;
    n->size = size;
    n->parenti = parent.m ? parent.a.i : 0;
    if (size) {
	if (size == sizeof(int)) {
	    n->flags = 0;
	    *((int *)&n->surface) = *(int *)surface;
	}
	else {
	    n->flags = TFLAG_ALLOCATED;
	    n->surface = malloc(size);
	    if (surface)
		memcpy(n->surface,surface,size);
	}
    }
    else {
	n->flags = 0;
    }

    return h;
}

H _m_newi(H parent,Symbol symbol,int surface) {
    return _m_new(parent,symbol,&surface,sizeof(int));
}

N *__m_get(H h) {
    L *l = GET_LEVEL(h);
    N *n = GET_NODE(h,l);
    return n;
}

size_t _m_size(H h) {return __m_get(h)->size;}

void _m_free(H h) { free(h.m);}

int _m_children(H h) {
    Mlevel levels = h.m->levels;

    if (h.a.l >= levels) {
	raise_error0("address too deep!");
    }
    else if (h.a.l == levels-1) {
	return 0;
    }
    L *l = _GET_LEVEL(h,h.a.l+1);
    Mindex c = 0;
    Mindex i = 0,pi = h.a.i;
    Mindex max = l->nodes;
    N *n = &l->nP[0];

/* this works if nodes are sorted
    while (i < max && n->parenti != h.a.i) {
	n++;i++;
    }
    while (i < max && n->parenti == h.a.i) {
	n++;i++;c++;
    }
*/
    while (i < max) {
	if (pi == n->parenti) c++;
	n++;i++;
    }

    return c;
}

void * _m_surface(H h) {
    N *n = __m_get(h);
    if (n->flags & TFLAG_ALLOCATED)
	return n->surface;
    else
	return &n->surface;
}

Maddr _m_parent(H h) {
    Maddr a = {NULL_ADDR,NULL_ADDR};
    if (h.a.l > 0) {
	N *n = __m_get(h);
	a.l = h.a.l-1;
	a.i = n->parenti;
    }
    return a;
}

Maddr _m_child(H h,Mindex c) {
    Maddr a = {NULL_ADDR,NULL_ADDR};
    Mlevel levels = h.m->levels;
    if (h.a.l >= levels) {
	raise_error0("address too deep!");
    }
    else if (h.a.l == levels-1) {
	return a;
    }
    a.l = h.a.l+1;
    L *l = &h.m->lP[a.l];
    Mindex ci = 0,max = l->nodes;
    N *n = &l->nP[0];
    if (c == NULL_ADDR) {
	a.i = NULL_ADDR;
	while(ci < max) {
	    if (n->parenti == h.a.i) a.i = ci;
	    ci++;
	    n++;
	}
	if (a.i == NULL_ADDR)
	    a.l = NULL_ADDR;
    }
    else {
	a.i = 0;
	while(a.i < max) {
	    if (n->parenti == h.a.i) ci++;
	    if (ci == c) return a;
	    a.i++;n++;
	}
	a.l = NULL_ADDR;
	a.i = NULL_ADDR;
    }
/* works if nodes are sorted
    //skip past nodes of children of parents before our parent
    while (a.i < max && n->parenti < h.a.i) {
	a.i++;n++;
    }

    // if you pass in NULL_ADDR for the child,
    // this routine returns the last child address
    if (c == NULL_ADDR) {
	while (a.i < max && n->parenti == h.a.i) {
	    a.i++;n++;
	}
	a.i--;
	if (a.i == -1) a.l = NULL_ADDR;
    }
    else {
	if (a.i+c > l->nodes) {
	    raise_error0("address too deep!");
	}
	a.i += c-1;
    }
*/

    return a;
}

Symbol _m_symbol(H h) {
    N *n = __m_get(h);
    return n->symbol;
}

Maddr _m_next_sibling(H h) {
    L *l = GET_LEVEL(h);
    int i = h.a.i+1;
    Maddr r;
    N *n = GET_NODE(h,l);
    Mindex pi = n->parenti;
    while (i<l->nodes) {
	n++;
	if (n->parenti == pi) {
	    r.l = h.a.l;
	    r.i = i;
	    return r;
	}
	i++;
    }
    return null_H.a;
}
/** @}*/
