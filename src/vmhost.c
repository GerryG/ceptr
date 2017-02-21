/**
 * @ingroup vmhost
 *
 * @{
 * @file vmhost.c
 * @brief virtual machine host receptor implementation
 *
 * @copyright Copyright (C) 2013-2016, The MetaCurrency Project (Eric Harris-Braun, Arthur Brock, et. al).  This file is part of the Ceptr platform and is released under the terms of the license contained in the file LICENSE (GPLv3).
 */

#include "ceptr.h"
/******************  create and destroy virtual machine */


/* 
 * allocates and initializes vm from a table with base definitions loaded
 *
 * creates the root (system) receptor
 * creates a single namespace (scape) mapping receptor idetifiers to surfaces
 */
VMHost *__v_init(SemTable *table) {
    VMHost *vm = malloc(sizeof(VMHost));
    memset(vm,0,sizeof(VMHost));

    vm->ceptr = _r_new(table, SYS_RECEPTOR)
    vm->sem = table;
    vm->installed_receptors = _s_new(RECEPTOR_IDENTIFIER,RECEPTOR_SURFACE);

    vm->vm_thread.state = 0;
    vm->active_receptor_count = 0;
    vm->receptor_count = 0;
    vm->clock_thread.state = 0;

    return vm;
}

/**
 * @brief Creates a new virtual machine host
 *
 * allocates all the memory needed in the heap
 *
 * @returns pointer to a newly allocated VMHost

 * <b>Examples (from test suite):</b>
 * @snippet spec/vmhost_spec.h testVMHostCreate
 */
VMHost * _v_new() {
    VMHost *vm = __v_init(_base_sem_table()); // init vm with base defs
    load_system(vm) // load system definitions (sys_defs)
    return vm;
}

/**
 * Destroys a vmhost freeing all memory it uses.
 *
 * @param[in] v the VMHost to free
 */
void _v_free(VMHost *v) {
    _r_free(v->ceptr);
    _s_free(v->installed_receptors);
    _t_free(_t_root(v->sem->stores[0].definitions));
    _sem_free(v->sem);
    free(v);
}

/*
 * add a receptor from definition (surface?)
 */
Xaddr _v_add_receptor(VMHost *vm, Receptory parent, TreeNode ceptr_def) {
  _v_new_receptor(vm, parent, ceptr_def, _r_new(vm->sem, ceptr_def));
}

/**
 * Add a receptor package into the local compository to make it available
 * for installation and binding
 *
 * @param[in] vm VMHost in which to install the receptor
 * @param[in] p receptor package
 * @returns Xaddr of the receptor package in the compository
 * @todo validate signature and checksums??
 *
 * <b>Examples (from test suite):</b>
 * @snippet spec/vmhost_spec.h testVMHostLoadReceptorPackage
 */
// vm.load_receptor_package(ceptr_package)
Xaddr _v_load_receptor_package(VMHost *vm,TreeNode *ceptr_package) {
    return _r_load_receptor_package(vm->ceptr, ceptr_package);
}

/**
 * install a receptor into vmhost, creating a symbol for it
 *
 * @param[in] vm VMHost in which to install the receptor
 * @param[in] package xaddr of package to install
 * @param[in] bindings completed manifest which specifies how the receptor will be installed
 * @param[in] label label to be used for the semantic name for this receptor
 * @returns Xaddr of the instance
 *
 * <b>Examples (from test suite):</b>
 * @snippet spec/vmhost_spec.h testVMHostInstallReceptor
 * pkgtree: (1:manifest((N, S) ...) 2:id )
 * bindiings: ((N, V) ... )
 *
 * monifest should validate (names ? no) V should have S structure
 */
Xaddr _v_install_ceptr(VMHost *vm, Xaddr package, Treenode *bindings, char *label) {
    raise_error("not implemented");
    TreeNode *pkgtree; // = _r_get_instance(vm->ceptr, package);
    TreeNode *id = _t_child(pkgtree, 2);
    TreeHash key = _t_hash(vm->ceptr->sem, id); // hash type id?

    // make sure we aren't re-installing an already installed receptor
    Xaddr x = _s_get(vm->installed_receptors,h);
    if (!(is_null_xaddr(x))) return G_null_xaddr;
    _s_add(vm->installed_receptors, key, package);

    // confirm that the bindings match the manifest
    // @todo expand the manifest to allow optional binding, etc, using
    // semtrex to do the matching instead of assuming positional matching
    if (bindings) {
        TreeNode *manifest = _t_child(pkgtree,1);
        int mani_cnt = _t_children(manifest);
        // WTF, is this a list of pairs or even numbered list (flattened)?
        if (mani_cnt % 2) {raise_error("manifest must have even number of children!");}
        int i;
        for(i=1; i<=mani_cnt; i+=2) { // +=1 if not flattened
            TreeNode *manifest_ptr = _t_child(manifest, i);
            TreeNode *surface = _t_child(manifest_ptr, 2);
            //TreeNode *surface = _t_child(manifest_ptr, i+1); // if flattened
            TreeNode *binding_ptr = _t_child(bindings, i);
            if (!binding_ptr) {
                raise_error("missing binding for %s",(char *)_t_surface(_t_child(manifest_ptr,1)));
            }
            TreeNode *bind_value = _t_child(binding_ptr,2);
            Symbol spec = *(Symbol *)_t_surface(surface); // list of symbols (child types)
            if (semeq(_t_symbol(bind_value),spec)) {
                T *symbols = _t_child(pkgtree,3);
                raise_error("bindings symbol %s doesn't match spec %s",_sem_get_name(vm->ceptr->sem,_t_symbol(bind_value)),_sem_get_name(vm->ceptr->sem,spec));
            }
        }
    }

    Symbol symbol = _r_define_symbol(vm->ceptr, RECEPTOR, label);

    raise_error("fix semtable");
    Receptor *ceptr = _r_new_receptor_from_package(NULL, symbol, pkgtree, bindings);
    return _v_new_receptor(vm, vm->ceptr, symbol, ceptr);
}

Xaddr _v_new_receptor(VMHost *vm,Receptor *parent,Symbol name, Receptor *parent) {
    TreeNode *ceptr = _t_new_receptor(NULL,name,parent);
    if (vm->receptor_count > MAX_RECEPTORS) { raise_error("too many receptors"); }
    int next_idx = vm->receptor_count++;
    vm->routing_table[next_idx].r=parent;
    vm->routing_table[next_idx].s=name;
    parent->addr.addr = next_idx;

    //@todo what ever else is needed at the vmhost level to add the receptor's
    // process queue to the process tables etc...
    return _r_new_instance(parent,ceptr);
}

/**
 * Activate a receptor
 *
 * @param[in] v VMHost
 * @param[in] x Xaddr of receptor to activate
 *
 */
void _v_activate(VMHost *v, Xaddr x) {
    if (v->active_receptor_count+1 >= MAX_ACTIVE_RECEPTORS) {
        raise_error("too many active receptors");
    }
    T *t = _r_get_instance(v->ceptr,x);
    Receptor *r = __r_get_receptor(t);
    int c = v->active_receptor_count++;
    v->active_receptors[c].r=r;
    v->active_receptors[c].x=x;

    // handle special cases
    if (semeq(x.symbol,CLOCK_RECEPTOR)) {
        _v_start_thread(&v->clock_thread,___clock_thread,r);
    }
}

/**
 * scaffolding for send a signal from ouside the VMhost
 *
 * <b>Examples (from test suite):</b>
 * @snippet spec/vmhost_spec.h testVMHostActivateReceptor
 */
void _v_send(VMHost *v,ReceptorAddress from,ReceptorAddress to,Aspect aspect,Symbol carrier,T *contents) {
    T *s = __r_make_signal(from,to,aspect,carrier,contents,0,0,0);
    T *x = _r_send(v->ceptr,s);
    _t_free(x);
}

/**
 * walk through the list of signals and send them
 */
void _v_send_signals(VMHost *v,T *signals) {
    while(_t_children(signals)>0) {
        T *s = _t_detach_by_idx(signals,1);
        T *r = _r_send(v->ceptr,s);
        _t_free(r);  //@todo WHAT????  throwing away the rsult??
    }
}

/// @todo this should probably be implemented in a scape but for now
// we just loop through all instances searching for a match
/* Xaddr __v_get_receptor_xaddr(Instances *instances,Receptor *r) { */
/*     instances_elem *e = 0; */
/*     SemanticID sid = {r->parent,SEM_TYPE_RECEPTOR,r->context}; */
/*     Xaddr result = {sid,0}; */
/*     HASH_FIND_INT( *instances, &sid, e ); */
/*     if (e) { */
/*         Instance *iP = &e->instances; */

/*         instance_elem *curi,*tmpi; */
/*         HASH_ITER(hh, *iP, curi, tmpi) { */
/*             if (__r_get_receptor(curi->instance) == r) { */
/*                 result.addr = curi->addr; */
/*                 return result; */
/*             } */
/*         } */
/*     } */
/*     return result; */
/* } */

/**
 * scaffolding function for signal delivery
 */
void _v_deliver_signals(VMHost *v, Receptor *sender) {
    T *signals = sender->pending_signals;

    while(_t_children(signals)>0) {
        T *s = _t_detach_by_idx(signals,1);
        T *head = _t_getv(s,SignalMessageIdx,MessageHeadIdx,TREE_PATH_TERMINATOR);

        ReceptorAddress *toP = (ReceptorAddress *)_t_surface(_t_child(_t_child(head,HeadToIdx),1));
        ReceptorAddress *fromP = (ReceptorAddress *)_t_surface(_t_child(_t_child(head,HeadFromIdx),1));

        // if the from or to address is "self" (-1) we find the senders self
        // fix the values in the signal we are about to deliver.

        if (fromP->addr == SELF_RECEPTOR_ADDR) {
            *fromP = __r_get_self_address(sender);
        }

        Receptor *r;
        if (toP->addr == SELF_RECEPTOR_ADDR) {
            *toP = __r_get_self_address(sender);
        }
        else  {
            if (toP->addr >= v->receptor_count) {
                raise_error("to address: %d doesn't exist!",toP->addr);
            }
            r = v->routing_table[toP->addr].r;
        }

        Error err = _r_deliver(r,s);
        if (err) {
            raise_error("delivery error: %d",err);
        }
    }
}

/**
 * this is the VMhost main monitoring and execution thread
 */
void *__v_process(void *arg) {
    VMHost *v = (VMHost *) arg;
    int c,i;

    while(v->ceptr->state == Alive) {
        // make sure everybody's doing the right thing...
        // reallocate threads as necessary...
        // do edge-receptor type stuff..
        // what ever other watchdoggy type things are necessary...
        //        printf ("something\n");
        //    sleep(1);

        // for now we will check all receptors for any active contexts and
        // we will reduce them here.  Really this should be a thread pool manager
        // where we put allocate receptor's queues for processing according to
        // priority/etc...

        for (i=0;v->ceptr->state == Alive && i<v->active_receptor_count;i++) {
            Receptor *r = v->active_receptors[i].r;
            if (r->q && r->q->contexts_count > 0) {
                _p_reduceq(r->q);
            }
            // send any signals generated by the reduction
            _v_deliver_signals(v,r);

            // cleanup any fully reduced run-trees
            if (r->q->completed) _p_cleanup(r->q);
        }
    }

    // close down all receptors
    for (i=0;i<v->active_receptor_count;i++) {
        Receptor *r = v->active_receptors[i].r;
        __r_kill(r);
        // if other receptors have threads associated with them, the possibly we should
        // be doing a thread_join here, or maybe even inside __r_kill @fixme
    }

    int err =0;
    pthread_exit(&err);  //@todo determine if we should use pthread_exit or just return 0
    return 0;
}

// fire up the threads that make the vmhost work
void _v_start_vmhost(VMHost *v) {
    _v_start_thread(&v->vm_thread,__v_process,v);
}

/**
 * create all the built in receptors that exist in all VMhosts
 */
void _v_instantiate_builtins(VMHost *v) {
    Receptor *r = _r_makeClockReceptor(v->sem);
    Xaddr clock = _v_new_receptor(v,v->ceptr,CLOCK_RECEPTOR,r);
    _v_activate(v,clock);
}

/******************  thread handling */

void _v_start_thread(thread *t,void *(*start_routine)(void*), void *arg) {
    int rc;
    if (t->state) {
        raise_error("attempt to double-start a thread");
    }
    rc = pthread_create(&t->pthread,0,start_routine,arg);
    if (rc){
        raise_error("Error starting thread; return code from pthread_create() is %d\n", rc);
    }
    t->state = 1;
}

void _v_join_thread(thread *t) {
    if (t->state) { // make sure the thread was started before trying to join it
        void *status;
        int rc;

        rc = pthread_join(t->pthread, &status);
        if (rc) {
            raise_error("ERROR; return code from pthread_join() is %d\n", rc);
        }
        t->state = 0;
    }
}

/** @}*/
