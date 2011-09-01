/* embedwsdl - embeds wsdls into c files
 * Copyright (C) 2007  University of Rostock
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#  define SNPRINTF _snprintf
#  pragma warning(disable : 4996)       /* Remove snprintf Secure Warnings */
#else
#define SNPRINTF snprintf
#endif

int
main (int argc, char *argv[])
{
  char **files = NULL;
  int file_count = 0;
  char *dest_path = NULL;
  const char *prefix = "dpws";

  if (argc < 3)
    {
      perror ("Too few Files");
      exit (1);
    }
  else
    {
      files = malloc (argc * sizeof (char *));
      if (!files)
        {
          perror ("Cannot allocate memory");
          exit (1);
        }
      memset (files, 0, argc * sizeof (char *));


      /* parsing command line options */
      while (argc > 1)
        {
          if (argv[1][0] == '-')
            {
              char *option = &argv[1][1];
              switch (option[0])
                {
                case 'd':
                  if (strlen (option) > 2)
                    {
                      ++option;
                      dest_path = option;
                    }
                  else
                    {
                      --argc;
                      ++argv;
                      dest_path = argv[1];
                    }
                  break;
                case 'p':
                  if (strlen (option) > 2)
                    {
                      ++option;
                      prefix = option;
                    }
                  else
                    {
                      --argc;
                      ++argv;
                      prefix = argv[1];
                    }
                  break;
                default:
                  fprintf (stderr, "\nembedwsdl: Bad option %s\n", argv[1]);
                }
            }
          else
            {
              files[file_count] = argv[1];
              file_count++;
            }
          --argc;
          ++argv;
        }
    }

  if (file_count > 0)
    {
      char *wsdl_fname = NULL, *header_fname = NULL;
      int wsdl_fname_len = 0, i, c, c2, doprint = 1;
      FILE *wsdl;
      FILE *cfile;

      wsdl_fname_len = strlen (dest_path);
      wsdl_fname_len += strlen (prefix);
      wsdl_fname_len += strlen ("/_wsdl.c") + 1;

      wsdl_fname = malloc (wsdl_fname_len);
      memset (wsdl_fname, 0, wsdl_fname_len);

      SNPRINTF (wsdl_fname, wsdl_fname_len, "%s/%s_wsdl.c", dest_path,
                prefix);

      header_fname = malloc (wsdl_fname_len);
      memset (header_fname, 0, wsdl_fname_len);

      SNPRINTF (header_fname, wsdl_fname_len, "%s/%s_wsdl.h", dest_path,
                prefix);

      if ((cfile = fopen (header_fname, "w+")) == NULL)
        {
          perror ("Cannot open wsdl.h for writing");
          exit (1);
        }

      fprintf (cfile, "#ifndef %s_WSDL_H\n", prefix);
      fprintf (cfile, "#define %s_METADATA_H\n", prefix);

      fprintf (cfile, "#ifdef __cplusplus\n");
      fprintf (cfile, "extern \"C\" {\n");
      fprintf (cfile, "#endif\n");

      fprintf (cfile, "\nint %s_set_wsdl(struct dpws_s *device);\n\n",
               prefix);

      fprintf (cfile, "#ifdef __cplusplus\n");
      fprintf (cfile, "}\n");
      fprintf (cfile, "#endif\n");

      fprintf (cfile, "#endif /* %s_METADATA_H */\n", prefix);

      fclose (cfile);
      free (header_fname);

      if ((cfile = fopen (wsdl_fname, "w+")) == NULL)
        {
          perror ("Cannot open wsdl.c for writing");
          exit (1);
        }

      fprintf (cfile, "#include \"dpwsH.h\"\n");
      fprintf (cfile, "#include \"stddpws.h\"\n");
      fprintf (cfile, "#include <stdio.h>\n");
      fprintf (cfile, "#include \"%s_wsdl.h\"\n", prefix);

      fprintf (cfile, "int %s_set_wsdl(struct dpws_s *device)\n{\n", prefix);

      fprintf (cfile, "\n\tint old_index = 0, array_index = 0;");

      fprintf (cfile, "\n\tstruct wsdl_s *wsdls = NULL;");

      fprintf (cfile, "\n\tif(device->wsdls)");
      fprintf (cfile, "\n\t{");
      fprintf (cfile,
               "\n\t\tfor(old_index = 0; device->wsdls[old_index].name != NULL; old_index++);");
      fprintf (cfile, "\n\t}");
      fprintf (cfile,
               "\n\twsdls = ws4d_malloc_alist((1 + old_index + %d) * sizeof(struct wsdl_s), &device->alloc_list);",
               file_count);

      fprintf (cfile, "\n\tif(device->wsdls)");
      fprintf (cfile, "\n\t{");
      fprintf (cfile,
               "\n\t\tfor(array_index = 0; array_index < old_index; array_index ++)");
      fprintf (cfile, "\n\t\t{");
      fprintf (cfile,
               "\n\t\t\twsdls[array_index].name = device->wsdls[array_index].name;");
      fprintf (cfile,
               "\n\t\t\twsdls[array_index].content = device->wsdls[array_index].content;");
      fprintf (cfile, "\n\t\t}");
      fprintf (cfile, "\n\t}");

      for (i = 0; i < file_count; i++)
        {

          if ((wsdl = fopen (files[i], "r")) == NULL)
            {
              perror ("Cannot open file for reading");
              exit (1);
            }
          fprintf (cfile, "\n\twsdls[array_index].name = \"%s\";\n",
                   files[i]);
          fprintf (cfile, "\n\twsdls[array_index].content = \"");

          while ((c = getc (wsdl)) != EOF)
            {
              switch (c)
                {
                case '"':
                  if (doprint)
                    fprintf (cfile, "\\\"");
                  break;
                case '\n':
                  if (doprint)
                    fprintf (cfile, "\\n\"\n\t\"");
                  break;
                case '<':
                  c2 = getc (wsdl);
                  if (c2 != EOF && c2 != '?')
                    {
                      putc (c, cfile);
                      putc (c2, cfile);
                    }
                  else
                    {
                      doprint = 0;
                    }
                  break;
                case '?':
                  c2 = getc (wsdl);
                  if (c2 != EOF && c2 != '>')
                    {
                      if (doprint)
                        {
                          putc (c, cfile);
                          putc (c2, cfile);
                        }
                    }
                  else
                    {
                      doprint = 1;
                    }
                  break;
                default:
                  if (doprint)
                    putc (c, cfile);
                }
            }

          fprintf (cfile, "\";");
          fprintf (cfile, "\n\tarray_index++;");

          fclose (wsdl);
        }

      fprintf (cfile, "\n\twsdls[array_index].name = NULL;\n");
      fprintf (cfile, "\n\twsdls[array_index].content = NULL;\n");

      fprintf (cfile, "\n\tif (device->wsdls)");
      fprintf (cfile, "\n\t{");
      fprintf (cfile, "\n\t\tws4d_free_alist(device->wsdls);");
      fprintf (cfile, "\n\t}");
      fprintf (cfile, "\n\tdevice->wsdls = wsdls;");

      fprintf (cfile, "\n\treturn SOAP_OK;\n}\n");

      free (files);
      free (wsdl_fname);

      fclose (cfile);
    }

  exit (0);
}
