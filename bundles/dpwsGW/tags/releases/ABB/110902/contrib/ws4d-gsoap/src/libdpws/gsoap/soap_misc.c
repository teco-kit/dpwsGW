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

#include "stdsoap2.h"

#include <stdarg.h>

#include "dpwsH.h"
#include "soap_misc.h"

/**
 * @addtogroup Internals Internals
 *
 * @{
 */

#include "ws4d_localizedstring.h"

struct wsdp__LocalizedStringType *
ws4d_locstring_tosoap (struct ws4d_locstring *string,
                       int size, ws4d_alloc_list * alist)
{
  struct wsdp__LocalizedStringType *res;
  int i, err = WS4D_OK;

  /* test parameters */
  ws4d_assert (string && (size > 0) && alist, NULL);

  res =
    ws4d_malloc_alist (sizeof (struct wsdp__LocalizedStringType) * size,
                       alist);
  for (i = 0; i < size; i++)
    {
      if (string[i].lang && string[i].string)
        {
          res[i].xml__lang = ws4d_strdup (string[i].lang, alist);
          res[i].__item = ws4d_strdup (string[i].string, alist);
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
              ws4d_free_alist (res[i].__item);
              ws4d_free_alist (res[i].xml__lang);
            }
        }
    }

  return res;
}

struct ws4d_locstring *
soap_locstring_tows4d (struct wsdp__LocalizedStringType *string,
                       int size, ws4d_alloc_list * alist)
{
  struct ws4d_locstring *res;
  int i, err = WS4D_OK;

  /* test parameters */
  ws4d_assert (string && (size > 0) && alist, NULL);

  res = ws4d_malloc_alist (sizeof (struct ws4d_locstring) * size, alist);
  for (i = 0; i < size; i++)
    {
      if (string[i].__item)
        {
          if (string[i].xml__lang)
            {
              res[i].lang = ws4d_strdup (string[i].xml__lang, alist);
            }
          res[i].string = ws4d_strdup (string[i].__item, alist);
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
          if (string[i].__item)
            {
              char *loc_string = (char *) res[i].string;

              if (string[i].xml__lang)
                {
                  char *loc_lang = (char *) res[i].lang;
                  ws4d_free_alist (loc_lang);
                }

              ws4d_free_alist (loc_string);
            }
        }
    }

  return res;
}

char *
soap_locstring_get (struct wsdp__LocalizedStringType *string,
                    int size, const char *lang)
{
  int i = 0;
  char *result = NULL;

  /* test parameters */
  ws4d_assert (string && (size > 0) && lang, NULL);

  for (i = 0; i < size; i++)
    {
      if (string[i].xml__lang && !STRCASECMP (string[i].xml__lang, lang))
        {
          result = string[i].__item;
          break;
        }
    }

  return result;
}

void
soap_locstring_free (struct wsdp__LocalizedStringType *string, int size)
{
  int i;

  ws4d_assert (string && (size > 0),);

  for (i = 0; i < size; i++)
    {
      if (string[i].xml__lang)
        {
          ws4d_free_alist (string[i].xml__lang);
        }

      if (string[i].__item)
        {
          ws4d_free_alist (string[i].__item);
        }
    }
  ws4d_free_alist (string);
}

static int
soap_ser_send (struct soap *soap, const char *buf, size_t len)
{
  if (soap->user)
    {
      strncat (soap->user, buf, len);
    }
  return SOAP_OK;
}

char *
soap_elem_to_str (void *ptr, char *tag, int type)
{
  struct soap tmp;
  soap_init (&tmp);
  soap_omode (&tmp, SOAP_XML_CANONICAL);
  tmp.fsend = soap_ser_send;
  soap_begin_count (&tmp);
  soap_putelement (&tmp, ptr, tag, -1, type);
  soap_end_count (&tmp);
  tmp.user = malloc (tmp.count + 1);
  memset (tmp.user, 0, tmp.count + 1);
  soap_begin_send (&tmp);
  soap_putelement (&tmp, ptr, tag, -1, type);
  soap_end_send (&tmp);
  return tmp.user;
}

struct soap_recv_buf
{
  int pos;
  size_t size;
  char *buf;
};

static size_t
soap_deser_recv (struct soap *soap, char *buf, size_t len)
{
  if (soap->user)
    {
      struct soap_recv_buf *recv_buf = soap->user;
      int ret = 0;
      if ((unsigned int) recv_buf->pos >= recv_buf->size)
        return 0;
      if (len >= (recv_buf->size - recv_buf->pos))
        {
          ret = recv_buf->size - recv_buf->pos;
        }
      else
        {
          ret = len;
        }

      memcpy (buf, recv_buf->buf + recv_buf->pos, ret);
      recv_buf->pos += ret;
      return ret;
    }
  return 0;
}

int
soap_begin_deser (struct soap *soap, char *str, size_t str_len,
                  struct Namespace *ns)
{
  struct soap_recv_buf *buf;
  int i;
  soap->frecv = soap_deser_recv;
  soap_begin_recv (soap);
  for (i = 0; ns[i].id; i++)
    {
      soap_push_namespace (soap, ns[i].id, ns[i].ns);
    }
  soap->level++;

  buf = (struct soap_recv_buf *) soap_malloc (soap,
                                              sizeof (struct soap_recv_buf));
  buf->pos = 0;
  buf->size = str_len + 1;
  buf->buf = str;
  soap->user = buf;
  return SOAP_OK;
}

int
soap_end_deser (struct soap *soap)
{
  soap->level--;
  soap_pop_namespace (soap);
  soap_end_recv (soap);
  return SOAP_OK;
}

int
soap_header_new (struct soap *soap, size_t size)
{
  if (!soap || (size < 1))
    return SOAP_ERR;

  soap->header = (struct SOAP_ENV__Header *) soap_malloc (soap, size);

  if (soap->header)
    {
      memset (soap->header, 0, size);
      return SOAP_OK;
    }
  else
    {
      return soap->error;
    }
}

struct Namespace *
soap_extend_namespaces (struct Namespace *orig_namespaces,
                        struct Namespace *extns, ws4d_alloc_list * alist)
{
  register struct Namespace *entry = NULL;
  struct Namespace *result = NULL, *tmp_result = NULL;
  int old_size = 0, ext_size = 0, new_size = 0, count = 0;

  if (!extns || !orig_namespaces || !alist)
    return result;

  for (entry = orig_namespaces; entry && entry->id; entry++)
    {
      old_size++;
    }

  for (entry = extns; entry && entry->id; entry++)
    {
      ext_size++;
    }

  tmp_result = malloc (sizeof (struct Namespace) * (ext_size));
  memset (tmp_result, 0, sizeof (struct Namespace) * (ext_size));

  new_size = old_size;

  for (entry = extns; entry && entry->id && entry->ns; entry++)
    {
      int duplicate = 0;
      register struct Namespace *old_entry = NULL;

      for (old_entry = orig_namespaces; old_entry && old_entry->id
           && old_entry->ns; old_entry++)
        {
          if (!strcmp (old_entry->ns, entry->ns) && !strcmp (old_entry->id,
                                                             entry->id))
            {
              duplicate = 1;
              break;
            }
        }

      if (!duplicate)
        {
          tmp_result[new_size - old_size].id = entry->id;
          tmp_result[new_size - old_size].ns = entry->ns;
          new_size++;
        }
    }

  if (new_size > old_size)
    {
      result = ws4d_malloc_alist (sizeof (struct Namespace) * (new_size + 1),
                                  alist);
    }
  else
    {
      if (tmp_result)
        free (tmp_result);

      return orig_namespaces;
    }

  memset (result, 0, sizeof (struct Namespace) * (new_size + 1));
  memcpy (result, orig_namespaces,
          sizeof (struct Namespace) * (old_size + 1));

  entry = result;
  entry += old_size;

  for (count = 0; count < (new_size - old_size); count++, entry++)
    {
      entry->id = tmp_result[count].id;
      entry->ns = tmp_result[count].ns;
    }

  if (tmp_result)
    free (tmp_result);

  return result;
}

#ifndef WITH_NOIO
void *
soap_getpeer (struct soap *soap)
{
  return &soap->peer;
}

size_t
soap_getpeerlen (struct soap * soap)
{
  return soap->peerlen;
}

#endif

struct soap *
soap_maccept (ws4d_time timeout, int count, struct soap **soap_handles)
{
  struct timeval timeout_t;
  register int i, r = -1;
  SOAP_SOCKET bigfd = SOAP_INVALID_SOCKET;
  fd_set fd;
  struct soap *result = NULL;

  FD_ZERO (&fd);

  if (!soap_handles)
    return result;

  for (i = 0; i < count; i++)
    {
      if (soap_valid_socket (soap_handles[i]->master))
        {
          FD_SET ((SOAP_SOCKET) soap_handles[i]->master, &fd);
          if ((SOAP_SOCKET) soap_handles[i]->master > bigfd)
            bigfd = soap_handles[i]->master;
        }
      else
        {
          printf ("register soap handle number %d first\n", i);
          goto exit;
        }
    }

  if (timeout != DPWS_SYNC)
    {
      timeout_t.tv_sec = timeout / 1000;
      timeout_t.tv_usec = (timeout % 1000) * 1000;
    }

  while (r < 0)
    {
      r = select (bigfd + 1, &fd, NULL, NULL, &timeout_t);
      if (r > 0)
        {
          for (i = 0; i < count; i++)
            {
              if (soap_valid_socket (soap_handles[i]->master))
                {
                  if (FD_ISSET ((SOAP_SOCKET) soap_handles[i]->master, &fd))
                    {
                      result = soap_handles[i];
                      goto exit;
                    }
                }
            }
        }
    }

exit:

  if (result)
    soap_accept (result);

  return result;
}

int
soap_mserve (struct soap *soap, int count,
             int (*serve_requests[])(struct soap * soap))
{
  unsigned int k = soap->max_keep_alive;
  int i = 0, err = SOAP_ERR;

  if (!soap || !serve_requests || count < 1)
    return SOAP_ERR;

  do
    {

      soap_begin (soap);

      if (!--k)
        soap->keep_alive = 0;

      if (soap_begin_recv (soap))
        {
          if (soap->error < SOAP_STOP)
            {
              return soap_send_fault (soap);
            }
          soap_closesock (soap);

          continue;
        }

      if (soap_envelope_begin_in (soap) || soap_recv_header (soap)
          || soap_body_begin_in (soap))
        {
          if (soap->mode & SOAP_IO_UDP)
            {
              return soap->error;
            }
          else
            {
              return soap_send_fault (soap);
            }
        }

      if (!soap->action)
        {
          printf
            ("Warning: Incoming message on handle 0x%p has no soap action!\n",
             soap);
        }

      i = 0;
      err = SOAP_NO_METHOD;
      while ((i < count) && (err == SOAP_NO_METHOD))
        {
          err = serve_requests[i] (soap);
          i++;
        }

      if ((err != SOAP_OK) && (err != SOAP_STOP))
        return soap_send_fault (soap);

      if (soap->fserveloop && soap->fserveloop (soap))
        {
          return soap_send_fault (soap);
        }

    }
  while (soap->keep_alive);

  return SOAP_OK;
}

char *
soap_gen_prefix (int *num, ws4d_alloc_list * alist,
                 struct Namespace *prefix_namespaces)
{
  char prefix[255];
  int collision = 0;

  if (!prefix_namespaces)
    return NULL;

  do
    {
      struct Namespace *cur_ns = prefix_namespaces;
      collision = 0;

      *num += 1;
      SNPRINTF (prefix, 255, "n%d", *num);

      while (cur_ns->ns)
        {
          if (cur_ns->id)
            {
              if (!strcmp (cur_ns->id, prefix))
                {
                  collision = 1;
                }
            }
          cur_ns++;
        }

    }
  while (collision == 1);

  return ws4d_strdup (prefix, alist);
}

struct Namespace *
soap_qnamelist_namespaces (ws4d_qnamelist * head,
                           ws4d_alloc_list * alist,
                           struct Namespace *qname_namespaces)
{
  register struct ws4d_qname *type, *next;
  register struct Namespace *result = NULL, *entry = NULL, *ns = NULL;
  int type_count = 0, prefix_count = 0;

  if (!head || !alist)
    return result;

  ws4d_list_foreach (type, next, head, struct ws4d_qname, list)
  {
    if (type && type->ns)
      {
        int found = 0;
        for (ns = qname_namespaces; ns && ns->id; ns++)
          {
            if (ns && ns->ns && !strcmp (type->ns, ns->ns))
              {
                found = 1;
                break;
              }
          }
        if (found == 0)
          {
            type_count++;
          }
      }
  }

  result = ws4d_malloc_alist (sizeof (struct Namespace) * (type_count + 1),
                              alist);
  if (!result)
    return result;
  memset (result, 0, sizeof (struct Namespace) * (type_count + 1));

  entry = result;

  ws4d_list_foreach (type, next, head, struct ws4d_qname, list)
  {
    if (type && type->ns)
      {
        int found = 0;
        for (ns = qname_namespaces; ns && ns->id; ns++)
          {
            if (ns && ns->ns && !strcmp (type->ns, ns->ns))
              {
                found = 1;
                break;
              }
          }
        if (found == 0)
          {
            if (type->prefix)
              {
                entry->id = type->prefix;
              }
            else
              {
                /* TODO: should fail somehow if no prefix can be generated */
                entry->id =
                  soap_gen_prefix (&prefix_count, alist, qname_namespaces);
              }
            entry->ns = type->ns;
            entry++;
          }
      }
  }

  return result;
}

int
soap_out_transform_qnames (struct soap *soap, char **outstring,
                           char *const *a)
{
  char **array = NULL;
  char *pos = NULL;
  int count = 0, entry = 0;

  pos = *a;
  count = 1;
  while (pos)
    {
      pos = memchr (pos, ' ', strlen (pos));
      if (pos)
        {
          pos++;
          count++;
        }
    }

  array = soap_malloc (soap, sizeof (char *) * count);
  memset (array, 0, sizeof (char *) * count);

  pos = *a;
  entry = 0;
  while (pos)
    {
      char *newpos = NULL;

      newpos = memchr (pos, ' ', strlen (pos));

      if (*pos && (pos[0] == '\"'))
        {
          char *name = NULL;

          pos++;
          name = strstr (pos, "\":");

          if (name && ((newpos == 0) || (name < newpos)))
            {
              struct Namespace *namespace = soap->local_namespaces;
              int namespace_len = name - pos;

              while (namespace && namespace->ns && strncmp (namespace->ns,
                                                            pos,
                                                            namespace_len))
                {
                  namespace++;
                }

              name++;

              if (namespace && namespace->id)
                {
                  int qname_len = 0;

                  qname_len = strlen (namespace->id);
                  if (newpos)
                    {
                      qname_len += newpos - name;
                    }
                  else
                    {
                      qname_len += strlen (name);
                    }
                  qname_len += 1;

                  array[entry] =
                    soap_malloc (soap, sizeof (char) * qname_len);
                  memset (array[entry], 0, sizeof (char) * qname_len);
                  strcat (array[entry], namespace->id);
                  if (newpos)
                    {
                      strncat (array[entry], name, newpos - name);
                    }
                  else
                    {
                      strcat (array[entry], name);
                    }
                  entry++;
                }
              else
                {
                  soap->error = SOAP_NAMESPACE;
                  return soap->error;
                }
            }
          else
            {
              soap->error = SOAP_NAMESPACE;
              return soap->error;
            }
        }

      if (newpos && (newpos[0] == ' '))
        {
          newpos++;
        }
      pos = newpos;
    }

  if (entry > 0)
    {
      int i = 0;
      int outstring_len = 0;

      for (i = 0; i < entry; i++)
        {
          outstring_len += strlen (array[i]) + 1;
        }

      if (outstring_len > 0)
        {
          *outstring = soap_malloc (soap, sizeof (char) * outstring_len);
          memset (*outstring, 0, sizeof (char) * outstring_len);

          strcpy (*outstring, array[0]);
          for (i = 1; i < entry; i++)
            {
              strcat (*outstring, " ");
              strcat (*outstring, array[i]);
            }
        }
    }

  return SOAP_OK;
}

int
soap_in_expand_qnames (struct soap *soap, char **instring, char **a)
{
  char **array = NULL;
  char *pos = NULL;
  int count = 0, entry = 0, i, result_size;

  pos = *instring;
  count = 1;
  while (pos)
    {
      pos = memchr (pos, ' ', strlen (pos));
      if (pos)
        {
          pos++;
          count++;
        }
    }

  array = soap_malloc (soap, sizeof (char *) * count);
  memset (array, 0, sizeof (char *) * count);

  pos = *instring;
  entry = 0;
  while (pos)
    {
      char *newpos = NULL, *name = NULL;
      struct soap_nlist *namespace = soap->nlist;

      /* replace white spaces with string terminations */
      newpos = memchr (pos, ' ', strlen (pos));
      if (newpos)
        {
          newpos[0] = '\0';
          newpos++;
        }

      if (*pos)
        {

          name = strchr (pos, ':');

          if (name)
            {
              register int prefixlen = name - pos;
              while (namespace && (strncmp (namespace->id, pos, prefixlen)
                                   || namespace->id[prefixlen]))
                {
                  namespace = namespace->next;
                }
              name++;
            }
          else
            {
              while (namespace && *namespace->id)
                namespace = namespace->next;
              name = pos;
            }

          if (namespace && namespace->index >= 0 && soap->local_namespaces
              && soap->local_namespaces[namespace->index].ns)
            {
              array[entry] = (char *) soap_malloc (soap, strlen (name)
                                                   +
                                                   strlen
                                                   (soap->local_namespaces
                                                    [namespace->index].ns) +
                                                   4);
              sprintf (array[entry], "\"%s\":%s",
                       soap->local_namespaces[namespace->index].ns, name);
              entry++;
            }
          else
            {

              if (namespace && namespace->ns)
                {
                  array[entry] = (char *) soap_malloc (soap, strlen (name)
                                                       +
                                                       strlen (namespace->ns)
                                                       + 4);
                  sprintf (array[entry], "\"%s\":%s", namespace->ns, name);
                  entry++;
                }
              else
                {
                  soap->error = SOAP_NAMESPACE;
                  return soap->error;
                }
            }
        }

      pos = newpos;
    }

  result_size = 0;
  for (i = 0; i < entry; i++)
    {
      if (i > 0)
        {
          result_size += strlen (" ");
        }
      result_size += strlen (array[i]);
    }
  result_size++;

  *a = (char *) soap_malloc (soap, sizeof (char) * result_size);
  memset (*a, 0, sizeof (char) * result_size);

  for (i = 0; i < entry; i++)
    {
      if (i > 0)
        {
          strcat (*a, " ");
        }
      strcat (*a, array[i]);
    }

  return SOAP_OK;
}

/************
 * soap based allocator
 * **********/

#define SOAP_ALLOCATORPLUGIN_ID "gSOAP allocator V 1.0"
const char *soap_allocator_plugin_id = SOAP_ALLOCATORPLUGIN_ID;

#define soap_allocator_checkid(allocator) \
  ((allocator)->id && (((allocator)->id == soap_allocator_plugin_id)||(!strcmp((allocator)->id, soap_allocator_plugin_id))))

static void
soap_allocator_fdone (struct ws4d_abs_allocator *allocator)
{
  struct soap *soap;

  ws4d_assert (soap_allocator_checkid (allocator),);
  soap = allocator->data;

  return;
}

static int
soap_allocator_ffreeall (struct ws4d_abs_allocator *allocator)
{
  struct soap *soap;

  ws4d_assert (soap_allocator_checkid (allocator), WS4D_ERR);
  soap = allocator->data;

  return WS4D_ERR;
}

static void *
soap_allocator_falloc (struct ws4d_abs_allocator *allocator, size_t size)
{
  struct soap *soap;

  ws4d_assert (soap_allocator_checkid (allocator), NULL);
  soap = allocator->data;

  return soap_malloc (soap, size);
}

static int
soap_allocator_ffree (struct ws4d_abs_allocator *allocator, void *buf)
{
  struct soap *soap;

  ws4d_assert (soap_allocator_checkid (allocator), WS4D_ERR);
  soap = allocator->data;

  soap_dealloc (soap, buf);
  return WS4D_OK;
}

static void *
soap_allocator_fmemdup (struct ws4d_abs_allocator *allocator, const void *buf,
                        size_t size)
{
  struct soap *soap;
  void *result;

  ws4d_assert (soap_allocator_checkid (allocator) && (size > 0), NULL);
  soap = allocator->data;

  result = soap_malloc (soap, size);
  if (!result)
    return NULL;

  memcpy (result, buf, size);

  return result;
}

static struct ws4d_abs_allocator_cbs soap_allocator_cbs = {
  soap_allocator_fdone,
  soap_allocator_ffreeall,
  soap_allocator_falloc,
  soap_allocator_ffree,
  soap_allocator_fmemdup,
  soap_allocator_fmemdup,
  soap_allocator_ffree
};

int
soap_allocator_finit (struct ws4d_abs_allocator *allocator, void *arg)
{
  struct soap *soap = arg;

  ws4d_assert (soap, WS4D_ERR);

  allocator->id = soap_allocator_plugin_id;
  allocator->callbacks = &soap_allocator_cbs;
  allocator->data = soap;

  return WS4D_OK;
}

/** @} */
