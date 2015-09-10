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

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/param.h>
#include <atomic.h>

extern char **__environ;
static int initialized = 0;

#define TUNABLES_INTERNAL 1
#include "tunables.h"

/* The string is a space separated list of name=value for each tunable.  */
static void
parse_tunable_string (const char *str)
{
  char *input = strdup (str);
  char *in = input;

  while (input != NULL)
    {
      char *end = strchr (input, ' ');

      if (end != NULL)
	*end++ = '\0';

      char *equals = strchr (input, '=');
      if (equals != NULL)
	{
	  *equals++ = '\0';
	  for (int i = 0; i < sizeof (tunable_list) / sizeof (tunable_t); i++)
	    {
	      if (strcmp (input, tunable_list[i].name) == 0)
		{
		  tunable_list[i].val = strdup (equals);
		  break;
		}
	    }
	}

      input = end;
    }

  free (in);
}

static void
tunables_init (char **env)
{
  char **envp = env;
  while (envp != NULL && *envp != NULL)
    {
      char *envline = *envp;
      int len = 0;

      while (envline[len] != '\0' && envline[len] != '=')
	len++;

      /* Just the name and no value.  */
      if (envline[len] == '\0')
	continue;

      const char *name = "GNU_LIBC_TUNABLES";

      if (memcmp (envline, name, MIN(len, strlen (name))) == 0)
	{
	  parse_tunable_string (&envline[len + 1]);
	  break;
	}

      envp++;
    }
}

/* Initialize all tunables in the namespace group specified by ID.  */
void
compat_tunables_init_envvars (struct compat_tunable_env *envvars, int count)
{
  if (atomic_load_acquire (&initialized) == 0)
    {
      tunables_init (__environ);
      atomic_store_release (&initialized, 1);
    }

  /* Traverse through the environment to find environment variables we may need
     to set.  */
  char **envp = __environ;
  while (envp != NULL && *envp != NULL)
    {
      char *envline = *envp;
      int len = 0;

      while (envline[len] != '\0' && envline[len] != '=')
	len++;

      /* Just the name and no value.  */
      if (envline[len] == '\0')
	continue;

      int init_count = 0;
      for (int i = 0; i < count; i++)
	{
	  tunable_id_t t = envvars[i].id;
	  tunable_t *cur = &tunable_list[t];

	  /* Skip over tunables that have already been initialized.  */
	  if (cur->initialized)
	    {
	      init_count++;
	      continue;
	    }

	  const char *name = envvars[i].env;

	  /* We have a match.  Initialize and move on to the next line.  */
	  if (memcmp (envline, name, MIN(len, strlen (name))) == 0)
	    {
	      cur->val = strdup (&envline[len + 1]);
	      cur->set (cur->val);
	      cur->initialized = true;
	      break;
	    }
	}

      /* All of the tunable envvars have been initialized.  */
      if (count == init_count)
	break;

      envp++;
    }
}

/* Initialize a tunable and set its value.  */
void
tunable_register (tunable_id_t id, tunable_setter_t set_func)
{
  if (atomic_load_acquire (&initialized) == 0)
    {
      tunables_init (__environ);
      atomic_store_release (&initialized, 1);
    }

  tunable_t *cur = &tunable_list[id];

  cur->set = set_func;
  if (cur->val != NULL)
    {
      set_func (cur->val);
      cur->initialized = true;
    }
}
