/*  =========================================================================
    zdiscgoplugin - Plugin API

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of zdiscgo.                                      
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
    =========================================================================
*/

/*
@header
    zdiscgoplugin - Plugin API
@discuss
@end
*/

#include "zdiscgo_classes.h"

//  Structure of our class

struct _zdiscgoplugin_t {
    void *handle;
    char * (*discover)(go_str, go_str);
};


//  --------------------------------------------------------------------------
//  Create a new zdiscgoplugin

zdiscgoplugin_t *
zdiscgoplugin_new (char *libpath)
{
    zdiscgoplugin_t *self = (zdiscgoplugin_t *) zmalloc (sizeof (zdiscgoplugin_t));
    assert (self);
   
    self->handle = dlopen (libpath, RTLD_NOW);
    if (!self->handle)
        return NULL;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-pedantic"
    self->discover = (char * (*)(go_str, go_str)) dlsym(self->handle, "ZDiscgoDiscoverEndpoints");
    if (!self->discover)
        return NULL;
#pragma GCC diagnostic pop 

    return self;
}

//  --------------------------------------------------------------------------
//  Get endpoint list from a Go library

const char *
zdiscgoplugin_discover_endpoints (zdiscgoplugin_t *self, char *url, char *key) {

    go_str discover_url = {url, strlen (url)};
    go_str discover_key = {key, strlen (key)};
    char *endpoints = self->discover (discover_url, discover_key);
    return endpoints;
}


//  --------------------------------------------------------------------------
//  Destroy the zdiscgoplugin

void
zdiscgoplugin_destroy (zdiscgoplugin_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zdiscgoplugin_t *self = *self_p;
        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------------------------------------------
//  Self test of this class

void
zdiscgoplugin_test (bool verbose)
{
    printf (" * zdiscgoplugin: ");

    //  @selftest
    
    //  Simple create/destroy test
    zdiscgoplugin_t *self = zdiscgoplugin_new ("./go/libmockdiscgo.so");
    assert (self);

    const char *endpoints = zdiscgoplugin_discover_endpoints (self, "url", "key");
    assert (streq ("inproc://url-key", endpoints));
    
    zdiscgoplugin_destroy (&self);

    // Test behavior when a bad lib path is passed
    self = zdiscgoplugin_new ("/does/not/exist");
    assert (!self);

    //  @end
    printf ("OK\n");
}
