/*
 * C Implementation: uricmp_rfc2396
 *
 * for further information see:
 * - http://www.ietf.org/rfc/rfc2396.txt
 * - http://www.faqs.org/rfcs/rfc822.html
 * - http://www.ietf.org/rfc/rfc2732.txt
 * - http://www.ietf.org/rfc/rfc2373.txt
 *
 * Author: Sascha Feldhorst <sascha.feldhorst@udo.edu>, (C) 2008
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 */

/* TODO: use length paramater instead of strlen */
/* TODO: use memchr and strpbrk instead of loops */
/* TODO: are variable-sized arrays ok ? */

#include "stdsoap2.h"
#include "ws4d_misc.h"


int
ws4d_uri_init (struct ws4d_uri *u)
{
  if (!u)
    return SOAP_ERR;

  memset (u, 0, sizeof (struct ws4d_uri));
  WS4D_ALLOCLIST_INIT (&u->alist);

  return SOAP_OK;
}

void
ws4d_uri_done (struct ws4d_uri *u)
{
  if (!u)
    return;

  ws4d_alloclist_done (&u->alist);
  memset (u, 0, sizeof (struct ws4d_uri));
}

int
ws4d_is_escaped (const char *s)
{
  int res = 0;
  if (s && strlen (s) == 3)
    {
      if (s[0] == '%' && isxdigit (s[1]) && isxdigit (s[2]))
        {
          res = 1;              /* validation successful */
        }
    }
  return res;
}

int
ws4d_is_hostname (const char *s)
{
  int res = 0;
  int cur = 0;
  int last = 0;

  if (s)
    {
      /* host name is not allowed to be empty */
      int len = strlen (s);
      if (len == 0)
        {
          return res;           /* validation failed */
        }

      /* first and last character are not allowed to be dots or hyphens */
      if (!isalnum (s[0]) || !isalnum (s[len - 1]))
        {
          return res;           /* validation failed */
        }


      for (; s[cur] != '\0'; cur++)
        {
          if (s[cur] == '.' || s[cur] == '-')
            {
              if (cur - last < 2)
                {
                  return res;   /* validation failed */
                }
              last = cur;
            }
          else if (!isalnum (s[cur]))
            {
              return res;       /* validation failed */
            }
        }

      /* Only succeed if the toplabel does not end with a dot nor begins with
       * none alphabetic character.
       */
      if (last < cur && isalpha (s[last + 1]))
        res = 1;                /* validation succeeded */
    }
  return res;
}

int
ws4d_is_ipv4 (const char *s)
{
  struct in_addr ia;

  return ws4d_inet_pton (AF_INET, s, &ia);
}


int
ws4d_is_ipv6 (const char *s)
{
#ifdef WITH_IPV6
  struct in6_addr ia;

  return inet_pton (AF_INET6, s, &ia);
#else
  WS4D_UNUSED_PARAM (s);

  return 0;
#endif
}


int
ws4d_is_urimark (char c)
{
  return c == '-' || c == '_' || c == '.' || c == '!' || c == '~' || c == '*'
    || c == '\'' || c == '(' || c == ')';
}

int
ws4d_is_notin (char c, const char *s)
{
  int res = 1;
  if (s)
    {
      int i = 0;
      for (; s[i] != '\0'; i++)
        {
          if (c == s[i])
            {
              res = 0;          /* ok, c matches a character from s */
              break;
            }
        }
    }
  return res;
}

int
ws4d_is_urisegment (const char *s)
{
  int res = 1;
  if (s && strlen (s) > 0)
    {
      int i = 0;
      for (; s[i] != '\0'; i++)
        {
          /* check if we got a valid pchar here (see chapter 3.3) */
          if (!ws4d_is_uriunreserved (s[i])
              && ws4d_is_notin (s[i], ":@&=+$,"))
            {
              /* maybe we got a escaped character here (see chapter 2.4.1) */
              if (s[i] == '%' && s[i + 1] != '\0' && s[i + 2] != '\0')
                {
                  char tmp[4];
                  memset (tmp, 0, 4);
                  strncpy (tmp, s + i, 3);
                  if (!ws4d_is_escaped (tmp))
                    {
                      res = 0;  /* validation failed */
                      break;
                    }
                }
            }
        }
    }
  return res;
}

int
ws4d_is_uriunreserved (char c)
{
  /* part of the unreserved character set defined in spec chapter 2.3 */
  return isalnum (c) || ws4d_is_urimark (c);
}

int
ws4d_is_uric (const char *s)
{
  int res = 1;
  if (s)
    {
      int cur = 0;
      for (; s[cur] != '\0'; cur++)
        {
          /* is the current char element of the union of reserved and unreserved
           * characters ?
           */
          if (!ws4d_is_uriunreserved (s[cur])
              && ws4d_is_notin (s[cur], ";/?:@&=+$,"))
            {
              /* maybe we got a escaped character here (see chapter 2.4.1) */
              if (s[cur] == '%' && s[cur + 1] != '\0' && s[cur + 2] != '\0')
                {
                  char tmp[4];
                  memset (tmp, 0, 4);
                  strncpy (tmp, s + cur, 3);
                  if (!ws4d_is_escaped (tmp))
                    {
                      res = 0;  /* validation failed */
                      break;
                    }
                }
            }
        }
    }
  return res;
}

int
ws4d_uri_validate_scheme (const char *s)
{
  int res = 1;
  if (s)
    {
      /* only proceed if the first character is alphabetic */
      if (isalpha (s[0]))
        {
          int i = 0;
          for (; s[i] != '\0'; i++)
            {
              /* check if the only the scheme character set is used (see chapter 3.1) */
              if (!isalpha (s[i]) && !isdigit (s[i])
                  && ws4d_is_notin (s[i], "+-."))
                {
                  res = 0;      /* validation failed */
                  break;
                }
            }
        }
      else
        {
          res = 0;              /* validation failed */
        }
    }
  return res;
}

int
ws4d_uri_validate_userinfo (const char *s)
{
  int res = 1;
  if (s && strlen (s) > 0)
    {
      int i = 0;
      for (; s[i] != '\0'; i++)
        {
          /* check if the only the userinfo character set is used (see chapter 3.2.2) */
          if (!ws4d_is_uriunreserved (s[i]) && ws4d_is_notin (s[i], ";:=+$,"))
            {
              /* maybe we got a escaped character here (see chapter 2.4.1) */
              if (s[i] == '%' && s[i + 1] != '\0' && s[i + 2] != '\0')
                {
                  char tmp[4];
                  memset (tmp, 0, 4);
                  strncpy (tmp, s + i, 3);
                  if (!ws4d_is_escaped (tmp))
                    {
                      res = 0;  /* validation failed */
                      break;
                    }
                }
            }
        }
    }
  else
    {
      res = 0;                  /* validation failed */
    }
  return res;
}

int
ws4d_uri_validate_host (const char *s)
{
#ifdef WITH_IPV6
  return ws4d_is_hostname (s) || ws4d_is_ipv4 (s) || ws4d_is_ipv6 (s);
#else
  return ws4d_is_hostname (s) || ws4d_is_ipv4 (s);
#endif
}

int
ws4d_uri_validate_port (const char *s)
{
  int i = 0, result = 0;

  /* skip empty ports */
  if (!s || strlen (s) == 0)
    return 0;

  for (; s[i] != '\0'; i++)
    {
      result += (isdigit (s[i]) ? 1 : 0);
    }

  return (result == i);
}

int
ws4d_uri_validate_path (const char *s)
{
  int res = 0;
  if (s && s[0] == '/')
    {
      unsigned int cur = 1;
      unsigned int last = 0;
      char *tmp = NULL;

      for (; s[cur] != '\0'; cur++)
        {
          if (s[cur] == '/')
            {
              int len = (cur - last);
              if (len < 2)
                break;          /* validation failed */

              tmp = malloc ((len + 1) * sizeof (char));
              memset (tmp, 0, len + 1);
              strncpy (tmp, s + cur, len);

              /* only proceed if we found a valid segment here */
              if (!ws4d_is_urisegment (tmp))
                break;          /* validation failed */

              last = cur;
              free (tmp);
              tmp = NULL;
            }
        }

      if (cur == strlen (s))
        res = 1;                /* validation sucessful */
    }
  return res;
}

int
ws4d_uri_validate_query (const char *s)
{
  return ws4d_is_uric (s);
}

int
ws4d_uri_validate_fragment (const char *s)
{
  return ws4d_is_uric (s);
}

int
ws4d_parse_uri (const char *uri, struct ws4d_uri *res, int flags)
{
  int has_scheme = 0, has_host = 0;
  int cur = 0, i = 0, len;

  if (uri && res)
    {
      /* first set all fields in the result struct to NULL */
      res->fragment = NULL;
      res->host = NULL;
      res->path = NULL;
      res->port = NULL;
      res->query = NULL;
      res->scheme = NULL;
      res->userinfo = NULL;

      len = strlen (uri);

      /* ignore leading white spaces */
      for (i = 0; (cur + i) < len; i++)
        {
          if (uri[cur + i] == ' ')
            {
              cur++;
              i--;
            }
          else
            {
              break;
            }
        }

      /* search :// the character string before :// is the scheme */
      for (i = 0; (cur + i) < len; i++)
        {
          if ((uri[cur + i] == ':') && (uri[cur + i + 1] == '/')
              && (uri[cur + i + 2] == '/'))
            {
              res->scheme = ws4d_strndup (uri + cur, i, &res->alist);
              if (!res->scheme)
                {
                  return SOAP_EOM;
                }

              cur += i + 3;
              has_scheme = 1;
              break;
            }
        }

      /* fail if ws4d_uri has no scheme or contains invalid characters */
      if (!has_scheme || !ws4d_uri_validate_scheme (res->scheme))
        {
          return SOAP_ERR;
        }

      /* free scheme if flag is not set */
      if (!(flags & WITH_SCHEME))
        {
          ws4d_free_alist (res->scheme);
          res->scheme = NULL;
        }

      /* search for a @ */
      for (i = 0; (cur + i) < len; i++)
        {
          if (uri[cur + i] == '@')
            break;
        }

      /* optional userinfo is handled here */
      if ((cur + i) < len)
        {
          /* the character string before @ contains the userinfo */
          if (uri[cur + i] == '@')
            {
              /* only copy and validate the string if the corresponding flag was set */
              if (flags & WITH_USERINFO)
                {
                  res->userinfo = ws4d_strndup (uri + cur, i, &res->alist);
                  if (!res->userinfo)
                    {
                      return SOAP_EOM;
                    }

                  /* validate the specified userinfo string */
                  if (!ws4d_uri_validate_userinfo (res->userinfo))
                    {
                      return SOAP_ERR;
                    }
                }

              /* skip the userinfo and the @ */
              cur += i + 1;
            }
        }

      if (uri[cur] == '[')
        {
          cur++;

          for (i = 0;
               (((cur + i) < len) && uri[cur + i])
               && ((isascii (uri[cur + i]) && isxdigit (uri[cur + i]))
                   || uri[cur + i] == ':' || uri[cur + i] == '.'); i++)
            {
            }

          if (uri[cur + i] != ']')
            {
              return SOAP_ERR;
            }
        }
      else
        {
          /* search for : or / or ? */
          for (i = 0; (cur + i) < len; i++)
            {
              if (uri[cur + i] == '/' || uri[cur + i] == ':'
                  || uri[cur + i] == '?')
                break;
            }
        }

      /* the non empty charachter string before : or /  or ? is the host */
      if ((cur + i) < len)
        {
          res->host = ws4d_strndup (uri + cur, i, &res->alist);
          if (!res->host)
            {
              return SOAP_EOM;
            }

          has_host = 1;
          cur += i;

          if (uri[cur] == ']')
            cur++;
        }

      /* fail if ws4d_uri has no host or host contains reserved symbols */
      if (!has_host || !ws4d_uri_validate_host (res->host))
        {
          return SOAP_ERR;
        }

      /* free host if flag is not set */
      if (!(flags & WITH_HOST))
        {
          ws4d_free_alist (res->host);
          res->host = NULL;
        }

      /* stop parsing if we have reached the end of the ws4d_uri */
      if (cur >= len)
        return SOAP_OK;

      /* if there is a : the authority contains a port */
      if (uri[cur] == ':')
        {
          cur += 1;

          /* search for / or ? which terminates the authority, where the
           * is the last part (spec chapter 3.2), e.g.
           * <userinfo>@<host>:<port>
           */
          for (i = 0; (cur + i) < len; i++)
            {
              if (uri[cur + i] == '/' || uri[cur + i] == '?')
                break;
            }

          /* charachter string before / is the port */
          /* only copy and validate the string if the corresponding flag was set */
          if (flags & WITH_PORT)
            {
              res->port = ws4d_strndup (uri + cur, i, &res->alist);
              if (!res->port)
                {
                  return SOAP_EOM;
                }
              /* fail if port contains reserved characters */
              if (!ws4d_uri_validate_port (res->port))
                {
                  return SOAP_ERR;
                }
            }
          cur += i;
        }

      /* stop parsing if we have reached the end of the ws4d_uri */
      if (cur >= len)
        return SOAP_OK;

      /* character string between host and optional query or fragment is the path */
      /* search for ? or # */
      for (i = 0; (cur + i) < len; i++)
        {
          if (uri[cur + i] == '?' || uri[cur + i] == '#')
            break;
        }

      /* only copy and validate the path if the corresponding flag was set */
      if (flags & WITH_PATH)
        {
          res->path = ws4d_strndup (uri + cur, i, &res->alist);
          if (!res->path)
            {
              return SOAP_EOM;
            }

          /* validate path */
          if (!ws4d_uri_validate_path (res->path))
            {
              return SOAP_ERR;
            }
        }

      cur += i;

      /* stop parsing if we have reached the end of the ws4d_uri */
      if ((cur + 1) >= len)
        return SOAP_OK;

      /* handle an optional query */
      if (uri[cur] == '?')
        {
          cur += 1;

          /* search for # */
          for (i = 0; (cur + i) < len; i++)
            {
              if (uri[cur + i] == '#')
                break;
            }

          /* only copy and validate the string if the corresponding flag was set */
          if (flags & WITH_QUERY)
            {
              res->query = ws4d_strndup (uri + cur, i, &res->alist);
              if (!res->query)
                {
                  return SOAP_EOM;
                }

              /* fail if query is not welformed */
              if (!ws4d_uri_validate_query (res->query))
                {
                  return SOAP_ERR;
                }
            }
          cur += i;
        }

      /* stop parsing if we have reached the end of the ws4d_uri */
      if ((cur + 1) >= len)
        return SOAP_OK;

      /* handle an optional fragment */
      if (uri[cur] == '#')
        {
          cur += 1;

          /* only copy and validate the string if the corresponding flag was set */
          if (flags & WITH_FRAGMENT)
            {
              res->fragment = ws4d_strndup (uri + cur, i, &res->alist);
              if (!res->fragment)
                {
                  return SOAP_EOM;
                }

              /* fail if fragment is not welformed */
              if (!ws4d_uri_validate_fragment (res->fragment))
                {
                  return SOAP_ERR;
                }
            }

        }
      return SOAP_OK;
    }

  return SOAP_ERR;
}


int
ws4d_cmp_uri (const char *s1, const char *s2)
{
  int flags = WITH_ALL;
  int len;
  struct ws4d_uri u1, u2;
  const char *p1 = NULL;
  const char *p2 = NULL;

  ws4d_uri_init (&u1);
  ws4d_uri_init (&u2);

  /* first create ws4d_uri structs from the specified ws4d_uri strings */
  if (ws4d_parse_uri (s1, &u1, flags) != SOAP_OK)
    {
      ws4d_uri_done (&u1);
      return SOAP_SYNTAX_ERROR;
    }

  if (ws4d_parse_uri (s2, &u2, flags) != SOAP_OK)
    {
      ws4d_uri_done (&u1);
      ws4d_uri_done (&u2);
      return SOAP_SYNTAX_ERROR;
    }

  /* s1 and s2 need to have the same scheme, e.g. http */
  if (STRCASECMP (u1.scheme, u2.scheme) != 0)
    {
      ws4d_uri_done (&u1);
      ws4d_uri_done (&u2);
      return SOAP_ERR;
    }

  /* s1 and s2 need to have the same authority; refering to rfc2396 an
   * authority has the following syntax:
   * [userinfo@]host[:port]
   */

  /* first compare the optional userinfos */
  if ((u1.userinfo || u2.userinfo)
      && strcmp ((u1.userinfo ? u1.userinfo : ""),
                 (u2.userinfo ? u2.userinfo : "")) != 0)
    {
      ws4d_uri_done (&u1);
      ws4d_uri_done (&u2);
      return SOAP_ERR;
    }

  /* compare the host parts; refering to chapter 6 of the rfc the host part
   * of an ws4d_uri is not case sensitive.
   */
  if (STRCASECMP (u1.host, u2.host) != 0)
    {
      ws4d_uri_done (&u1);
      ws4d_uri_done (&u2);
      return SOAP_ERR;
    }

  /* compare the optional ports */
  p1 = (u1.port ? u1.port : "");
  p2 = (u2.port ? u2.port : "");
  if ((u1.port || u2.port) && strcmp (p1, p2) != 0)
    {
      /* maybe the default port was used in one of the uris */
      if ((strlen (p1) != 0 || strcmp (p2, "80") != 0)
          && (strlen (p2) != 0 || strcmp (p1, "80") != 0))
        {
          ws4d_uri_done (&u1);
          ws4d_uri_done (&u2);
          return SOAP_ERR;
        }
    }

  /* compare the optional paths of the specified uris */
  if (!u1.path && !u2.path)
    return SOAP_OK;

  /* check if both uris have a path */
  if ((!u1.path && u2.path) || (u1.path && !u2.path))
    {
      ws4d_uri_done (&u1);
      ws4d_uri_done (&u2);
      return SOAP_ERR;
    }

  /* At first we have to ensure, that the path segments are absolute and do
   * not contain . or .. segments (see ws-discovery chapter 5.1).
   */
  if (strstr (u1.path, ".") || strstr (u1.path, "..")
      || strstr (u2.path, ".") || strstr (u2.path, ".."))
    {
      ws4d_uri_done (&u1);
      ws4d_uri_done (&u2);
      return SOAP_SYNTAX_ERROR;
    }

  /* compare the path segment (not string) wise with each other; refer
   * to ws-discovery spec chapter 5.1
   */
  len = strlen (u1.path);
  len = (u1.path[len - 1] == '/') ? len - 1 : len;      /* ommit trailing slashes */

  /* check if u1 is a segment wise prefix of u2 */
  if ((strncmp (u1.path, u2.path, len) != 0) || (u2.path[len] != '/'
                                                 && u2.path[len] != '\0'))
    {
      ws4d_uri_done (&u1);
      ws4d_uri_done (&u2);
      return SOAP_ERR;
    }

  /* free resources */
  ws4d_uri_done (&u1);
  ws4d_uri_done (&u2);

  /* the query and fragment parts can be ignored (refering to the spec) */
  return SOAP_OK;
}

int
ws4d_uri_tostrlen (struct ws4d_uri *res)
{
  register unsigned int length = 0;

  ws4d_assert (res, 0);

  /* check if minimal parts are there */
  if (!res->scheme || !res->host || !res->path)
    {
      return 0;
    }

  /* calculate length of ws4d_uri string */
  length += strlen (res->scheme);
  length += 3;                  /* strlen ("://") */
  if (res->userinfo)
    {
      length += strlen (res->userinfo);
      length += 1;              /* strlen ("@") */
    }
  length += strlen (res->host);
  if (ws4d_is_ipv6 (res->host))
    {
      length += 2;              /* strlen ("[]") */
    }
  if (res->port)
    {
      length += 1;              /* strlen (":") */
      length += strlen (res->port);
    }
  if (res->path)
    {
      length += strlen (res->path);
      if (res->path[0] != '/')
        {
          length += 1;          /* strlen ("/") */
        }
    }
  if (res->query)
    {
      length += 1;              /* strlen ("?") */
      length += strlen (res->query);
    }
  if (res->fragment)
    {
      length += 1;              /* strlen ("#") */
      length += strlen (res->fragment);
    }

  return length;
}

int
ws4d_uri_tostr (struct ws4d_uri *res, char *uri, size_t size)
{
  register unsigned int length = 0;

  ws4d_assert (res && uri && (size > 0), WS4D_ERR);

  length = ws4d_uri_tostrlen (res);

  /* is ws4d_uri string length sufficient? */
  if (length > size)
    {
      return WS4D_ERR;
    }

  /* reset ws4d_uri string */
  memset (uri, 0, size);

  /* build ws4d_uri */
  strncat (uri, res->scheme, strlen (res->scheme));
  strncat (uri, "://", 3);
  if (res->userinfo)
    {
      strncat (uri, res->userinfo, strlen (res->userinfo));
      strncat (uri, "@", 1);
    }

  if (ws4d_is_ipv6 (res->host))
    {
      strncat (uri, "[", 1);
    }
  strncat (uri, res->host, strlen (res->host));
  if (ws4d_is_ipv6 (res->host))
    {
      strncat (uri, "]", 1);
    }

  if (res->port)
    {
      strncat (uri, ":", 1);
      strncat (uri, res->port, strlen (res->port));
    }
  if (res->path)
    {
      if (res->path[0] != '/')
        {
          strncat (uri, "/", 1);
        }
      strncat (uri, res->path, strlen (res->path));
    }
  if (res->query)
    {
      strncat (uri, "?", 1);
      strncat (uri, res->query, strlen (res->query));
    }
  if (res->fragment)
    {
      strncat (uri, "#", 1);
      strncat (uri, res->fragment, strlen (res->fragment));
    }

  return WS4D_OK;
}
