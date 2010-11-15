#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

#include "lwip/opt.h"

#include "lwip/init.h"

#include "lwip/api.h"

#include "lwip/tcpip.h"
#include "lwip/sockets.h"

#include "lwip/ip_addr.h"

#include "stdsoap2.h"
#include "gsoap-lwip-io.h"

#include "ws4d_misc.h"
#include "soap_misc.h"

/******************************************************************
 * gSOAP lwIP-Plugin                                              *
 ******************************************************************/

#define LWIP_PLUGIN_ID "lwIP-PLUGIN-0.1"
const char *lwip_plugin_id = LWIP_PLUGIN_ID;

struct plugin_peer
{
  int ipv6_multicast_if;        /* always include this to keep the soap struct size the same in v4 and v6 */
  char *ipv4_multicast_if;      /* always include this to keep the soap struct size the same in v4 and v6 */
  int ipv4_multicast_ttl;       /* multicast scope */
#ifdef WITH_IPV6
  struct sockaddr_storage peer; /* IPv6: set by soap_accept and by UDP recv */
#else
  struct sockaddr_in peer;      /* IPv4: set by soap_connect/soap_accept and by UDP recv */
#endif
};

static int lwip_plugin_init (struct soap *soap, struct soap_plugin *p,
                             void *arg);

static void
lwip_plugin_delete (struct soap *soap, struct soap_plugin *p)
{
  WS4D_UNUSED_PARAM (soap);

  free (p->data);
}


static int
lwip_plugin_copy (struct soap *soap, struct soap_plugin *dst,
                  struct soap_plugin *src)
{
  WS4D_UNUSED_PARAM (src);

  return lwip_plugin_init (soap, dst, NULL);
}

static void lwip_plugin_setio (struct soap *soap);

static int
lwip_plugin_init (struct soap *soap, struct soap_plugin *p, void *arg)
{
  lwip_plugin_setio (soap);
  WS4D_UNUSED_PARAM (arg);

  p->id = lwip_plugin_id;
  p->data = (void *) malloc (sizeof (struct plugin_peer));
  memset (p->data, 0, sizeof (struct plugin_peer));
  p->fcopy = lwip_plugin_copy;
  p->fdelete = lwip_plugin_delete;

  return SOAP_OK;
}

static struct plugin_peer *
lwip_get_plugindata (struct soap *soap)
{
  return (struct plugin_peer *) soap_lookup_plugin (soap, LWIP_PLUGIN_ID);
}

void *
soap_getpeer (struct soap *soap)
{
  struct plugin_peer *plugin = lwip_get_plugindata (soap);

  ws4d_assert (plugin, NULL);

  return &plugin->peer;
}

size_t
soap_getpeerlen (struct soap * soap)
{
#ifdef WITH_IPV6
  return sizeof (struct sockaddr_storage);
#else
  return sizeof (struct sockaddr_in);
#endif
}

/******************************************************************
 * lwIP io functions for gSOAP
 */

unsigned char debug_flags;
struct netif *netif;

#define SOAP_SOCKBLOCK(fd) fcntl(fd, F_SETFL, fcntl(fd, F_GETFL)&~O_NONBLOCK);
#define SOAP_SOCKNONBLOCK(fd) fcntl(fd, F_SETFL, fcntl(fd, F_GETFL)|O_NONBLOCK);


static int
tcp_init (struct soap *soap)
{
  soap->errmode = 1;
#ifdef WIN32
  if (tcp_done)
    return 0;
  else
    {
      WSADATA w;
      if (WSAStartup (MAKEWORD (1, 1), &w))
        return -1;
      tcp_done = 1;
    }
#endif
  return 0;
}

static const char *
tcp_error (struct soap *soap)
{
  register const char *msg = NULL;
  switch (soap->errmode)
    {
    case 0:
      /* msg = soap_strerror(soap); */
      break;
    case 1:
      msg = "WSAStartup failed";
      break;
    case 2:
      {
        sprintf (soap->msgbuf, "TCP/UDP IP error %d", soap->errnum);
        msg = soap->msgbuf;

      }
    }
  return msg;
}

static int
my_tcp_gethost (struct soap *soap, const char *addr, struct in_addr *inaddr)
{
  WS4D_UNUSED_PARAM (soap);

  inaddr->s_addr = inet_addr (addr);
  return ERR_OK;
}


static int
gsoap_lwip_fsend (struct soap *soap, const char *s, size_t n)
{
  /* get Plugin from soap struct */
  struct plugin_peer *mysoap =
    (struct plugin_peer *) lwip_get_plugindata (soap);

  register int nwritten, err;
#if defined(__cplusplus) && !defined(WITH_LEAN)
  if (soap->os)
    {
      soap->os->write (s, (std::streamsize) n);
      if (soap->os->good ())
        return SOAP_OK;
      soap->errnum = 0;
      return SOAP_EOF;
    }
#endif
  while (n)
    {
      if (soap_valid_socket (soap->socket))
        {
#ifndef WITH_LEAN
          if (soap->send_timeout)
            {
#ifndef WIN32
              if ((int) soap->socket >= (int) FD_SETSIZE)
                return SOAP_FD_EXCEEDED;        /* Hint: MUST increase FD_SETSIZE */
#endif
              for (;;)
                {
                  struct timeval timeout;
                  fd_set fd;
                  register int r;
                  if (soap->send_timeout > 0)
                    {
                      timeout.tv_sec = soap->send_timeout;
                      timeout.tv_usec = 0;
                    }
                  else
                    {
                      timeout.tv_sec = -soap->send_timeout / 1000000;
                      timeout.tv_usec = -soap->send_timeout % 1000000;
                    }
                  FD_ZERO (&fd);
                  FD_SET (soap->socket, &fd);
#ifdef WITH_OPENSSL
                  if (soap->ssl)
                    r =
                      select ((int) soap->socket + 1, &fd, &fd, &fd,
                              &timeout);
                  else
#endif
                    r =
                      select ((int) soap->socket + 1, NULL, &fd, &fd,
                              &timeout);
                  if (r > 0)
                    break;
                  if (!r)
                    {
                      soap->errnum = 0;
                      return SOAP_EOF;
                    }
                  err = soap_socket_errno (soap->socket);
                  if (err != SOAP_EINTR && err != SOAP_EAGAIN
                      && err != SOAP_EWOULDBLOCK)
                    {
                      soap->errnum = err;
                      return SOAP_EOF;
                    }
                }
            }
#endif
#ifdef WITH_OPENSSL
          if (soap->ssl)
            nwritten = SSL_write (soap->ssl, s, (int) n);
          else if (soap->bio)
            nwritten = BIO_write (soap->bio, s, (int) n);
          else
#endif
#ifndef WITH_LEAN
          if ((soap->omode & SOAP_IO_UDP))
            {
              if (soap->peerlen)
                nwritten =
                  sendto (soap->socket, s, (SOAP_WINSOCKINT) n,
                          soap->socket_flags,
                          (struct sockaddr *) &mysoap->peer,
                          (SOAP_WINSOCKINT) soap->peerlen);
              else
                nwritten =
                  send (soap->socket, s, (SOAP_WINSOCKINT) n,
                        soap->socket_flags);
              /* retry and back-off algorithm */
              /* TODO: this is not very clear from specs so verify and limit conditions under which we should loop (e.g. ENOBUFS) */
              if (nwritten < 0)
                {
                  struct timeval timeout;
                  fd_set fd;
                  int udp_repeat;
                  int udp_delay;
#ifndef WIN32
                  if ((int) soap->socket >= (int) FD_SETSIZE)
                    return SOAP_FD_EXCEEDED;    /* Hint: MUST increase FD_SETSIZE */
#endif
                  if ((soap->connect_flags & SO_BROADCAST))
                    udp_repeat = 3;     /* SOAP-over-UDP MULTICAST_UDP_REPEAT - 1 */
                  else
                    udp_repeat = 1;     /* SOAP-over-UDP UNICAST_UDP_REPEAT - 1 */
                  udp_delay = (soap_random % 201) + 50; /* UDP_MIN_DELAY .. UDP_MAX_DELAY */
                  do
                    {
                      timeout.tv_sec = 0;
                      timeout.tv_usec = 1000 * udp_delay;       /* ms */
                      FD_ZERO (&fd);
                      FD_SET (soap->socket, &fd);
                      select ((int) soap->socket + 1, NULL, NULL, &fd,
                              &timeout);
                      if (soap->peerlen)
                        nwritten =
                          sendto (soap->socket, s, (SOAP_WINSOCKINT) n,
                                  soap->socket_flags,
                                  (struct sockaddr *) &mysoap->peer,
                                  (SOAP_WINSOCKINT) soap->peerlen);
                      else
                        nwritten =
                          send (soap->socket, s, (SOAP_WINSOCKINT) n,
                                soap->socket_flags);
                      udp_delay <<= 1;
                      if (udp_delay > 500)      /* UDP_UPPER_DELAY */
                        udp_delay = 500;
                    }
                  while (nwritten < 0 && --udp_repeat > 0);
                }
            }
          else
#endif
#if !defined(PALM) && !defined(AS400)
            nwritten = send (soap->socket, s, (int) n, soap->socket_flags);
#else
            nwritten = send (soap->socket, (void *) s, n, soap->socket_flags);
#endif
          if (nwritten <= 0)
            {
#if defined(WITH_OPENSSL) || !defined(WITH_LEAN)
              register int r = 0;
#endif
              err = soap_socket_errno (soap->socket);
#ifdef WITH_OPENSSL
              if (soap->ssl
                  && (r =
                      SSL_get_error (soap->ssl, nwritten)) != SSL_ERROR_NONE
                  && r != SSL_ERROR_WANT_READ && r != SSL_ERROR_WANT_WRITE)
                {
                  soap->errnum = err;
                  return SOAP_EOF;
                }
#endif
              if (err == SOAP_EWOULDBLOCK || err == SOAP_EAGAIN)
                {
#ifndef WITH_LEAN
                  struct timeval timeout;
                  fd_set fd;
#ifndef WIN32
                  if ((int) soap->socket >= (int) FD_SETSIZE)
                    return SOAP_FD_EXCEEDED;    /* Hint: MUST increase FD_SETSIZE */
#endif
                  if (soap->send_timeout > 0)
                    {
                      timeout.tv_sec = soap->send_timeout;
                      timeout.tv_usec = 0;
                    }
                  else if (soap->send_timeout < 0)
                    {
                      timeout.tv_sec = -soap->send_timeout / 1000000;
                      timeout.tv_usec = -soap->send_timeout % 1000000;
                    }
                  else
                    {
                      timeout.tv_sec = 0;
                      timeout.tv_usec = 10000;
                    }
                  FD_ZERO (&fd);
                  FD_SET (soap->socket, &fd);
#ifdef WITH_OPENSSL
                  if (soap->ssl && r == SSL_ERROR_WANT_READ)
                    r =
                      select ((int) soap->socket + 1, &fd, NULL, &fd,
                              &timeout);
                  else
                    r =
                      select ((int) soap->socket + 1, NULL, &fd, &fd,
                              &timeout);
#else
                  r =
                    select ((int) soap->socket + 1, NULL, &fd, &fd, &timeout);
#endif
                  if (r < 0
                      && (r = soap_socket_errno (soap->socket)) != SOAP_EINTR)
                    {
                      soap->errnum = r;
                      return SOAP_EOF;
                    }
#endif
                }
              else if (err && err != SOAP_EINTR)
                {
                  soap->errnum = err;
                  return SOAP_EOF;
                }
              nwritten = 0;     /* and call write() again */
            }
        }
      else
        {
#ifdef WITH_FASTCGI
          nwritten = fwrite ((void *) s, 1, n, stdout);
          fflush (stdout);
#else
#ifdef UNDER_CE
          nwritten = fwrite (s, 1, n, soap->sendfd);
#else
#ifdef VXWORKS
#ifdef WMW_RPM_IO
          if (soap->rpmreqid)
            nwritten = (httpBlockPut (soap->rpmreqid, s, n) == 0) ? n : -1;
          else
#endif
            nwritten =
              fwrite (s, sizeof (char), n, fdopen (soap->sendfd, "w"));
#else
          nwritten = write (soap->sendfd, s, (unsigned int) n);
#endif
#endif
#endif
          if (nwritten <= 0)
            {
#ifndef WITH_FASTCGI
              err = soap_errno;
#else
              err = EOF;
#endif
              if (err && err != SOAP_EINTR && err != SOAP_EWOULDBLOCK
                  && err != SOAP_EAGAIN)
                {
                  soap->errnum = err;
                  return SOAP_EOF;
                }
              nwritten = 0;     /* and call write() again */
            }
        }
      n -= nwritten;
      s += nwritten;
    }

  return SOAP_OK;
}

static size_t
gsoap_lwip_frecv (struct soap *soap, char *s, size_t n)
{
  /* get Plugin from soap struct */
  struct plugin_peer *mysoap = lwip_get_plugindata (soap);

  register int r;
#ifndef WITH_LEAN
  register int retries = 100;   /* max 100 retries with non-blocking sockets */
#endif
  soap->errnum = 0;
#if defined(__cplusplus) && !defined(WITH_LEAN)
  if (soap->is)
    {
      if (soap->is->good ())
        return soap->is->read (s, (std::streamsize) n).gcount ();
      return 0;
    }
#endif
  if (soap_valid_socket (soap->socket))
    {
      for (;;)
        {
#ifdef WITH_OPENSSL
          register int err = 0;
#endif
#ifndef WITH_LEAN
#ifdef WITH_OPENSSL
          if (soap->recv_timeout && !soap->ssl) /* SSL: sockets are nonblocking */
#else
          if (soap->recv_timeout)
#endif
            {
#ifndef WIN32
              if ((int) soap->socket >= (int) FD_SETSIZE)
                {
                  soap->error = SOAP_FD_EXCEEDED;
                  return 0;     /* Hint: MUST increase FD_SETSIZE */
                }
#endif
              for (;;)
                {
                  struct timeval timeout;
                  fd_set fd;
                  if (soap->recv_timeout > 0)
                    {
                      timeout.tv_sec = soap->recv_timeout;
                      timeout.tv_usec = 0;
                    }
                  else
                    {
                      timeout.tv_sec = -soap->recv_timeout / 1000000;
                      timeout.tv_usec = -soap->recv_timeout % 1000000;
                    }
                  FD_ZERO (&fd);
                  FD_SET (soap->socket, &fd);
                  r =
                    select ((int) soap->socket + 1, &fd, NULL, &fd, &timeout);
                  if (r > 0)
                    break;
                  if (!r)
                    {
                      soap->errnum = 0;
                      return 0;
                    }
                  r = soap_socket_errno (soap->socket);
                  if (r != SOAP_EINTR && r != SOAP_EAGAIN
                      && r != SOAP_EWOULDBLOCK)
                    {
                      soap->errnum = r;
                      return 0;
                    }
                }
            }
#endif
#ifdef WITH_OPENSSL
          if (soap->ssl)
            {
              r = SSL_read (soap->ssl, s, (int) n);
              if (r > 0)
                return (size_t) r;
              err = SSL_get_error (soap->ssl, r);
              if (err != SSL_ERROR_NONE && err != SSL_ERROR_WANT_READ
                  && err != SSL_ERROR_WANT_WRITE)
                return 0;
            }
          else if (soap->bio)
            {
              r = BIO_read (soap->bio, s, (int) n);
              if (r > 0)
                return (size_t) r;
              return 0;
            }
          else
#endif
            {
#ifndef WITH_LEAN
              if ((soap->omode & SOAP_IO_UDP))
                {
                  SOAP_SOCKLEN_T k = (SOAP_SOCKLEN_T) sizeof (mysoap->peer);
                  memset ((void *) &mysoap->peer, 0, sizeof (mysoap->peer));
                  r = recvfrom (soap->socket, s, (SOAP_WINSOCKINT) n, soap->socket_flags, (struct sockaddr *) &mysoap->peer, &k);       /* portability note: see SOAP_SOCKLEN_T definition in stdsoap2.h */
                  soap->peerlen = (size_t) k;
#ifndef WITH_IPV6
                  soap->ip = ntohl (mysoap->peer.sin_addr.s_addr);
#endif
                }
              else
#endif
                r = recv (soap->socket, s, (int) n, soap->socket_flags);
#ifdef PALM
              /* CycleSyncDisplay(curStatusMsg); */
#endif
              if (r >= 0)
                {
                  return (size_t) r;
                }
              r = soap_socket_errno (soap->socket);
              if (r != SOAP_EINTR && r != SOAP_EAGAIN
                  && r != SOAP_EWOULDBLOCK)
                {
                  soap->errnum = r;
                  return 0;
                }
            }
#ifndef WITH_LEAN
          {
            struct timeval timeout;
            fd_set fd;
            if (soap->recv_timeout > 0)
              {
                timeout.tv_sec = soap->recv_timeout;
                timeout.tv_usec = 0;
              }
            else if (soap->recv_timeout < 0)
              {
                timeout.tv_sec = -soap->recv_timeout / 1000000;
                timeout.tv_usec = -soap->recv_timeout % 1000000;
              }
            else
              {
                timeout.tv_sec = 5;
                timeout.tv_usec = 0;
              }
#ifndef WIN32
            if ((int) soap->socket >= (int) FD_SETSIZE)
              {
                soap->error = SOAP_FD_EXCEEDED;
                return 0;       /* Hint: MUST increase FD_SETSIZE */
              }
#endif
            FD_ZERO (&fd);
            FD_SET (soap->socket, &fd);
#ifdef WITH_OPENSSL
            if (soap->ssl && err == SSL_ERROR_WANT_WRITE)
              r = select ((int) soap->socket + 1, NULL, &fd, &fd, &timeout);
            else
              r = select ((int) soap->socket + 1, &fd, NULL, &fd, &timeout);
#else
            r = select ((int) soap->socket + 1, &fd, NULL, &fd, &timeout);
#endif
            if (!r && soap->recv_timeout)
              {
                soap->errnum = 0;
                return 0;
              }
            if (r < 0)
              {
                r = soap_socket_errno (soap->socket);
                if (r != SOAP_EINTR && r != SOAP_EAGAIN
                    && r != SOAP_EWOULDBLOCK)
                  {
                    soap->errnum = r;
                    return 0;
                  }
              }
            if (retries-- <= 0)
              {
                soap->errnum = soap_socket_errno (soap->socket);
                return 0;
              }
          }
#endif
#ifdef PALM
          r = soap_socket_errno (soap->socket);
          if (r != SOAP_EINTR && retries-- <= 0)
            {
              soap->errnum = r;
              return 0;
            }
#endif
        }
    }
#ifdef WITH_FASTCGI
  return fread (s, 1, n, stdin);
#else
#ifdef UNDER_CE
  return fread (s, 1, n, soap->recvfd);
#else
#ifdef WMW_RPM_IO
  if (soap->rpmreqid)
    r = httpBlockRead (soap->rpmreqid, s, n);
  else
#endif
    r = read (soap->recvfd, s, (unsigned int) n);
  if (r >= 0)
    return (size_t) r;
  soap->errnum = soap_errno;
  return 0;
#endif
#endif
}

static SOAP_SOCKET
gsoap_lwip_fopen (struct soap *soap, const char *endpoint, const char *host,
                  int port)
{
  //get Plugin from soap struct
  struct plugin_peer *mysoap = lwip_get_plugindata (soap);

#ifdef WITH_IPV6
  struct addrinfo hints, *res, *ressave;
#endif
  SOAP_SOCKET fd;
  int err = 0;
#ifndef WITH_LEAN
  int retry = 10;
  int len = SOAP_BUFLEN;
  int set = 1;
#endif
  if (soap_valid_socket (soap->socket))
    soap->fclosesocket (soap, soap->socket);
  soap->socket = SOAP_INVALID_SOCKET;
  if (tcp_init (soap))
    {
      soap->errnum = 0;
      soap_set_sender_error (soap, tcp_error (soap),
                             "TCP init failed in tcp_connect()",
                             SOAP_TCP_ERROR);
      return SOAP_INVALID_SOCKET;
    }
  soap->errmode = 0;
#ifdef WITH_IPV6
  memset ((void *) &hints, 0, sizeof (hints));
  hints.ai_family = PF_UNSPEC;
#ifndef WITH_LEAN
  if ((soap->omode & SOAP_IO_UDP))
    hints.ai_socktype = SOCK_DGRAM;
  else
#endif
    hints.ai_socktype = SOCK_STREAM;
  soap->errmode = 2;
  if (soap->proxy_host)
    err =
      getaddrinfo (soap->proxy_host, soap_int2s (soap, soap->proxy_port),
                   &hints, &res);
  else
    err = getaddrinfo (host, soap_int2s (soap, port), &hints, &res);
  if (err)
    {
      soap_set_sender_error (soap, SOAP_GAI_STRERROR (err),
                             "getaddrinfo failed in tcp_connect()",
                             SOAP_TCP_ERROR);
      return SOAP_INVALID_SOCKET;
    }
  ressave = res;
again:
  fd = socket (res->ai_family, res->ai_socktype, res->ai_protocol);
  soap->errmode = 0;
#else
#ifndef WITH_LEAN
again:
#endif
#ifndef WITH_LEAN
  if ((soap->omode & SOAP_IO_UDP))
    fd = socket (AF_INET, SOCK_DGRAM, 0);
  else
#endif
    fd = socket (AF_INET, SOCK_STREAM, 0);
#endif
  if (!soap_valid_socket (fd))
    {
#ifdef WITH_IPV6
      if (res->ai_next)
        {
          res = res->ai_next;
          goto again;
        }
#endif
      soap->errnum = soap_socket_errno (fd);
      soap_set_sender_error (soap, tcp_error (soap),
                             "socket failed in tcp_connect()",
                             SOAP_TCP_ERROR);
#ifdef WITH_IPV6
      freeaddrinfo (ressave);
#endif
      return SOAP_INVALID_SOCKET;
    }
#ifdef SOCKET_CLOSE_ON_EXEC
#ifdef WIN32
#ifndef UNDER_CE
  SetHandleInformation ((HANDLE) fd, HANDLE_FLAG_INHERIT, 0);
#endif
#else
  fcntl (fd, F_SETFD, 1);
#endif
#endif
#ifndef WITH_LEAN
  if (soap->connect_flags == SO_LINGER)
    {
      struct linger linger;
      memset ((void *) &linger, 0, sizeof (linger));
      linger.l_onoff = 1;
      linger.l_linger = soap->linger_time;
      if (setsockopt
          (fd, SOL_SOCKET, SO_LINGER, (char *) &linger,
           sizeof (struct linger)))
        {
          soap->errnum = soap_socket_errno (fd);
          soap_set_sender_error (soap, tcp_error (soap),
                                 "setsockopt SO_LINGER failed in tcp_connect()",
                                 SOAP_TCP_ERROR);
          soap->fclosesocket (soap, fd);
#ifdef WITH_IPV6
          freeaddrinfo (ressave);
#endif
          return SOAP_INVALID_SOCKET;
        }
    }
  else if (soap->connect_flags
           && setsockopt (fd, SOL_SOCKET, soap->connect_flags, (char *) &set,
                          sizeof (int)))
    {
      soap->errnum = soap_socket_errno (fd);
      soap_set_sender_error (soap, tcp_error (soap),
                             "setsockopt failed in tcp_connect()",
                             SOAP_TCP_ERROR);
      soap->fclosesocket (soap, fd);
#ifdef WITH_IPV6
      freeaddrinfo (ressave);
#endif
      return SOAP_INVALID_SOCKET;
    }
  if ((soap->keep_alive || soap->tcp_keep_alive)
      && setsockopt (fd, SOL_SOCKET, SO_KEEPALIVE, (char *) &set,
                     sizeof (int)))
    {
      soap->errnum = soap_socket_errno (fd);
      soap_set_sender_error (soap, tcp_error (soap),
                             "setsockopt SO_KEEPALIVE failed in tcp_connect()",
                             SOAP_TCP_ERROR);
      soap->fclosesocket (soap, fd);
#ifdef WITH_IPV6
      freeaddrinfo (ressave);
#endif
      return SOAP_INVALID_SOCKET;
    }
#ifdef TCP_KEEPIDLE
  if (soap->tcp_keep_idle
      && setsockopt ((SOAP_SOCKET) fd, IPPROTO_TCP, TCP_KEEPIDLE,
                     (unsigned int *) &(soap->tcp_keep_idle), sizeof (int)))
    {
      soap->errnum = soap_socket_errno (fd);
      soap_set_sender_error (soap, tcp_error (soap),
                             "setsockopt TCP_KEEPIDLE failed in tcp_connect()",
                             SOAP_TCP_ERROR);
      soap->fclosesocket (soap, (SOAP_SOCKET) fd);
#ifdef WITH_IPV6
      freeaddrinfo (ressave);
#endif
      return SOAP_INVALID_SOCKET;
    }
#endif
#ifdef TCP_KEEPINTVL
  if (soap->tcp_keep_intvl
      && setsockopt ((SOAP_SOCKET) fd, IPPROTO_TCP, TCP_KEEPINTVL,
                     (unsigned int *) &(soap->tcp_keep_intvl), sizeof (int)))
    {
      soap->errnum = soap_socket_errno (fd);
      soap_set_sender_error (soap, tcp_error (soap),
                             "setsockopt TCP_KEEPINTVL failed in tcp_connect()",
                             SOAP_TCP_ERROR);
      soap->fclosesocket (soap, (SOAP_SOCKET) fd);
#ifdef WITH_IPV6
      freeaddrinfo (ressave);
#endif
      return SOAP_INVALID_SOCKET;
    }
#endif
#ifdef TCP_KEEPCNT
  if (soap->tcp_keep_cnt
      && setsockopt ((SOAP_SOCKET) fd, IPPROTO_TCP, TCP_KEEPCNT,
                     (unsigned int *) &(soap->tcp_keep_cnt), sizeof (int)))
    {
      soap->errnum = soap_socket_errno (fd);
      soap_set_sender_error (soap, tcp_error (soap),
                             "setsockopt TCP_KEEPCNT failed in tcp_connect()",
                             SOAP_TCP_ERROR);
      soap->fclosesocket (soap, (SOAP_SOCKET) fd);
#ifdef WITH_IPV6
      freeaddrinfo (ressave);
#endif
      return SOAP_INVALID_SOCKET;
    }
#endif
#ifdef TCP_NODELAY
  if (!(soap->omode & SOAP_IO_UDP)
      && setsockopt (fd, IPPROTO_TCP, TCP_NODELAY, (char *) &set,
                     sizeof (int)))
    {
      soap->errnum = soap_socket_errno (fd);
      soap_set_sender_error (soap, tcp_error (soap),
                             "setsockopt TCP_NODELAY failed in tcp_connect()",
                             SOAP_TCP_ERROR);
      soap->fclosesocket (soap, fd);
#ifdef WITH_IPV6
      freeaddrinfo (ressave);
#endif
      return SOAP_INVALID_SOCKET;
    }
#endif
#ifdef WITH_IPV6
  if ((soap->omode & SOAP_IO_UDP) && soap->ipv6_multicast_if)
    {
      struct sockaddr_in6 *in6addr = (struct sockaddr_in6 *) res->ai_addr;
      in6addr->sin6_scope_id = soap->ipv6_multicast_if;
    }
#else
  if ((soap->omode & SOAP_IO_UDP) && mysoap->ipv4_multicast_if)
    {
      if (mysoap->ipv4_multicast_ttl > 0)
        {
          char ttl = (char) (mysoap->ipv4_multicast_ttl);
          if (setsockopt
              (fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof (ttl)))
            {
              soap->errnum = soap_socket_errno (fd);
              soap_set_sender_error (soap, tcp_error (soap),
                                     "setsockopt IP_MULTICAST_TTL failed in tcp_connect()",
                                     SOAP_TCP_ERROR);
              soap->fclosesocket (soap, fd);
              return SOAP_INVALID_SOCKET;
            }
        }
#ifndef WINDOWS
      if (setsockopt
          (fd, IPPROTO_IP, IP_MULTICAST_IF, mysoap->ipv4_multicast_if,
           sizeof (struct in_addr)))
        {
          soap->errnum = soap_socket_errno (fd);
          soap_set_sender_error (soap, tcp_error (soap),
                                 "setsockopt IP_MULTICAST_IF failed in tcp_connect()",
                                 SOAP_TCP_ERROR);
          soap->fclosesocket (soap, fd);
          return SOAP_INVALID_SOCKET;
        }
#else
#ifndef IP_MULTICAST_IF
#define IP_MULTICAST_IF 2
#endif
      if (setsockopt
          (fd, IPPROTO_IP, IP_MULTICAST_IF, soap->ipv4_multicast_if,
           sizeof (struct in_addr)))
        {
          soap->errnum = soap_socket_errno (fd);
          soap_set_sender_error (soap, tcp_error (soap),
                                 "setsockopt IP_MULTICAST_IF failed in tcp_connect()",
                                 SOAP_TCP_ERROR);
          soap->fclosesocket (soap, fd);
          return SOAP_INVALID_SOCKET;
        }
#endif
    }
#endif
#endif
  DBGLOG (TEST,
          SOAP_MESSAGE (fdebug, "Opening socket %d to host='%s' port=%d\n",
                        fd, host, port));
#ifndef WITH_IPV6
  soap->peerlen = sizeof (mysoap->peer);
  memset ((void *) &mysoap->peer, 0, sizeof (mysoap->peer));

  mysoap->peer.sin_family = AF_INET;
  soap->errmode = 2;
  if (soap->proxy_host)
    {
      if (soap->fresolve (soap, soap->proxy_host, &mysoap->peer.sin_addr))
        // {   if(my_tcp_gethost(soap, soap->proxy_host, &mysoap->peer.sin_addr))
        {
          soap_set_sender_error (soap, tcp_error (soap),
                                 "get proxy host by name failed in tcp_connect()",
                                 SOAP_TCP_ERROR);
          soap->fclosesocket (soap, fd);
#ifdef WITH_IPV6
          freeaddrinfo (ressave);
#endif
          return SOAP_INVALID_SOCKET;
        }
      mysoap->peer.sin_port = htons ((short) soap->proxy_port);
    }
  else
    {
      if (soap->fresolve (soap, host, &mysoap->peer.sin_addr))
        // { if (my_tcp_gethost(soap, host, &mysoap->peer.sin_addr))
        {
          soap_set_sender_error (soap, tcp_error (soap),
                                 "get host by name failed in tcp_connect()",
                                 SOAP_TCP_ERROR);
          soap->fclosesocket (soap, fd);
#ifdef WITH_IPV6
          freeaddrinfo (ressave);
#endif
          return SOAP_INVALID_SOCKET;
        }
      mysoap->peer.sin_port = htons ((short) port);
    }
  soap->errmode = 0;
#ifndef WITH_LEAN
  if ((soap->omode & SOAP_IO_UDP))
    {
#ifdef WITH_IPV6
      freeaddrinfo (ressave);
#endif
      return fd;
    }
#endif
#endif
#ifndef WITH_LEAN
  if (soap->connect_timeout)
    SOAP_SOCKNONBLOCK (fd)
    else
    SOAP_SOCKBLOCK (fd)
#endif
      for (;;)
      {
#ifdef WITH_IPV6
        if (connect (fd, res->ai_addr, (int) res->ai_addrlen))
#else
        if (connect
            (fd, (struct sockaddr *) &mysoap->peer, sizeof (mysoap->peer)))
#endif
          {
            err = soap_socket_errno (fd);
#ifndef WITH_LEAN
            if (err == SOAP_EADDRINUSE)
              {
                soap->fclosesocket (soap, fd);
                if (retry-- > 0)
                  goto again;
              }
            else if (soap->connect_timeout
                     && (err == SOAP_EINPROGRESS || err == SOAP_EAGAIN
                         || err == SOAP_EWOULDBLOCK))
              {
                SOAP_SOCKLEN_T k;
#ifndef WIN32
                if ((int) soap->socket >= (int) FD_SETSIZE)
                  {
                    soap->error = SOAP_FD_EXCEEDED;
#ifdef WITH_IPV6
                    freeaddrinfo (ressave);
#endif
                    return SOAP_INVALID_SOCKET; /* Hint: MUST increase FD_SETSIZE */
                  }
#endif
                for (;;)
                  {
                    struct timeval timeout;
                    fd_set fds;
                    register int r;
                    if (soap->connect_timeout > 0)
                      {
                        timeout.tv_sec = soap->connect_timeout;
                        timeout.tv_usec = 0;
                      }
                    else
                      {
                        timeout.tv_sec = -soap->connect_timeout / 1000000;
                        timeout.tv_usec = -soap->connect_timeout % 1000000;
                      }
                    FD_ZERO (&fds);
                    FD_SET (fd, &fds);
                    r = select ((int) fd + 1, NULL, &fds, NULL, &timeout);
                    if (r > 0)
                      break;
                    if (!r)
                      {
                        soap->errnum = 0;
                        DBGLOG (TEST,
                                SOAP_MESSAGE (fdebug, "Connect timeout\n"));
                        soap_set_sender_error (soap, "Timeout",
                                               "connect failed in tcp_connect()",
                                               SOAP_TCP_ERROR);
                        soap->fclosesocket (soap, fd);
#ifdef WITH_IPV6
                        freeaddrinfo (ressave);
#endif
                        return SOAP_INVALID_SOCKET;
                      }
                    r = soap_socket_errno (fd);
                    if (r != SOAP_EINTR)
                      {
                        soap->errnum = r;
                        DBGLOG (TEST,
                                SOAP_MESSAGE (fdebug,
                                              "Could not connect to host\n"));
                        soap_set_sender_error (soap, tcp_error (soap),
                                               "connect failed in tcp_connect()",
                                               SOAP_TCP_ERROR);
                        soap->fclosesocket (soap, fd);
#ifdef WITH_IPV6
                        freeaddrinfo (ressave);
#endif
                        return SOAP_INVALID_SOCKET;
                      }
                  }
                k = (SOAP_SOCKLEN_T) sizeof (soap->errnum);
                if (!getsockopt (fd, SOL_SOCKET, SO_ERROR, (char *) &soap->errnum, &k) && !soap->errnum)        /* portability note: see SOAP_SOCKLEN_T definition in stdsoap2.h */
                  break;
                DBGLOG (TEST,
                        SOAP_MESSAGE (fdebug, "Could not connect to host\n"));
                if (!soap->errnum)
                  soap->errnum = soap_socket_errno (fd);
                soap_set_sender_error (soap, tcp_error (soap),
                                       "connect failed in tcp_connect()",
                                       SOAP_TCP_ERROR);
                soap->fclosesocket (soap, fd);
#ifdef WITH_IPV6
                freeaddrinfo (ressave);
#endif
                return SOAP_INVALID_SOCKET;
              }
#endif
#ifdef WITH_IPV6
            if (res->ai_next)
              {
                res = res->ai_next;
                soap->fclosesocket (soap, fd);
                goto again;
              }
#endif
            if (err && err != SOAP_EINTR)
              {
                soap->errnum = err;
                DBGLOG (TEST,
                        SOAP_MESSAGE (fdebug, "Could not connect to host\n"));
                soap_set_sender_error (soap, tcp_error (soap),
                                       "connect failed in tcp_connect()",
                                       SOAP_TCP_ERROR);
                soap->fclosesocket (soap, fd);
#ifdef WITH_IPV6
                freeaddrinfo (ressave);
#endif
                return SOAP_INVALID_SOCKET;
              }
          }
        else
          break;
      }
#ifdef WITH_IPV6
  soap->peerlen = 0;            /* IPv6: already connected so use send() */
  freeaddrinfo (ressave);
#endif
#ifndef WITH_LEAN
  if (soap->recv_timeout || soap->send_timeout)
    SOAP_SOCKNONBLOCK (fd)
    else
    SOAP_SOCKBLOCK (fd)
#endif
      soap->socket = fd;
  soap->imode &= ~SOAP_ENC_SSL;
  soap->omode &= ~SOAP_ENC_SSL;
  if (!soap_tag_cmp (endpoint, "https:*"))
    {
#ifdef WITH_OPENSSL
      BIO *bio;
      int r;
      if (soap->proxy_host)
        {
          soap_mode m = soap->omode;    /* make sure we only parse HTTP */
          size_t n = soap->count;       /* save the content length */
          char *userid, *passwd;
          soap->omode &= ~SOAP_ENC;     /* mask IO and ENC */
          soap->omode |= SOAP_IO_BUFFER;
          DBGLOG (TEST,
                  SOAP_MESSAGE (fdebug, "Connecting to %s proxy server\n",
                                soap->proxy_http_version));
          sprintf (soap->tmpbuf, "CONNECT %s:%d HTTP/%s", host, port,
                   soap->proxy_http_version);
          if (soap_begin_send (soap)
              || (soap->error = soap->fposthdr (soap, soap->tmpbuf, NULL)))
            {
              soap->fclosesocket (soap, fd);
              return SOAP_INVALID_SOCKET;
            }
#ifndef WITH_LEAN
          if (soap->proxy_userid && soap->proxy_passwd
              && strlen (soap->proxy_userid) + strlen (soap->proxy_passwd) <
              761)
            {
              sprintf (soap->tmpbuf + 262, "%s:%s", soap->proxy_userid,
                       soap->proxy_passwd);
              strcpy (soap->tmpbuf, "Basic ");
              soap_s2base64 (soap,
                             (const unsigned char *) (soap->tmpbuf + 262),
                             soap->tmpbuf + 6,
                             (int) strlen (soap->tmpbuf + 262));
              if ((soap->error =
                   soap->fposthdr (soap, "Proxy-Authorization",
                                   soap->tmpbuf)))
                {
                  soap->fclosesocket (soap, fd);
                  return soap->error;
                }
            }
#endif
          if ((soap->error = soap->fposthdr (soap, NULL, NULL))
              || soap_flush (soap))
            {
              soap->fclosesocket (soap, fd);
              return SOAP_INVALID_SOCKET;
            }
          soap->omode = m;
          m = soap->imode;
          soap->imode &= ~SOAP_ENC;     /* mask IO and ENC */
          userid = soap->userid;        /* preserve */
          passwd = soap->passwd;        /* preserve */
          if ((soap->error = soap->fparse (soap)))
            {
              soap->fclosesocket (soap, fd);
              return SOAP_INVALID_SOCKET;
            }
          soap->userid = userid;        /* restore */
          soap->passwd = passwd;        /* restore */
          soap->imode = m;      /* restore */
          soap->count = n;      /* restore */
          if (soap_begin_send (soap))
            {
              soap->fclosesocket (soap, fd);
              return SOAP_INVALID_SOCKET;
            }
          if (endpoint)
            strncpy (soap->endpoint, endpoint, sizeof (soap->endpoint) - 1);    /* restore */
        }
      if (!soap->ctx && (soap->error = soap->fsslauth (soap)))
        {
          soap_set_sender_error (soap, "SSL error",
                                 "SSL authentication failed in tcp_connect(): check password, key file, and ca file.",
                                 SOAP_SSL_ERROR);
          soap->fclosesocket (soap, fd);
          return SOAP_INVALID_SOCKET;
        }
      if (!soap->ssl)
        {
          soap->ssl = SSL_new (soap->ctx);
          if (!soap->ssl)
            {
              soap->fclosesocket (soap, fd);
              soap->error = SOAP_SSL_ERROR;
              return SOAP_INVALID_SOCKET;
            }
        }
      else
        SSL_clear (soap->ssl);
      if (soap->session)
        {
          if (!strcmp (soap->session_host, host)
              && soap->session_port == port)
            SSL_set_session (soap->ssl, soap->session);
          SSL_SESSION_free (soap->session);
          soap->session = NULL;
        }
      soap->imode |= SOAP_ENC_SSL;
      soap->omode |= SOAP_ENC_SSL;
      bio = BIO_new_socket ((int) fd, BIO_NOCLOSE);
      SSL_set_bio (soap->ssl, bio, bio);
#ifndef WITH_LEAN
      /* Connect timeout: set SSL sockets to non-blocking */
      if (soap->connect_timeout)
        SOAP_SOCKNONBLOCK (fd)
        else
        SOAP_SOCKBLOCK (fd)
          /* Try connecting until success or timeout */
          do
          {
            if ((r = SSL_connect (soap->ssl)) <= 0)
              {
                int err;
                if ((err = SSL_get_error (soap->ssl, r)) == SSL_ERROR_NONE)
                  break;
                if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE)
                  {
                    soap_set_sender_error (soap, soap_ssl_error (soap, r),
                                           "SSL connect failed in tcp_connect()",
                                           SOAP_SSL_ERROR);
                    soap->fclosesocket (soap, fd);
                    return SOAP_INVALID_SOCKET;
                  }
                if (soap->connect_timeout)
                  {
#ifndef WIN32
                    if ((int) soap->socket >= (int) FD_SETSIZE)
                      {
                        soap->error = SOAP_FD_EXCEEDED;
                        return SOAP_INVALID_SOCKET;     /* Hint: MUST increase FD_SETSIZE */
                      }
#endif
                    for (;;)
                      {
                        struct timeval timeout;
                        fd_set fds;
                        register int r;
                        if (soap->connect_timeout > 0)
                          {
                            timeout.tv_sec = soap->connect_timeout;
                            timeout.tv_usec = 0;
                          }
                        else
                          {
                            timeout.tv_sec = -soap->connect_timeout / 1000000;
                            timeout.tv_usec =
                              -soap->connect_timeout % 1000000;
                          }
                        FD_ZERO (&fds);
                        FD_SET (fd, &fds);
                        if (err == SSL_ERROR_WANT_READ)
                          r =
                            select ((int) fd + 1, &fds, NULL, NULL, &timeout);
                        else
                          r =
                            select ((int) fd + 1, NULL, &fds, NULL, &timeout);
                        if (r >= 1)
                          {
                            r = 1;
                            break;
                          }
                        else
                          {
                            soap->errnum = 0;
                            DBGLOG (TEST,
                                    SOAP_MESSAGE (fdebug,
                                                  "Connect timeout\n"));
                            soap_set_sender_error (soap, "Timeout",
                                                   "connect failed in tcp_connect()",
                                                   SOAP_TCP_ERROR);
                            soap->fclosesocket (soap, fd);
                            return SOAP_INVALID_SOCKET;
                          }
                      }
                    continue;
                  }
              }
            break;
          }
        while (r == 1 && !SSL_is_init_finished (soap->ssl));
      /* Set SSL sockets to nonblocking */
      SOAP_SOCKNONBLOCK (fd)
#endif
        /* Check server credentials when required */
        if ((soap->ssl_flags & SOAP_SSL_REQUIRE_SERVER_AUTHENTICATION))
        {
          int err;
          if ((err = SSL_get_verify_result (soap->ssl)) != X509_V_OK)
            {
              soap_set_sender_error (soap,
                                     X509_verify_cert_error_string (err),
                                     "SSL certificate presented by peer cannot be verified in tcp_connect()",
                                     SOAP_SSL_ERROR);
              soap->fclosesocket (soap, fd);
              return SOAP_INVALID_SOCKET;
            }
          if (!(soap->ssl_flags & SOAP_SSL_SKIP_HOST_CHECK))
            {
              X509_NAME *subj;
              int ext_count;
              int ok = 0;
              X509 *peer;
              peer = SSL_get_peer_certificate (soap->ssl);
              if (!peer)
                {
                  soap_set_sender_error (soap, "SSL error",
                                         "No SSL certificate was presented by the peer in tcp_connect()",
                                         SOAP_SSL_ERROR);
                  soap->fclosesocket (soap, fd);
                  return SOAP_INVALID_SOCKET;
                }
              ext_count = X509_get_ext_count (peer);
              if (ext_count > 0)
                {
                  int i;
                  for (i = 0; i < ext_count; i++)
                    {
                      X509_EXTENSION *ext = X509_get_ext (peer, i);
                      const char *ext_str =
                        OBJ_nid2sn (OBJ_obj2nid
                                    (X509_EXTENSION_get_object (ext)));
                      if (ext_str && !strcmp (ext_str, "subjectAltName"))
                        {
                          X509V3_EXT_METHOD *meth = X509V3_EXT_get (ext);
                          void *ext_data;
#if (OPENSSL_VERSION_NUMBER >= 0x0090800fL)
                          const unsigned char *data;
#else
                          unsigned char *data;
#endif
                          STACK_OF (CONF_VALUE) * val;
                          int j;
                          if (!meth)
                            break;
                          data = ext->value->data;
#if (OPENSSL_VERSION_NUMBER > 0x00907000L)
                          if (meth->it)
                            ext_data =
                              ASN1_item_d2i (NULL, &data, ext->value->length,
                                             ASN1_ITEM_ptr (meth->it));
                          else
                            {   /* OpenSSL not perfectly portable at this point (?):
                                 * Some compilers appear to prefer
                                 * meth->d2i(NULL, (const unsigned char**)&data, ...
                                 * or
                                 * meth->d2i(NULL, &data, ext->value->length);
                                 */
                              ext_data =
                                meth->d2i (NULL, &data, ext->value->length);
                            }
#else
                          ext_data =
                            meth->d2i (NULL, &data, ext->value->length);
#endif
                          val = meth->i2v (meth, ext_data, NULL);
                          for (j = 0; j < sk_CONF_VALUE_num (val); j++)
                            {
                              CONF_VALUE *nval = sk_CONF_VALUE_value (val, j);
                              if (nval && !strcmp (nval->name, "DNS")
                                  && !strcmp (nval->value, host))
                                {
                                  ok = 1;
                                  break;
                                }
                            }
                        }
                      if (ok)
                        break;
                    }
                }
              if (!ok && (subj = X509_get_subject_name (peer)))
                {
                  int i = -1;
                  do
                    {
                      ASN1_STRING *name;
                      i =
                        X509_NAME_get_index_by_NID (subj, NID_commonName, i);
                      if (i == -1)
                        break;
                      name =
                        X509_NAME_ENTRY_get_data (X509_NAME_get_entry
                                                  (subj, i));
                      if (name)
                        {
                          if (!soap_tag_cmp (host, (const char *) name))
                            ok = 1;
                          else
                            {
                              unsigned char *tmp = NULL;
                              ASN1_STRING_to_UTF8 (&tmp, name);
                              if (tmp)
                                {
                                  if (!soap_tag_cmp
                                      (host, (const char *) tmp))
                                    ok = 1;
                                  OPENSSL_free (tmp);
                                }
                            }
                        }
                    }
                  while (!ok);
                }
              X509_free (peer);
              if (!ok)
                {
                  soap_set_sender_error (soap, "SSL error",
                                         "SSL certificate host name mismatch in tcp_connect()",
                                         SOAP_SSL_ERROR);
                  soap->fclosesocket (soap, fd);
                  return SOAP_INVALID_SOCKET;
                }
            }
        }
#else
      soap->fclosesocket (soap, fd);
      soap->error = SOAP_SSL_ERROR;
      return SOAP_INVALID_SOCKET;
#endif
    }
  return fd;
}

static SOAP_SOCKET
gsoap_lwip_faccept (struct soap *soap, SOAP_SOCKET s, struct sockaddr *a,
                    int *n)
{
  SOAP_SOCKET fd;

  WS4D_UNUSED_PARAM (soap);

  fd = accept (s, a, (SOAP_SOCKLEN_T *) n);     /* portability note: see SOAP_SOCKLEN_T definition in stdsoap2.h */
#ifdef SOCKET_CLOSE_ON_EXEC
#ifdef WIN32
#ifndef UNDER_CE
  SetHandleInformation ((HANDLE) fd, HANDLE_FLAG_INHERIT, 0);
#endif
#else
  fcntl (fd, F_SETFD, FD_CLOEXEC);
#endif
#endif
  return fd;
}

static int
gsoap_lwip_fclose (struct soap *soap)
{
#ifdef WITH_OPENSSL
  if (soap->ssl)
    {
      int r, s = 0;
      if (soap->session)
        {
          SSL_SESSION_free (soap->session);
          soap->session = NULL;
        }
      if (*soap->host)
        {
          soap->session = SSL_get1_session (soap->ssl);
          if (soap->session)
            {
              strcpy (soap->session_host, soap->host);
              soap->session_port = soap->port;
            }
        }
      r = SSL_shutdown (soap->ssl);
      if (r == 0)
        {
          if (soap_valid_socket (soap->socket))
            {
              struct timeval timeout;
              fd_set fd;
              if (soap->fshutdownsocket (soap, soap->socket, 1))
                {               /*
                                 * wait up to 10 seconds for close_notify to be sent by peer (if peer not
                                 * present, this avoids calling SSL_shutdown() which has a lengthy return
                                 * timeout)
                                 */
#ifndef WIN32
                  if ((int) soap->socket < (int) FD_SETSIZE)
                    {
#endif
                      timeout.tv_sec = 10;
                      timeout.tv_usec = 0;
                      FD_ZERO (&fd);
                      FD_SET (soap->socket, &fd);
                      r =
                        select ((int) soap->socket + 1, &fd, NULL, &fd,
                                &timeout);
                      if (r <= 0
                          && soap_socket_errno (soap->socket) != SOAP_EINTR)
                        {
                          soap->errnum = 0;
                          DBGLOG (TEST,
                                  SOAP_MESSAGE (fdebug,
                                                "Connection lost...\n"));
                          soap->fclosesocket (soap, soap->socket);
                          soap->socket = SOAP_INVALID_SOCKET;
                          ERR_remove_state (0);
                          return SOAP_OK;
                        }
#ifndef WIN32
                    }
#endif
                }
            }
          r = SSL_shutdown (soap->ssl);
        }
      if (r != 1)
        {
          s = ERR_get_error ();
          if (s)
            {
              DBGLOG (TEST,
                      SOAP_MESSAGE (fdebug, "Shutdown failed: %d\n",
                                    SSL_get_error (soap->ssl, r)));
              if (soap_valid_socket (soap->socket)
                  && !(soap->omode & SOAP_IO_UDP))
                {
                  soap->fclosesocket (soap, soap->socket);
                  soap->socket = SOAP_INVALID_SOCKET;
                }
            }
        }
      SSL_free (soap->ssl);
      soap->ssl = NULL;
      if (s)
        return SOAP_SSL_ERROR;
      ERR_remove_state (0);
    }
#endif
  if (soap_valid_socket (soap->socket) && !(soap->omode & SOAP_IO_UDP))
    {
      soap->fshutdownsocket (soap, soap->socket, 2);
      soap->fclosesocket (soap, soap->socket);
      soap->socket = SOAP_INVALID_SOCKET;
    }
  return SOAP_OK;
}

static int
gsoap_lwip_fclosesocket (struct soap *soap, SOAP_SOCKET fd)
{
  DBGLOG (TEST, SOAP_MESSAGE (fdebug, "Close socket %d\n", (int) fd));
  return soap_closesocket (fd);
}

static int
gsoap_lwip_fshutdownsocket (struct soap *soap, SOAP_SOCKET fd, int how)
{
  DBGLOG (TEST,
          SOAP_MESSAGE (fdebug, "Shutdown socket %d how=%d\n", (int) fd,
                        how));
  return shutdown (fd, how);
}

static int
gsoap_lwip_fpoll (struct soap *soap)
{
#ifndef WITH_LEAN
  struct timeval timeout;
  fd_set rfd, sfd, xfd;
  register int r;
#ifndef WIN32
  if ((int) soap->socket >= (int) FD_SETSIZE)
    return SOAP_FD_EXCEEDED;    /* Hint: MUST increase FD_SETSIZE */
#endif
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  FD_ZERO (&rfd);
  FD_ZERO (&sfd);
  FD_ZERO (&xfd);
  if (soap_valid_socket (soap->socket))
    {
      FD_SET (soap->socket, &rfd);
      FD_SET (soap->socket, &sfd);
      FD_SET (soap->socket, &xfd);
      r = select ((int) soap->socket + 1, &rfd, &sfd, &xfd, &timeout);
      if (r > 0 && FD_ISSET (soap->socket, &xfd))
        r = -1;
    }
  else if (soap_valid_socket (soap->master))
    {
      FD_SET (soap->master, &sfd);
      r = select ((int) soap->master + 1, NULL, &sfd, NULL, &timeout);
    }
  else
    return SOAP_OK;
  if (r > 0)
    {
#ifdef WITH_OPENSSL
      if (soap->imode & SOAP_ENC_SSL)
        {
          if (soap_valid_socket (soap->socket)
              && FD_ISSET (soap->socket, &sfd)
              && (!FD_ISSET (soap->socket, &rfd)
                  || SSL_peek (soap->ssl, soap->tmpbuf, 1) > 0))
            return SOAP_OK;
        }
      else
#endif
      if (soap_valid_socket (soap->socket)
            && FD_ISSET (soap->socket, &sfd)
            && (!FD_ISSET (soap->socket, &rfd)
                  || recv (soap->socket, soap->tmpbuf, 1, MSG_PEEK) > 0))
        return SOAP_OK;
    }
  else if (r < 0)
    {
      soap->errnum = soap_socket_errno (soap->master);
      if ((soap_valid_socket (soap->master)
           || soap_valid_socket (soap->socket))
          && soap_socket_errno (soap->master) != SOAP_EINTR)
        {
          soap_set_receiver_error (soap, tcp_error (soap),
                                   "select failed in soap_poll()",
                                   SOAP_TCP_ERROR);
          return soap->error = SOAP_TCP_ERROR;
        }
    }
  else
    soap->errnum = 0;
  DBGLOG (TEST,
          SOAP_MESSAGE (fdebug,
                        "Polling: other end down on socket=%d select=%d\n",
                        soap->socket, r));
  return SOAP_EOF;
#else
  return SOAP_OK;
#endif
}

SOAP_SOCKET
soap_bind (struct soap * soap, const char *host, int port, int backlog)
{
  //get Plugin from soap struct
  struct plugin_peer *mysoap = lwip_get_plugindata (soap);

#ifdef WITH_IPV6
  struct addrinfo *addrinfo = NULL;
  struct addrinfo hints;
  struct addrinfo res;
  int err;
#endif
#ifndef WITH_LEAN
  int len = SOAP_BUFLEN;
  int set = 1;
#endif
  if (soap_valid_socket (soap->master))
    {
      soap->fclosesocket (soap, soap->master);
      soap->master = SOAP_INVALID_SOCKET;
    }
  soap->socket = SOAP_INVALID_SOCKET;
  soap->errmode = 1;
  if (tcp_init (soap))
    {
      soap_set_receiver_error (soap, tcp_error (soap),
                               "TCP init failed in soap_bind()",
                               SOAP_TCP_ERROR);
      return SOAP_INVALID_SOCKET;
    }
#ifdef WITH_IPV6
  memset ((void *) &hints, 0, sizeof (hints));
  hints.ai_family = PF_UNSPEC;
#ifndef WITH_LEAN
  if ((soap->omode & SOAP_IO_UDP))
    hints.ai_socktype = SOCK_DGRAM;
  else
#endif
    hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  soap->errmode = 2;
  err = getaddrinfo (host, soap_int2s (soap, port), &hints, &addrinfo);
  if (addrinfo)
    {
      res = *addrinfo;
      memcpy (&soap->peer, addrinfo->ai_addr, addrinfo->ai_addrlen);
      soap->peerlen = addrinfo->ai_addrlen;
      res.ai_addr = (struct sockaddr *) &soap->peer;
      res.ai_addrlen = soap->peerlen;
      freeaddrinfo (addrinfo);
    }
  if (err || !addrinfo)
    {
      soap_set_receiver_error (soap, SOAP_GAI_STRERROR (err),
                               "getaddrinfo failed in soap_bind()",
                               SOAP_TCP_ERROR);
      return SOAP_INVALID_SOCKET;
    }
  soap->master =
    (int) socket (res.ai_family, res.ai_socktype, res.ai_protocol);
#else
#ifndef WITH_LEAN
  if ((soap->omode & SOAP_IO_UDP))
    soap->master = (int) lwip_socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  else
#endif
    soap->master = (int) lwip_socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif
  soap->errmode = 0;
  if (!soap_valid_socket (soap->master))
    {
      soap->errnum = soap_socket_errno (soap->master);
      soap_set_receiver_error (soap, tcp_error (soap),
                               "socket failed in soap_bind()",
                               SOAP_TCP_ERROR);
      return SOAP_INVALID_SOCKET;
    }
#ifndef WITH_LEAN
  if ((soap->omode & SOAP_IO_UDP))
    soap->socket = soap->master;
#endif
#ifdef SOCKET_CLOSE_ON_EXEC
#ifdef WIN32
#ifndef UNDER_CE
  SetHandleInformation ((HANDLE) soap->master, HANDLE_FLAG_INHERIT, 0);
#endif
#else
  fcntl (soap->master, F_SETFD, 1);
#endif
#endif
#ifndef WITH_LEAN

//  if (soap->bind_flags && setsockopt(soap->master, SOL_SOCKET, soap->bind_flags, (char*)&set, sizeof(int)))
//  { soap->errnum = soap_socket_errno(soap->master);
//    soap_set_receiver_error(soap, tcp_error(soap), "setsockopt failed in soap_bind()", SOAP_TCP_ERROR);
//    return SOAP_INVALID_SOCKET;
//  }
//  if (((soap->imode | soap->omode) & SOAP_IO_KEEPALIVE) && setsockopt(soap->master, SOL_SOCKET, SO_KEEPALIVE, (char*)&set, sizeof(int)))
//  { soap->errnum = soap_socket_errno(soap->master);
//    soap_set_receiver_error(soap, tcp_error(soap), "setsockopt SO_KEEPALIVE failed in soap_bind()", SOAP_TCP_ERROR);
//    return SOAP_INVALID_SOCKET;
//  }

#endif
#ifdef WITH_IPV6
  soap->errmode = 0;
  if (bind (soap->master, res.ai_addr, (int) res.ai_addrlen))
    {
      soap->errnum = soap_socket_errno (soap->master);
      DBGLOG (TEST, SOAP_MESSAGE (fdebug, "Could not bind to host\n"));
      soap_closesock (soap);
      soap_set_receiver_error (soap, tcp_error (soap),
                               "bind failed in soap_bind()", SOAP_TCP_ERROR);
      return SOAP_INVALID_SOCKET;
    }
#else
  soap->peerlen = sizeof (mysoap->peer);
  memset (&mysoap->peer, 0, sizeof (mysoap->peer));
  mysoap->peer.sin_family = AF_INET;
  soap->errmode = 2;
  if (host)
    {

      if (soap->fresolve (soap, host, &mysoap->peer.sin_addr))
        //  if (my_tcp_gethost(soap, host, &mysoap->peer.sin_addr))
        {
          soap_set_receiver_error (soap, tcp_error (soap),
                                   "get host by name failed in soap_bind()",
                                   SOAP_TCP_ERROR);
          return SOAP_INVALID_SOCKET;
        }
    }
  else
    mysoap->peer.sin_addr.s_addr = htonl (INADDR_ANY);
  mysoap->peer.sin_port = htons ((short) port);
  soap->errmode = 0;
  if (lwip_bind
      (soap->master, (struct sockaddr *) &mysoap->peer, (int) soap->peerlen))
    {
      soap->errnum = soap_socket_errno (soap->master);
      DBGLOG (TEST, SOAP_MESSAGE (fdebug, "Could not bind to host\n"));
      soap_closesock (soap);
      soap_set_receiver_error (soap, tcp_error (soap),
                               "bind failed in soap_bind()", SOAP_TCP_ERROR);
      return SOAP_INVALID_SOCKET;
    }
#endif
  if (!(soap->omode & SOAP_IO_UDP) && lwip_listen (soap->master, backlog))
    {
      soap->errnum = soap_socket_errno (soap->master);
      DBGLOG (TEST, SOAP_MESSAGE (fdebug, "Could not bind to host\n"));
      soap_closesock (soap);
      soap_set_receiver_error (soap, tcp_error (soap),
                               "listen failed in soap_bind()",
                               SOAP_TCP_ERROR);
      return SOAP_INVALID_SOCKET;
    }
  return soap->master;
}

SOAP_SOCKET
soap_accept (struct soap * soap)
{
  /* get Plugin from soap struct */
  struct plugin_peer *mysoap = lwip_get_plugindata (soap);

  int n = (int) sizeof (mysoap->peer);
#ifndef WITH_LEAN
  int len = SOAP_BUFLEN;
  int set = 1;
#endif
  soap->error = SOAP_OK;
#ifndef WITH_LEAN
  if ((soap->omode & SOAP_IO_UDP))
    return soap->socket = soap->master;
#endif
  memset ((void *) &mysoap->peer, 0, sizeof (mysoap->peer));
  soap->socket = SOAP_INVALID_SOCKET;
  soap->errmode = 0;
  soap->keep_alive = 0;
  if (soap_valid_socket (soap->master))
    {
      register int err;
      for (;;)
        {
#ifndef WITH_LEAN
          if (soap->accept_timeout || soap->send_timeout
              || soap->recv_timeout)
            {
#ifndef WIN32
              if ((int) soap->socket >= (int) FD_SETSIZE)
                {
                  soap->error = SOAP_FD_EXCEEDED;
                  return SOAP_INVALID_SOCKET;   /* Hint: MUST increase FD_SETSIZE */
                }
#endif
              for (;;)
                {
                  struct timeval timeout;
                  fd_set fd;
                  register int r;
                  if (soap->accept_timeout > 0)
                    {
                      timeout.tv_sec = soap->accept_timeout;
                      timeout.tv_usec = 0;
                    }
                  else if (soap->accept_timeout < 0)
                    {
                      timeout.tv_sec = -soap->accept_timeout / 1000000;
                      timeout.tv_usec = -soap->accept_timeout % 1000000;
                    }
                  else
                    {
                      timeout.tv_sec = 60;
                      timeout.tv_usec = 0;
                    }
                  FD_ZERO (&fd);
                  FD_SET (soap->master, &fd);
                  r =
                    select ((int) soap->master + 1, &fd, &fd, &fd, &timeout);
                  if (r > 0)
                    break;
                  if (!r && soap->accept_timeout)
                    {
                      soap->errnum = 0;
                      soap_set_receiver_error (soap, "Timeout",
                                               "accept failed in soap_accept()",
                                               SOAP_TCP_ERROR);
                      return SOAP_INVALID_SOCKET;
                    }
                  if (r < 0)
                    {
                      r = soap_socket_errno (soap->master);
                      if (r != SOAP_EINTR)
                        {
                          soap->errnum = r;
                          soap_closesock (soap);
                          soap_set_sender_error (soap, tcp_error (soap),
                                                 "accept failed in soap_accept()",
                                                 SOAP_TCP_ERROR);
                          return SOAP_INVALID_SOCKET;
                        }
                    }
                }
            }
          if (soap->accept_timeout || soap->send_timeout
              || soap->recv_timeout)
            SOAP_SOCKNONBLOCK (soap->master)
            else
            SOAP_SOCKBLOCK (soap->master)
#endif
              soap->socket =
              soap->faccept (soap, soap->master,
                             (struct sockaddr *) &mysoap->peer, &n);
          soap->peerlen = (size_t) n;
          if (soap_valid_socket (soap->socket))
            {
#ifdef WITH_IPV6
              /* Use soap->host to store the numeric form of the remote host */
              getnameinfo ((struct sockaddr *) &soap->peer, n, soap->host,
                           sizeof (soap->host), NULL, 0,
                           NI_NUMERICHOST | NI_NUMERICSERV);
              DBGLOG (TEST,
                      SOAP_MESSAGE (fdebug, "Accept socket %d from %s\n",
                                    soap->socket, soap->host));
              soap->ip = 0;     /* info stored in soap->peer and soap->host */
              soap->port = 0;   /* info stored in soap->peer and soap->host */
#else
              soap->ip = ntohl (mysoap->peer.sin_addr.s_addr);
              soap->port = (int) ntohs (mysoap->peer.sin_port); /* does not return port number on some systems */
              DBGLOG (TEST,
                      SOAP_MESSAGE (fdebug,
                                    "Accept socket %d at port %d from IP %d.%d.%d.%d\n",
                                    soap->socket, soap->port,
                                    (int) (soap->ip >> 24) & 0xFF,
                                    (int) (soap->ip >> 16) & 0xFF,
                                    (int) (soap->ip >> 8) & 0xFF,
                                    (int) soap->ip & 0xFF));
#endif
#ifndef WITH_LEAN
              if (soap->accept_flags == SO_LINGER)
                {
                  struct linger linger;
                  memset ((void *) &linger, 0, sizeof (linger));
                  linger.l_onoff = 1;
                  linger.l_linger = soap->linger_time;
                  if (setsockopt
                      (soap->socket, SOL_SOCKET, SO_LINGER, (char *) &linger,
                       sizeof (struct linger)))
                    {
                      soap->errnum = soap_socket_errno (soap->socket);
                      soap_set_receiver_error (soap, tcp_error (soap),
                                               "setsockopt SO_LINGER failed in soap_accept()",
                                               SOAP_TCP_ERROR);
                      soap_closesock (soap);
                      return SOAP_INVALID_SOCKET;
                    }
                }
              else if (soap->accept_flags
                       && setsockopt (soap->socket, SOL_SOCKET,
                                      soap->accept_flags, (char *) &set,
                                      sizeof (int)))
                {
                  soap->errnum = soap_socket_errno (soap->socket);
                  soap_set_receiver_error (soap, tcp_error (soap),
                                           "setsockopt failed in soap_accept()",
                                           SOAP_TCP_ERROR);
                  soap_closesock (soap);
                  return SOAP_INVALID_SOCKET;
                }
              if (((soap->imode | soap->omode) & SOAP_IO_KEEPALIVE)
                  && setsockopt (soap->socket, SOL_SOCKET, SO_KEEPALIVE,
                                 (char *) &set, sizeof (int)))
                {
                  soap->errnum = soap_socket_errno (soap->socket);
                  soap_set_receiver_error (soap, tcp_error (soap),
                                           "setsockopt SO_KEEPALIVE failed in soap_accept()",
                                           SOAP_TCP_ERROR);
                  soap_closesock (soap);
                  return SOAP_INVALID_SOCKET;
                }
#ifdef TCP_NODELAY
              if (!(soap->omode & SOAP_IO_UDP)
                  && setsockopt (soap->socket, IPPROTO_TCP, TCP_NODELAY,
                                 (char *) &set, sizeof (int)))
                {
                  soap->errnum = soap_socket_errno (soap->socket);
                  soap_set_receiver_error (soap, tcp_error (soap),
                                           "setsockopt TCP_NODELAY failed in soap_accept()",
                                           SOAP_TCP_ERROR);
                  soap_closesock (soap);
                  return SOAP_INVALID_SOCKET;
                }
#endif
#endif
              soap->keep_alive =
                (((soap->imode | soap->omode) & SOAP_IO_KEEPALIVE) != 0);
              return soap->socket;
            }
          err = soap_socket_errno (soap->socket);
          if (err != 0 && err != SOAP_EINTR && err != SOAP_EAGAIN
              && err != SOAP_EWOULDBLOCK)
            {
              DBGLOG (TEST,
                      SOAP_MESSAGE (fdebug, "Accept failed from %s\n",
                                    soap->host));
              soap->errnum = err;
              soap_set_receiver_error (soap, tcp_error (soap),
                                       "accept failed in soap_accept()",
                                       SOAP_TCP_ERROR);
              soap_closesock (soap);
              return SOAP_INVALID_SOCKET;
            }
        }
    }
  else
    {
      soap->errnum = 0;
      soap_set_receiver_error (soap, tcp_error (soap),
                               "no master socket in soap_accept()",
                               SOAP_TCP_ERROR);
      return SOAP_INVALID_SOCKET;
    }
}

int
wsd_bind_multicastudp (struct soap *soap, const char *host,
                       const char *group, short int port, int backlog)
{
  SOAP_SOCKET fd;
  int err = 0;
#ifdef WITH_IPV6
  struct ipv6_mreq mreq6;
#endif
  struct ip_mreq mreq;

  /* test parameters */
  if (!soap || !host || !group)
    return SOAP_ERR;

  /* to let several instances of discovery work on the same */
  /* discovery multicast socket */
  soap->bind_flags = SO_REUSEPORT;

  /* same procedure as with unicast socket */
  err = wsd_bind_udp (soap,
#ifdef WITH_IPV6
                      ws4d_is_ipv4 (host) ? "0.0.0.0" : "::",
#else
                      "0.0.0.0",
#endif
                      port, backlog);
  if (err != SOAP_OK)
    return err;

  fd = soap->master;

  /* add to ws discovery multicast group */
#ifdef WITH_IPV6
  if (ws4d_is_ipv4 (host))
    {
#endif
      memset (&mreq, 0, sizeof (mreq));
      ws4d_inet_pton (AF_INET, group, &mreq.imr_multiaddr.s_addr);
      ws4d_inet_pton (AF_INET, host, &mreq.imr_interface.s_addr);
#ifdef WITH_IPV6
    }
  else
    {
      memset (&mreq6, 0, sizeof (mreq6));
      ws4d_inet_pton (AF_INET6, group, &mreq6.ipv6mr_multiaddr.s6_addr);

      /* TODO: specify interface id here */
      mreq6.ipv6mr_interface = 0;
    }
#endif

#ifdef WITH_IPV6
  if (ws4d_is_ipv4 (host))
    {
#endif
      if (lwip_setsockopt (fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &mreq,
                           sizeof (mreq)) < 0)
        {
          soap->fclosesocket (soap, (SOAP_SOCKET) fd);
          return SOAP_PLUGIN_ERROR;
        }
#ifdef WITH_IPV6
    }
  else
    {
      if (lwip_setsockopt (fd, IPPROTO_IPV6, IPV6_JOIN_GROUP, (void *) &mreq6,
                           sizeof (mreq6)) < 0)
        {
          perror ("setsockopt in wsd_bind_multicastudp()");
          soap->fclosesocket (soap, (SOAP_SOCKET) fd);
          return SOAP_PLUGIN_ERROR;
        }
    }
#endif

  return SOAP_OK;
}

int
gsoap_lwip_init (struct netif *netifp)
{
  ws4d_assert (netifp, SOAP_ERR);

  if (netif_is_up (netifp))
    {
      netifp = netifp;
      return SOAP_OK;
    }
  else
    {
      return SOAP_ERR;
    }
}

static void
lwip_plugin_setio (struct soap *soap)
{
  /* Init function-pointers of gSOAP */
  soap->fsend = gsoap_lwip_fsend;
  soap->frecv = gsoap_lwip_frecv;
  soap->fopen = gsoap_lwip_fopen;
  soap->fclose = gsoap_lwip_fclose;
  soap->fclosesocket = gsoap_lwip_fclosesocket;
  soap->fshutdownsocket = gsoap_lwip_fshutdownsocket;
  soap->fpoll = gsoap_lwip_fpoll;
  soap->faccept = gsoap_lwip_faccept;
  soap->fresolve = my_tcp_gethost;
}

int
gsoap_lwip_register_handle (struct soap *soap)
{
  return soap_register_plugin (soap, lwip_plugin_init);
}
