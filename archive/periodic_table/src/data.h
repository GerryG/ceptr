#include "ceptr.h"


typedef size_t (*sizeFunction)(Receptor *, Symbol, void *);

sizeFunction size_table[BUFFER_SIZE];

sizeFunction size_table_get(Symbol noun) {
    if (size_table[noun] == 0) {
        raise_error("null size fn for noun %d \n", noun);
    }
    return size_table[noun];
}

int _data_sem_check(Data *d, Xaddr xaddr) {
    Symbol noun = _t_noun(_t_get_child(d->root,xaddr.key));
    return (noun == xaddr.noun);
}

void *_data_get(Data *d,int key) {
    return _t_get_child_surface(d->root,key);
}

void *data_get(Receptor *r, Xaddr xaddr) {
    if (!_data_sem_check(&r->data, xaddr)) {
        raise_error2("semcheck failed getting xaddr { %d, %d }\n", xaddr.key, xaddr.noun );
    }
    return _data_get(&r->data,xaddr.key);
}

void size_table_set(Symbol noun, sizeFunction func) {
    size_table[noun] = func;
}

void data_set(Receptor *r, Xaddr xaddr, void *value, size_t size) {
    Data *d = &r->data;
    void *surface = _data_get(d,xaddr.key);
    if (xaddr.key == 0 || !_data_sem_check(d, xaddr)) {
        raise_error("I do not think that word (%d) means what you think it means!\n", xaddr.noun);
    }
    memcpy(surface, value, size);
}

void *_data_new_uninitialized(Data *d, Xaddr *new_xaddr, Symbol noun, size_t size) {
    T *n = _t_new(d->root,noun,0,size);
    new_xaddr->key = _t_children(d->root);
    new_xaddr->noun = noun;
    return _t_surface(n);
}

Xaddr data_new(Receptor *r, Symbol noun, void *surface, size_t size) {
    Xaddr new_xaddr;
    //    printf("DATA_NEW: noun-%d,size-%ld\n",noun,size);
    _data_new_uninitialized(&r->data, &new_xaddr, noun, size);
    data_set(r, new_xaddr, surface, size);
    return new_xaddr;
}

void data_new_by_reference(Receptor *r, Xaddr *new_xaddr, Symbol noun, void *surface, size_t size) {
    _data_new_uninitialized(&r->data, new_xaddr, noun, size);
    data_set(r, *new_xaddr, surface, size);
}

Symbol data_new_noun(Receptor *r, Xaddr xaddr, char *label) {
    Data *d = &r->data;
    size_t s = sizeof(NounSurface)+strlen(label)+1;
    Xaddr x;
    NounSurface *ns = _data_new_uninitialized(d,&x,r->nounSpecXaddr.noun,s);
    ns->specXaddr = xaddr;
    memcpy(&ns->label,label,strlen(label)+1);
    return x.key;
}

void _data_init(Data *d) {
    scapes_init(d);
    d->root = _t_new_root(SURFACES_ARRAY_NOUN);

    LogMeta *l;
    d->log = _t_new(0,LOG_META_NOUN,0,sizeof(LogMeta));
    l = _t_surface(d->log);
    assert( pthread_mutex_init(&l->mutex, NULL) == 0);
    assert( pthread_cond_init(&l->changed, NULL) == 0);
}

void _data_free(Data *d) {
    _t_free(d->log);
    _t_free(d->root);
    scapes_free(d);
}

// this should go somewhere once the dependencies are better resolved.  right now it depends on everything.
Symbol spec_noun_for_noun(Receptor *r, Symbol noun);
size_t size_of_named_surface(Receptor *r, Symbol instanceNoun, void *surface) {
    size_t result = 0;
    if (instanceNoun <= 0){
        switch (instanceNoun) {
	case XADDR_NOUN:
	    result = sizeof(Xaddr);
	    break;
	case CSPEC_NOUN:
	    result = (size_t) 0;
	    break;
	case CSTRING_NOUN:
	    result = strlen((char *) surface) + 1;
	    break;
	case PATTERN_SPEC_DATA_NOUN:
	    result = sizeof(PatternSpecData);
	    break;
	default:
	    raise_error("can't get size of instance of %d \n", instanceNoun);
        }
    } else {
        result = (*size_table_get(spec_noun_for_noun(r, instanceNoun)))(r, instanceNoun, surface);
    }
    return result;
}
