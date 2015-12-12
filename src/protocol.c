/**
 * @ingroup receptor
 *
 * @{
 * @file protocol.c
 * @brief implementation of protocol helpers
 *
 * @copyright Copyright (C) 2013-2015, The MetaCurrency Project (Eric Harris-Braun, Arthur Brock, et. al).  This file is part of the Ceptr platform and is released under the terms of the license contained in the file LICENSE (GPLv3).
 */

#include "protocol.h"
#include "vmhost.h"
#include "semtrex.h"
#include "debug.h"

extern VMHost *G_vm;
// search the children of a tree for symbol matching sym
T *_t_find_child(T *t,Symbol sym) {
    T *p;
    DO_KIDS(t,
            puts(_sem_get_name(G_vm->r->sem,_t_symbol(_t_child(t,i))));
            if (semeq(_t_symbol(p=_t_child(t,i)),sym)) return p;

            );
    return NULL;
}

/**
 * helper to build a PROTOCOL_DEFINITION
 *
 * NOTE: this just makes it easier to build protocol defs in c.  If you don't
 * get the params right when calling it, it will segfault!
 *
 * @param[inout] protocols a protocols def tree containing protocols which will be added to
 * @param[in] label the name of the protocol
 * @param[in] ... params in correct order
 *
 */
T *_o_make_protocol_def(SemTable *sem,Context c,char *label,...) {
    va_list params;
    va_start(params,label);

    Symbol state = PROTOCOL_SEMANTICS;
    bool in_conv = false;
    T *p = _t_new_root(PROTOCOL_DEFINITION);
    _t_new_str(p,PROTOCOL_LABEL,label);
    T *t = _t_newr(p,PROTOCOL_SEMANTICS);
    bool done = false;
    Symbol param;
    bool pop = false;
    while(!done) {
        if (pop) {
            t = _t_parent(t);
            p = _t_parent(t);
            state = _t_symbol(t);
            if (semeq(_sem_get_symbol_structure(sem,state),INTERACTION)) state = INTERACTION;

            pop = false;
            debug(D_PROTOCOL,"popping to state %s with param %s\n",_sem_get_name(sem,state),_sem_get_name(sem,param));
        }
        else {
            param = va_arg(params,Symbol);
            debug(D_PROTOCOL,"reading %s in state %s\n",_sem_get_name(sem,param),_sem_get_name(sem,state));
        }
        if (semeq(state,PROTOCOL_DEFINITION)) {
            if (semeq(param,INTERACTION)) {
                state = INTERACTION;
            }
            else if (semeq(param,INCLUSION)) {
                p = t;
                state = INCLUSION;
            }
            else if (semeq(param,NULL_SYMBOL)) {
                done = true;
            }
            else raise_error("expecting ROLE,GOAL,USAGE,INCLUSION,INTERACTION or NULL_SYMBOL got %s",_sem_get_name(sem,param));
        }
        if (semeq(state,PROTOCOL_SEMANTICS)) {
            if (semeq(param,ROLE)) {
                Symbol role = va_arg(params,Symbol);
                _t_news(t,ROLE,role);
            }
            else if (semeq(param,GOAL)) {
                Symbol role = va_arg(params,Symbol);
                _t_news(t,GOAL,role);
            }
            else if (semeq(param,USAGE)) {
                Symbol role = va_arg(params,Symbol);
                _t_news(t,USAGE,role);
            }
            else pop = true;
        }
        if (semeq(state,INTERACTION)) {
            if (semeq(param,INTERACTION)) {
                char *l = va_arg(params,char*);
                Symbol interaction = _d_define_symbol(sem,INTERACTION,l,c);
                t = _t_newr(t,interaction);
            }
            else if (semeq(param,EXPECT) || semeq(param,INITIATE)) {
                p = t;
                state = param;
            }
            else pop = true;
        }
        if (semeq(state,INITIATE)) {
            if (semeq(param,INITIATE)) {
                t = _t_newr(p,INITIATE);
                Symbol role = va_arg(params,Symbol);
                _t_news(t,ROLE,role);
                T *s = _t_newr(t,DESTINATION);
                Symbol source = va_arg(params,Symbol);
                _t_news(s,ROLE,source);
                T *action = va_arg(params,T*);
                _t_add(t,action);
            }
            else pop = true;
        }
        if (semeq(state,EXPECT)) {
            if (semeq(param,EXPECT)) {
                t = _t_newr(p,EXPECT);
                Symbol role = va_arg(params,Symbol);
                _t_news(t,ROLE,role);
                T *s = _t_newr(t,SOURCE);
                Symbol source = va_arg(params,Symbol);
                _t_news(s,ROLE,source);
                T *pattern = va_arg(params,T*);
                _t_add(t,pattern);
                T *action = va_arg(params,T*);
                _t_add(t,action);
            }
            else pop = true;
        }
        if (semeq(state,INCLUSION)) {
            if (semeq(param,INCLUSION)) {
                t = _t_newr(p,INCLUSION);
                Symbol pname = va_arg(params,Symbol);
                _t_news(t,PNAME,pname);
            }
            else if (semeq(param,WHICH_SYMBOL)) {
                T *l = _t_newr(t,RESOLUTION);
                T *w = _t_newr(l,WHICH_SYMBOL);
                _t_news(w,USAGE,va_arg(params,Symbol));
                _t_news(w,ACTUAL_SYMBOL,va_arg(params,Symbol));
            }
            else if (semeq(param,WHICH_PROCESS)) {
                T *l = _t_newr(t,RESOLUTION);
                T *w = _t_newr(l,WHICH_PROCESS);
                _t_news(w,GOAL,va_arg(params,Process));
                _t_news(w,ACTUAL_PROCESS,va_arg(params,Process));
            }
            else if (semeq(param,WHICH_RECEPTOR)) {
                T *l = _t_newr(t,RESOLUTION);
                T *w = _t_newr(l,WHICH_RECEPTOR);
                _t_news(w,ROLE,va_arg(params,Symbol));
                _t_news(w,ACTUAL_RECEPTOR,va_arg(params,Symbol));
            }
            else if (semeq(param,WHICH_USAGE)) {
                T *l = _t_newr(t,CONNECTION);
                T *w = _t_newr(l,WHICH_USAGE);
                _t_news(w,USAGE,va_arg(params,Symbol));
                _t_news(w,USAGE,va_arg(params,Symbol));
            }
            else if (semeq(param,WHICH_GOAL)) {
                T *l = _t_newr(t,CONNECTION);
                T *w = _t_newr(l,WHICH_GOAL);
                _t_news(w,GOAL,va_arg(params,Process));
                _t_news(w,GOAL,va_arg(params,Process));
            }
            else if (semeq(param,WHICH_ROLE)) {
                T *l = _t_newr(t,CONNECTION);
                T *w = _t_newr(l,WHICH_ROLE);
                _t_news(w,ROLE,va_arg(params,Symbol));
                _t_news(w,ROLE,va_arg(params,Symbol));
            }
            else pop = true;
        }
    }
    va_end(params);
    return t;
}

/**
 * recursively expand wrapped protocol defs
 *
 * @param[in] sem SemTable of contexts
 * @param[in] def protocol definition tree
 * @param[out] unwrapped protocol definition tree
 */
T * _o_unwrap(SemTable *sem,T *def) {
    T *d = _t_clone(def);
    int i;
    for (i=1;i<=_t_children(d);i++) {
        T *t = _t_child(d,i);
        if (semeq(_t_symbol(t),INCLUSION)) {
            Protocol p = *(Protocol *)_t_surface(_t_child(t,InclusionPnameIdx));
            T *ps = _sem_get_defs(sem,p);
            T *p_def = _o_unwrap(sem,_t_child(ps,p.id));  // do the recursive unwrapping
            int j,c = _t_children(t);
            T *bindings = NULL;
            for(j=InclusionPnameIdx+1;j<=c;j++) {
                T *x = _t_child(t,j); // get the connection or resolution
                if (semeq(_t_symbol(x),CONNECTION)) {
                    T *w = _t_child(x,ConnectionWhichIdx);  // get the which

                    T *v = _t_clone(_t_child(w,1)); // get the source
                    T *stx = _t_new_root(SEMTREX_WALK);
                    T *g = _t_news(stx,SEMTREX_GROUP,_t_symbol(v));
                    T *vl = _t_newr(g,SEMTREX_VALUE_LITERAL);
                    _t_add(vl,v);
                    T *r = _t_child(w,2);
                    /* printf("matching %s\n",_t2s(sem,stx)); */
                    /* printf("against %s\n",_t2s(sem,p_def)); */
                    /* printf("to replace %s\n",_t2s(sem,r)); */
                    _stx_replace(stx,p_def,r);
                    /* printf("results in %s\n",_t2s(sem,p_def)); */
                    _t_free(stx);
                }
                else if (semeq(_t_symbol(x),RESOLUTION)) {
                    if (!bindings) bindings = _t_new_root(PROTOCOL_BINDINGS);
                    _t_add(bindings,_t_clone(x));
                }
                else raise_error("expecting CONNECTION or RESOLUTION");
            }
            if (bindings) {
                _o_resolve(sem,p_def,bindings);
            }

            // after doing the semantics mapping from the CONNECTIONS and RESOLUTIONS
            // we need to add into the parent semantics and items that weren't resolved
            // or connected for later binding
            T *unwrapped_semantics = _t_child(p_def,ProtocolDefSemanticsIdx);

            // but first remove any bound items from the PROTOCOL_SEMANTICS because those
            // don't need to be merged into the parents semantics
            if (bindings) {
                int i,c = _t_children(bindings);
                for(i=1;i<=c;i++) {
                    T *x = _t_child(bindings,i);
                    x = _t_child(_t_child(x,ResolutionWhichIdx),1);
                    Symbol symx = _t_symbol(x);
                    Symbol symxs = *(Symbol *) _t_surface(x);
                    T *parent_semantics = _t_child(d,ProtocolDefSemanticsIdx);
                    int j,b = _t_children(unwrapped_semantics);
                    // search the parent semantics for matching item and remove it.
                    for(j=1;j<=b;j++) {
                        T *y = _t_child(unwrapped_semantics,j);
                        if (semeq(symx,_t_symbol(y)) && semeq(symxs,*(Symbol *) _t_surface(y))) {
                            _t_free(_t_detach_by_idx(unwrapped_semantics,j));
                            break;
                        }
                    }
                }

                _t_free(bindings);
            }

            T *parent_semantics = _t_child(d,ProtocolDefSemanticsIdx);
            T *x;
            while(x = _t_detach_by_idx(unwrapped_semantics,1)) {
                Symbol sym = _t_symbol(x);
                c = _t_children(parent_semantics);
                for (j=1;j<=c;j++) {
                    if (semeq(sym,_t_symbol(_t_child(parent_semantics,j)))) break;
                }
                // not found so we can add it
                if (j>c) _t_add(parent_semantics,x);
                //          else _t_free(x);
            }
            _t_detach_by_ptr(d,t);  // remove the INCLUSION specs
            // add in the unwrapped interactions

            while(x = _t_detach_by_idx(p_def,ProtocolDefSemanticsIdx+1)) {
                _t_add(d,x);
            }
            _t_free(p_def);
        }
    }

    return d;
}

/**
 * convert a, possibly composed, protocol definition into an e
 *
 * convert a PROTOCOL_DEF to a concertized version according to the
 * RESOLUTIONS and CONNECTIONS in the the def and the bindings
 *
 * @param[in] sem SemTable contexts
 * @param[in,out] def protocol definition to resolve
 * @param[in] role the role that needs resolving
 * @param[in] bindings the PROTOCOL_BINDINGS tree with additional RESOLUTIONS
 */
void _o_resolve(SemTable *sem,T *def, T *bindings){
    T *d = def;
    if (bindings) {
        T *gstx = NULL;
        T *gv;
        T *ustx = NULL;
        T *uv;
        debug(D_PROTOCOL,"resolving bindings %s\n",t2s(bindings));
        DO_KIDS(bindings,
                T *res = _t_child(bindings,i);
                T *w = _t_child(res,ResolutionWhichIdx);
                debug(D_PROTOCOL,"resolving %s\n",t2s(w));
                if (semeq(_t_symbol(w),WHICH_PROCESS)) {
                    T *goal = _t_child(w,1);
                    // build the stx if we haven't already
                    if (!gstx) {
                        // %EXPECT/.*,<ACTION:GOAL=goal>
                        gstx = _t_new_root(SEMTREX_WALK);
                        T *s = _sl(gstx,EXPECT);
                        s = _t_newr(s,SEMTREX_SEQUENCE);
                        _t_newr(_t_newr(s,SEMTREX_ZERO_OR_MORE),SEMTREX_SYMBOL_ANY);
                        T *g = _t_news(s,SEMTREX_GROUP,ACTION);
                        T *vl = _t_newr(g,SEMTREX_VALUE_LITERAL);
                        gv = _t_news(vl,GOAL,*(Process *)_t_surface(goal));
                    }
                    else *(Process *)_t_surface(gv) = *(Process *)_t_surface(goal);

                    // replace the GOAL with the ACTUAL
                    T *a = _t_news(0,ACTION,*(Process *)_t_surface(_t_child(w,2)));
                    _stx_replace(gstx,d,a);
                    _t_free(a);
                }
                else if (semeq(_t_symbol(w),WHICH_SYMBOL)) {
                    T *usage = _t_child(w,1);
                    // build the stx if we haven't already
                    if (!ustx) {
                        // %SEMTREX_SYMBOL_LITERAL/<SEMTREX_SYMBOL:USAGE=usage>
                        ustx = _t_new_root(SEMTREX_WALK);
                        T *s = _sl(ustx,SEMTREX_SYMBOL_LITERAL);
                        T *g = _t_news(s,SEMTREX_GROUP,SEMTREX_SYMBOL);
                        T *vl = _t_newr(g,SEMTREX_VALUE_LITERAL);
                        uv = _t_news(vl,USAGE,*(Symbol *)_t_surface(usage));
                    }
                    else *(Symbol *)_t_surface(uv) = *(Symbol *)_t_surface(usage);

                    // replace the USAGE with the ACTUAL
                    T *a = _t_news(0,SEMTREX_SYMBOL,*(Symbol *)_t_surface(_t_child(w,2)));
                    _stx_replace(ustx,d,a);
                    _t_free(a);
                }
            );
        if (gstx) _t_free(gstx);
        if (ustx) _t_free(ustx);
    }
}

/**
 * setup a receptor to participate as some role in a protocol
 *
 * @param[in] receptor in which to express the protocol role
 * @param[in] protocol which protocol
 * @param[in] role symbol of role to express
 * @param[in] aspect on which aspect to install the expectations
 * @param[in] bindings PROTOCOL_BINDINGs tree to concretize the unknowns in the protocol def
 *
 */
void _o_express_role(Receptor *r,Protocol protocol,Symbol role,Aspect aspect,T *bindings) {
    T *p = _sem_get_def(r->sem,protocol);
    if (!p) raise_error("protocol not found");
    p = _o_unwrap(r->sem,p);  // creates a cloned, uwrapped protocol def.
    _o_resolve(r->sem,p,bindings);
    //@todo convert this search to be repeat Semtex matching on INTERACTION structure...
    T *t;
    int i,c=_t_children(p);
    for(i=1;i<=c;i++) {
        t = _t_child(p,i);
        if (semeq(_sem_get_symbol_structure(r->sem,_t_symbol(t)),INTERACTION)) {
            int j;
            int b = _t_children(t);
            for(j=1;j<=b;j++) {
                T *x = _t_child(t,j);
                if (semeq(_t_symbol(x),EXPECT)) {
                    T *rl = _t_child(x,ExpectRoleIdx);
                    if (semeq(*(Symbol *)_t_surface(rl),role)) {
                        T *pattern = _t_clone(_t_child(x,ExpectPatternIdx));
                        // @todo check pattern for unbound USAGEs

                        T *a = _t_clone(_t_child(x,ExpectActionIdx));
                        if (!bindings && semeq(_t_symbol(a),GOAL)) {
                            raise_error("binding missing for GOAL:%s in %s",_sem_get_name(r->sem,*(SemanticID *)_t_surface(a)),_t2s(r->sem,x));
                        }
                        T *e = __r_build_expectation(protocol,pattern,a,0,0);
                        __r_add_expectation(r,aspect,e);
                    }
                }
            }
        }
    }
    _t_free(p);
}

/** @}*/