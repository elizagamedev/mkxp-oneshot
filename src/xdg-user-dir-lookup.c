/*
  This file is not licenced under the GPL like the rest of the code.
  Its is under the MIT license, to encourage reuse by cut-and-paste.

  Copyright (c) 2007 Red Hat, Inc.

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation files
  (the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge,
  publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions: 

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software. 

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/**
 * xdg_user_dir_lookup:
 * @type: a string specifying the type of directory
 * @returns: a newly allocated absolute pathname
 *
 * Looks up a XDG user directory of the specified type.
 * Example of types are "DESKTOP" and "DOWNLOAD".
 *
 * The return value is always != NULL (unless out of memory),
 * and if a directory
 * for the type is not specified by the user the default
 * is the home directory. Except for DESKTOP which defaults
 * to ~/Desktop.
 *
 * The return value is newly allocated and must be freed with
 * free().
 **/
static char *
xdg_user_dir_lookup (const char *type)
{
  char *dir, *home_dir, *user_dir;
	  
  dir = xdg_user_dir_lookup_with_fallback (type, NULL);
  if (dir != NULL)
    return dir;
  
  home_dir = getenv ("HOME");
  
  if (home_dir == NULL)
    return strdup ("/tmp");
  
  /* Special case desktop for historical compatibility */
  if (strcmp (type, "DESKTOP") == 0)
    {
      user_dir = (char*) malloc (strlen (home_dir) + strlen ("/Desktop") + 1);
      if (user_dir == NULL)
        return NULL;

      strcpy (user_dir, home_dir);
      strcat (user_dir, "/Desktop");
      return user_dir;
    }
  
  return strdup (home_dir);
}

#ifdef STANDALONE_XDG_USER_DIR_LOOKUP
int
main (int argc, char *argv[])
{
  if (argc != 2)
    {
      fprintf (stderr, "Usage %s <dir-type>\n", argv[0]);
      exit (1);
    }
  
  printf ("%s\n", xdg_user_dir_lookup (argv[1]));
  return 0;
}
#endif
