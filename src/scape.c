/**
 * @ingroup receptor
 *
 * @{
 * @file scape.c
 * @brief implementation of scapes
 *
 * @copyright Copyright (C) 2013-2016, The MetaCurrency Project (Eric Harris-Braun, Arthur Brock, et. al).  This file is part of the Ceptr platform and is released under the terms of the license contained in the file LICENSE (GPLv3).
 */

#include "scape.h"

/**
 * create a new scape
 *
 * @params[in] key_source the symbol type key hashes are of
 * @params[in] data_source the symbol type of xaddrs to be associated with keys
 * @returns a pointer to a newly allocated Scape
 *
 * <b>Examples (from test suite):</b>
 * @snippet spec/scape_spec.h testScapeNew
 *
 * called from vmhost with RECEPTOR_IDENTIFIER, RECEPTOR_SURFACE
 * that is saying that this is a map from Receptor ids (symbols) to receptor values
 * presumably receptory surface maps to a representation of the receptor such that
 * it can act as the identified receptor. The value will be a hash key that can be
 * used to find actual represetation for use in processing.
 */
Scape *_s_new(Symbol key_source,Symbol data_source) {
    Scape *new_scape = malloc(sizeof(Scape));
    new_scape->key_source = key_source;
    new_scape->data_source = data_source;
    new_scape->data = NULL;
    return new_scape;
}

// free the hash table data
void scapedataFree(ScapeData *i) {
    scape_elem *cur,*tmp;
    HASH_ITER(hh, *i, cur, tmp) {
    HASH_DEL(*i,cur);  /* delete; cur advances to next */
    free(cur);
    }
}

/**
 * free the memory allocated to a scape
 */
void _s_free(Scape *s) {
    scapedataFree(&s->data);
    free(s);
}
/**
 * add a new element into a scape
 *
 * @params[in] s the scape
 * @params[in] h a hash of the tree node which is the key
 * @params[in] xaddr the instance address to be scaped
 *
 * <b>Examples (from test suite):</b>
 * @snippet spec/scape_spec.h testScapeAddElement
 */
void _s_add(Scape *scape,TreeHash hash,Xaddr external) {
    ScapeData *scape_hash = &scape->data;
    scape_elem *scape_el;

    HASH_FIND_INT( *scape_hash, &hash, scape_el );
    if (scape_el) { raise_error("allready there!"); }
    else {
      scape_el = malloc(sizeof(struct scape_elem));
      scape_el->key = hash;
      scape_el->value = external;
      HASH_ADD_INT(*scape_hash, hash, scape_el);
    }
}

/**
 * retrieve an Xaddr from the scape
 *
 * @params[in] s the scape
 * @params[in] key the tree node which is the key
 * @returns Xaddr to the scape item
 *
 * <b>Examples (from test suite):</b>
 * @snippet spec/scape_spec.h testScapeAddElement
 */
Xaddr _s_get(Scape *scape,TreeHash hash_key) {
    Xaddr external = {0,0};
    scape_elem *elem = 0;
    ScapeData *scape_hash = &scape->data;

    HASH_FIND_INT( *scape_hash, &hash_key, elem );
    if (elem) return elem->value;
    return external;
}

/** @}*/
