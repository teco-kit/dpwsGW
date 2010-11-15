/* WS4D-gSOAP - Implementation of the Devices Profile for Web Services
 * (DPWS) on top of gSOAP
 * Copyright (C) 2007 University of Rostock
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "ws4d_misc.h"
#ifndef WIN32
#include <sys/time.h>
#endif
#include <time.h>

/**
 * Function extracts a string of a speficic language from an array of struct wdp__LocalizedStringType
 *
 * @param string array of structure wdp__LocalizedStringType to
 * extract string from
 * @param size number of elements in array
 * @param lang language to extract
 *
 * @return pointer to extractet string on success, NULL otherwise
 */
const char *
ws4d_locstring_get (struct ws4d_locstring *string, int size, const char *lang)
{
  int i = 0;
  const char *result = NULL;

  /* test parameters */
  ws4d_assert (string && (size > 0) && lang, NULL);

  for (i = 0; i < size; i++)
    {
      if (string[i].lang && !STRCASECMP (string[i].lang, lang))
        {
          result = string[i].string;
          break;
        }
    }

  return result;
}

struct ws4d_locstring *
ws4d_locstring_dup (struct ws4d_locstring *string, int size,
                    ws4d_alloc_list * alist)
{
  struct ws4d_locstring *res;
  int i, err = WS4D_OK;

  /* test parameters */
  ws4d_assert (string && (size > 0) && alist, NULL);

  res = ws4d_malloc_alist (sizeof (struct ws4d_locstring) * size, alist);
  for (i = 0; i < size; i++)
    {
      if (string[i].lang && string[i].string)
        {
          res[i].lang = ws4d_strdup (string[i].lang, alist);
          res[i].string = ws4d_strdup (string[i].string, alist);
        }
      else
        {
          err = WS4D_ERR;
        }
    }

  if (err != WS4D_OK)
    {
      for (; i > 0; i--)
        {
          if (string[i].lang && string[i].string)
            {
              ws4d_free_alist ((void *) res[i].lang);
              ws4d_free_alist ((void *) res[i].string);
            }
        }
    }

  return res;
}

void
ws4d_locstring_free (struct ws4d_locstring *string, int size)
{
  int i;

  ws4d_assert (string && (size > 0),);

  for (i = 0; i < size; i++)
    {
      if (string[i].lang)
        {
          ws4d_free_alist ((void *) string[i].lang);
        }

      if (string[i].string)
        {
          ws4d_free_alist ((void *) string[i].string);
        }
    }
  ws4d_free_alist (string);
}

int
ws4d_dur_to_s (struct ws4d_dur *dur, ws4d_time * s)
{
  if (dur && s)
    {
      *s = ((946080000 * dur->years) +
            (2592000 * dur->months) +
            (86400 * dur->days) + (3600 * dur->hours) +
            (60 * dur->minutes) + dur->seconds);

      return WS4D_OK;
    }
  else
    {
      return WS4D_ERR;
    }
}

int
ws4d_s_to_dur (ws4d_time s, struct ws4d_dur *dur)
{
  if (dur)
    {
      dur->years = s / 946080000;
      s = s % 946080000;
      dur->months = s / 2592000;
      s = s % 2592000;
      dur->days = s / 86400;
      s = s % 86400;
      dur->hours = s / 3600;
      s = s % 3600;
      dur->minutes = s / 60;
      s = s % 60;
      dur->seconds = s;

      return WS4D_OK;
    }
  else
    {
      return WS4D_ERR;
    }
}

int
ws4d_xsddt_to_dur (const char *xsddt, struct ws4d_dur *dur)
{
  char *period = NULL;
  char *dur_time = NULL;

  ws4d_assert (xsddt && dur, WS4D_ERR);

  period = strchr (xsddt, 'P');
  dur_time = strchr (xsddt, 'T');

  if (period)
    {
      int cur = 0, last = 0;

      period++;

      while ((period[cur] != '\0') && (period[cur] != 'T'))
        {
          if (isdigit (period[cur]))
            {
              cur++;
            }
          else if (period[cur] == 'Y')
            {
              period[cur] = '\0';
              dur->years = atoi (period + last);
              cur++;
              last = cur;
            }
          else if (period[cur] == 'M')
            {
              period[cur] = '\0';
              dur->months = atoi (period + last);
              cur++;
              last = cur;
            }
          else if (period[cur] == 'D')
            {
              period[cur] = '\0';
              dur->days = atoi (period + last);
              cur++;
              last = cur;
            }
          else
            {
              /* parse error */
              return WS4D_ERR;
            }
        }
    }
  if (dur_time)
    {
      int cur = 0, last = 0;

      dur_time++;

      while (dur_time[cur] != '\0')
        {
          if (isdigit (dur_time[cur]))
            {
              cur++;
            }
          else if (dur_time[cur] == 'H')
            {
              dur_time[cur] = '\0';
              dur->years = atoi (dur_time + last);
              cur++;
              last = cur;
            }
          else if (dur_time[cur] == 'M')
            {
              dur_time[cur] = '\0';
              dur->months = atoi (dur_time + last);
              cur++;
              last = cur;
            }
          else if (dur_time[cur] == 'S')
            {
              dur_time[cur] = '\0';
              dur->days = atoi (dur_time + last);
              cur++;
              last = cur;
            }
          else
            {
              /* parse error */
              return WS4D_ERR;
            }
        }
    }

  return WS4D_OK;
}

int
ws4d_dur_to_xsddt (struct ws4d_dur *dur, char *buf, int buf_len)
{
  int ret;

  ws4d_assert (dur && buf && (buf_len > 0), WS4D_ERR);

  ret = SNPRINTF (buf, buf_len,
                  "P%dY%dM%dDT%dH%dM%dS", dur->years,
                  dur->months, dur->days, dur->hours, dur->minutes,
                  dur->seconds);

  if (ret > buf_len)
    {
      return WS4D_ERR;
    }
  else
    {
      return WS4D_OK;
    }
}

ws4d_time
ws4d_systime_ms (void)
{
#ifndef WIN32
  struct timeval tv;
  gettimeofday (&tv, (struct timezone *) 0);

  return (ws4d_time) (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
#else
  return (ws4d_time) GetTickCount ();
#endif
}

ws4d_time
ws4d_systime_s (void)
{
#ifndef WIN32
  struct timeval tv;
  gettimeofday (&tv, (struct timezone *) 0);

  return (ws4d_time) tv.tv_sec;
#else
  return (ws4d_time) GetTickCount () / 1000;
#endif
}

/**
 * @addtogroup DpwsXsdList xsd:List and xsd:Array helper functions
 * @ingroup Internals
 *
 * @{
 */

char **
ws4d_alloc_xsdArray (int count, ws4d_alloc_list * alist)
{
  char **result = { NULL };
  int i = 0;

  if (!alist || (count < 1))
    return result;

  result = ws4d_malloc_alist (sizeof (char *) * (count + 1), alist);

  if (result)
    {
      for (i = 0; i < count; i++)
        result[i] = (char *) "";
      result[count] = NULL;
    }

  return result;
}

void
ws4d_free_xsdArray (char **Array)
{
  int count = 0;
  if (Array)
    {
      while (Array[count])
        {
          ws4d_free_alist (Array[count]);
          count++;
        }
      ws4d_free_alist (Array);
    }
}

char **
ws4d_xsdList_to_Array (const char *List, ws4d_alloc_list * alist)
{
  char **noresult = { NULL }, **result = NULL;
  const char *pos1 = NULL, *pos2 = NULL;
  int count = 1, i;

  if (!List || !alist)
    return result;

  while (List[0] == ' ')
    List++;

  pos1 = List;
  while (pos1)
    {
      pos1 = memchr (pos1, ' ', strlen (pos1));
      if (pos1)
        {
          pos1++;
          count++;
        }
    }

  pos1 = List;
  result = ws4d_alloc_xsdArray (count, alist);

  if (result)
    {
      for (i = 0; i < count; i++)
        {
          int n;

          pos2 = memchr (pos1, ' ', strlen (pos1));
          if (!pos2)
            {
              pos2 = pos1 + strlen (pos1);
            }
          result[i] = ws4d_malloc_alist (pos2 - pos1 + 1, alist);
          if (result[i])
            {
              strncpy (result[i], pos1, pos2 - pos1);
              result[i][pos2 - pos1] = '\0';
            }
          else
            {
              return noresult;
            }
          pos1 = pos2;
          for (n = 0; (n < (signed int) strlen (pos1)) && (pos2[0] == ' ');
               n++, pos2++)
            ;
          pos1 = pos2;
        }
      return result;
    }
  else
    {
      return noresult;
    }
}

/** @} */

int
ws4d_xsdArray_match (char **Types, char **TypesToMatch,
                     int (*soap_match_func) (const char *s1, const char *s2))
{
  int matches = 1;

  if (!Types || !TypesToMatch || !soap_match_func)
    return 0;

  while (*TypesToMatch && matches)
    {
      char **LoopTypes;

      matches = 0;
      LoopTypes = Types;

      while (*LoopTypes)
        {
          if (!soap_match_func (*LoopTypes, *TypesToMatch))
            {
              matches = 1;
              break;
            }
          LoopTypes++;
        }
      TypesToMatch++;
    }

  return matches;
}

const char *ws4d_uuid_schema_prefix = "urn:uuid:";
const char *ws4d_uuid_schema_format = "urn:uuid:%s";

static unsigned char
ws4d_generaterandchar ()
{
  char s[10];

  sprintf (s, "%x", rand () % 16);
  return s[0];
}

void
ws4d_uuid_generate_random (char *out)
{
  int i = 0;

  srand ((unsigned int) ws4d_systime_ms ());
  for (i = 0; i < 8; i++)
    out[i] = ws4d_generaterandchar ();
  out[i++] = '-';
  for (i = 9; i < 13; i++)
    out[i] = ws4d_generaterandchar ();
  out[i++] = '-';
  out[i++] = '4';
  out[i++] = ws4d_generaterandchar ();
  out[i++] = ws4d_generaterandchar ();
  out[i++] = ws4d_generaterandchar ();
  out[i++] = '-';
  out[i++] = '8';
  out[i++] = ws4d_generaterandchar ();
  out[i++] = ws4d_generaterandchar ();
  out[i++] = ws4d_generaterandchar ();
  out[i++] = '-';
  for (i = 24; i < 36; i++)
    out[i] = ws4d_generaterandchar ();
  out[i++] = 0;
  return;
}

struct ws4d_qname *
ws4d_qname_alloc (int count, ws4d_alloc_list * alist)
{
  struct ws4d_qname *result = NULL;

  if ((count < 1) || !alist)
    return result;

  result = ws4d_malloc_alist (count * sizeof (struct ws4d_qname), alist);

  if (result)
    {
      WS4D_INIT_LIST (&result->list);
    }

  return result;
}

void
ws4d_qname_free (struct ws4d_qname *qname)
{
  ws4d_assert (qname,);

  if (qname->name)
    {
      ws4d_free_alist (qname->name);
    }

  if (qname->ns)
    {
      ws4d_free_alist (qname->ns);
    }

  if (qname->prefix)
    {
      ws4d_free_alist (qname->prefix);
    }

  ws4d_free_alist (qname);

  return;
}

static int
ws4d_qname_parse (char *string, struct ws4d_qname *qname,
                  ws4d_alloc_list * alist)
{
  char *id;

  ws4d_assert (string && qname && alist, WS4D_ERR);

  if (string[0] == '\"')
    {
      id = strstr (string, "\":");
    }
  else
    {
      id = strchr (string, ':');
    }

  if (id)
    {
      char *prefix;

      prefix = ws4d_malloc_alist (id - string + 1, alist);
      if (!prefix)
        return WS4D_EOM;

      if (string[0] == '\"')
        {
          memcpy (prefix, string + 1, id - string - 1);
          prefix[id - string] = '\0';
        }
      else
        {
          memcpy (prefix, string, id - string);
          prefix[id - string] = '\0';
        }

      if (string[0] == '\"')
        {
          qname->ns = prefix;
          id += 2;
        }
      else
        {
          qname->prefix = prefix;
          id += 1;
        }

      qname->name = ws4d_strdup (id, alist);

      return WS4D_OK;
    }

  return WS4D_ERR;
}

char *
ws4d_qname_tostring2 (struct ws4d_qname *qname, char *buffer, size_t size)
{
  ws4d_assert (qname && buffer && (size > 0), NULL);

  if ((strlen (qname->ns) + (strlen (qname->name)) + 4) > size)
    {
      return NULL;
    }
  else
    {
      memset (buffer, 0, size);
      strcat (buffer, "\"");
      strcat (buffer, qname->ns);
      strcat (buffer, "\"");
      strcat (buffer, ":");
      strcat (buffer, qname->name);

      return buffer;
    }

  return NULL;
}

struct ws4d_qname *
ws4d_qname_dup (struct ws4d_qname *src, ws4d_alloc_list * alist)
{
  struct ws4d_qname *result = NULL;

  if (!src || !alist)
    return result;

  result = ws4d_qname_alloc (1, alist);
  if (!result)
    return result;

  if (src->name)
    {
      result->name = ws4d_strdup (src->name, alist);
    }
  else
    {
      result->name = NULL;
    }

  if (src->ns)
    {
      result->ns = ws4d_strdup (src->ns, alist);
    }
  else
    {
      result->ns = NULL;
    }

  if (src->prefix)
    {
      result->prefix = ws4d_strdup (src->prefix, alist);
    }
  else
    {
      result->prefix = NULL;
    }

  return result;
}

int
ws4d_qnamelist_init (ws4d_qnamelist * head)
{
  if (!head)
    return WS4D_ERR;

  WS4D_INIT_LIST (head);

  return WS4D_OK;
}

int
ws4d_qnamelist_done (ws4d_qnamelist * head)
{
  if (!head)
    return WS4D_ERR;

  WS4D_INIT_LIST (head);

  return WS4D_OK;
}

int
ws4d_qnamelist_add (struct ws4d_qname *qname, ws4d_qnamelist * head)
{
  if (!qname || !head)
    return WS4D_ERR;

  ws4d_list_add_tail (&qname->list, head);

  return WS4D_OK;
}

int
ws4d_qnamelist_addstring (const char *string, ws4d_qnamelist * head,
                          ws4d_alloc_list * alist)
{
  char **qname_list = NULL, **temp = NULL;

  if (!string || !head || !alist)
    return WS4D_ERR;

  qname_list = ws4d_xsdList_to_Array (string, alist);
  temp = qname_list;

  while (*temp)
    {
      if (*temp)
        {
          struct ws4d_qname *type;

          type = ws4d_qname_alloc (1, alist);
          if (ws4d_qname_parse (*temp, type, alist) == WS4D_OK)
            {
              ws4d_qnamelist_add (type, head);
            }
          else
            {
              ws4d_qname_free (type);
            }

        }
      temp++;
    }

  ws4d_free_xsdArray (qname_list);

  return WS4D_OK;
}

int
ws4d_qnamelist_copy (ws4d_qnamelist * src, ws4d_qnamelist * dst,
                     ws4d_alloc_list * alist)
{
  register struct ws4d_qname *qname, *next;

  ws4d_assert (src && dst && alist, WS4D_ERR);

  ws4d_list_foreach (qname, next, src, struct ws4d_qname, list)
  {
    if (qname && qname->ns && qname->name)
      {
        struct ws4d_qname *qname_dup = NULL;
        int res = 0;

        qname_dup = ws4d_qname_dup (qname, alist);
        if (!qname_dup)
          return WS4D_ERR;

        res = ws4d_qnamelist_add (qname_dup, dst);
        if (res != WS4D_OK)
          return res;
      }
  }

  return WS4D_OK;
}

int
ws4d_qnamelist_remove (struct ws4d_qname *qname)
{
  if (!qname)
    return WS4D_ERR;

  ws4d_list_del (&qname->list);

  return WS4D_OK;
}

int
ws4d_qnamelist_clear (ws4d_qnamelist * head)
{
  register struct ws4d_qname *qname, *next;

  if (!head)
    return WS4D_ERR;

  ws4d_list_foreach (qname, next, head, struct ws4d_qname, list)
  {
    ws4d_qnamelist_remove (qname);
    ws4d_free_alist (qname);
  }

  return WS4D_OK;
}

int
ws4d_qnamelist_empty (ws4d_qnamelist * head)
{
  return ws4d_list_empty (head);
}

char *
ws4d_qnamelist_tostring (ws4d_qnamelist * head, ws4d_alloc_list * alist)
{
  register struct ws4d_qname *qname, *next;
  char *result = NULL;
  int qname_count = 0, length = 0, i = 0;

  if (!head || !alist)
    return result;

  ws4d_list_foreach (qname, next, head, struct ws4d_qname, list)
  {
    if (qname && qname->ns && qname->name)
      {
        qname_count++;
        length++;
        length += strlen (qname->ns) + 4;
        length += strlen (qname->name);
      }
  }

  if (qname_count < 1)
    return NULL;

  result = ws4d_malloc_alist (length + 1, alist);
  result[0] = '\0';

  i = 0;
  ws4d_list_foreach (qname, next, head, struct ws4d_qname, list)
  {
    if (qname && qname->ns && qname->name)
      {
        strcat (result, "\"");
        strcat (result, qname->ns);
        strcat (result, "\"");
        strcat (result, ":");
        strcat (result, qname->name);
        if ((qname_count > 1) && (i < (qname_count - 1)))
          {
            strcat (result, " ");
          }
        i++;
      }
  }

  return result;
}

char **
ws4d_qnamelist_toarray (ws4d_qnamelist * head, ws4d_alloc_list * alist)
{
  char **noresult = { NULL }, **result = NULL;
  register struct ws4d_qname *qname, *next;
  int qname_count = 0, i = 0;

  if (!head || !alist)
    return noresult;

  ws4d_list_foreach (qname, next, head, struct ws4d_qname, list)
  {
    if (qname && qname->ns && qname->name)
      {
        qname_count++;
      }
  }

  if (qname_count < 1)
    return noresult;

  result = ws4d_alloc_xsdArray (qname_count + 1, alist);

  if (result)
    {
      i = 0;
      ws4d_list_foreach (qname, next, head, struct ws4d_qname, list)
      {
        if (qname && qname->ns && qname->name)
          {
            int length = 0;

            length += strlen (qname->ns);
            length += strlen (qname->name);
            length += 4;

            result[i] = ws4d_malloc_alist (length + 1, alist);

            strcat (result[i], "\"");
            strcat (result[i], qname->ns);
            strcat (result[i], "\":");
            strcat (result[i], qname->name);

            i++;
          }
      }
    }

  result[qname_count] = NULL;

  return result;
}

#ifndef HAVE_INET_PTON_H
int
ws4d_inet_pton (int af, const char *src, void *dst)
{
#ifdef WIN32
  unsigned long *in4 = NULL;
#else
  struct in_addr *in4 = NULL;
#endif

  ws4d_assert (src && dst, -1);

  switch (af)
    {
    case AF_INET:
      in4 = dst;
      *in4 = inet_addr (src);
      return *in4 != INADDR_NONE;
      break;
    default:
      return -1;
    }
}
#endif

#ifndef HAVE_INET_NTOP_H
const char *
ws4d_inet_ntop (int af, const void *src, char *dst, size_t size)
{
  char *result;
#ifdef WIN32
  struct in_addr *in4 = (void *) src;
#else
  in_addr_t *in4 = src;
#endif

  ws4d_assert (src && dst && (size > 0), NULL);

  switch (af)
    {
    case AF_INET:
      result = inet_ntoa (*in4);
      if (result != NULL)
        {
          if (strlen (result) < size)
            {
              strncpy (dst, result, size);
              return dst;
            }
        }
      return NULL;
      break;
    default:
      return NULL;
    }
}
#endif
