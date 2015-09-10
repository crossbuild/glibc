/* The tunable framework.  See the README to know how to use the tunable in
   a glibc module.

   Copyright (C) 2015 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

typedef void (*tunable_setter_t) (const char *);

/* A tunable.  */
struct _tunable
{
  const char *name;
  const char *val;
  tunable_setter_t set;
  bool initialized;
};

typedef struct _tunable tunable_t;

#include "tunable-list.h"

struct compat_tunable_env
{
  tunable_id_t id;
  const char *env;
};

extern void compat_tunables_init_envvars (struct compat_tunable_env *, int);
extern void tunable_register (tunable_id_t, tunable_setter_t);

#define TUNABLE_REGISTER(ns,id,set) \
  TUNABLE_REGISTER_FULL (TOP_NAMESPACE, ns, id, set)

#define TUNABLE_REGISTER_FULL(tns,ns,id,set) \
  tunable_register (TUNABLE_ENUM_NAME (tns, ns, id), (set))

#define ADD_COMPAT_TUNABLE_ENV(__id,__env) \
({									      \
  envvars[envvars_cnt].id = TUNABLE_ENUM_NAME (TOP_NAMESPACE,		      \
					       TUNABLE_NAMESPACE, __id);      \
  envvars[envvars_cnt++].env = (__env);					      \
})

/* Start registering tunables in the current namespace.  */
#define COMPAT_TUNABLES_NAMESPACE_BEGIN(size) \
  {									      \
    struct compat_tunable_env envvars[size];				      \
    int envvars_cnt = 0;

/* Register a tunable.  This macro validates that the call is OK and then calls
   tunable_init to do the real work of adding the tunable and setting its value
   based on its environment variable(s).  */
#define COMPAT_TUNABLE_REGISTER(id,env,set) \
({									      \
  assert (envvars_cnt < (sizeof (envvars)				      \
		      / sizeof (struct compat_tunable_env)));		      \
  if (!__libc_enable_secure)						      \
    ADD_COMPAT_TUNABLE_ENV(id, env);					      \
  TUNABLE_REGISTER (TUNABLE_NAMESPACE, id, set);			      \
})

/* Does exactly the same thing as TUNABLE_REGISTER, except that it allows the
   tunable to look for environment variable values even for setuid binaries.
   This is a separate macro and not just another parameter in TUNABLE_REGISTER
   to avoid accidentally setting a secure flag where it is not required.  */
#define COMPAT_TUNABLE_REGISTER_SECURE(id,env,set) \
({									      \
  assert (envvars_cnt < (sizeof (envvars)			      \
		      / sizeof (struct compat_tunable_env)));		      \
  ADD_COMPAT_TUNABLE_ENV(id, env);					      \
  TUNABLE_REGISTER (TUNABLE_NAMESPACE, id, set);			      \
})

/* Initialize tunables in the namespace.  */
#define COMPAT_TUNABLES_NAMESPACE_INIT() \
    compat_tunables_init_envvars (envvars, envvars_cnt);   \
  }
