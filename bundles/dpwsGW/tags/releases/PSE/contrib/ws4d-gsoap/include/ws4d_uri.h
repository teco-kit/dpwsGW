/*
 * C Interface: uricmp_rfc2396
 *
 * Author: Sascha Feldhorst <sascha.feldhorst@udo.edu>, (C) 2008
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 */

#ifndef URICMP_RFC2396
#define URICMP_RFC2396

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ws4d_misc.h"

/**
 * @addtogroup WS4D_URI Functions for URI parsing and matching
 * @ingroup WS4D_UTILS
 *
 * @{
 */

/* Flags used to configure the ws4d_uri parser. */
#define WITH_SCHEME 1
#define WITH_USERINFO 2
#define WITH_HOST 4
#define WITH_PORT 8
#define WITH_PATH 16
#define WITH_QUERY 32
#define WITH_FRAGMENT 64
#define WITH_ALL 127


/** Uri struct for a convenient use of ws4d_uri's conform with rfc2396. */
struct ws4d_uri
{
  char *scheme; /**< ws4d_uri scheme, e.g. http */
  char *userinfo; /**< optional user information, e.g. username */
  char *host; /**< ipv4 address or hostname, e.g. schemas.xmlsoap.org */
  char *port; /**< optional port number, e.g. 80 */
  char *path; /**< optional hierarchical path, e.g. /ws/2005/04/discovery */
  char *query; /**< optional query, e.g. test=1 */
  char *fragment; /**< optional fragment, e.g. anchor1 */
  ws4d_alloc_list alist;
};

/**
 * Function to initialize a ws4d_uri structure. This functions
 * expects all unused fields to be set to NULL. Otherwise undefined behavior
 * is yielded.
 *
 * \param u Pointer to the struct, that shall be initialized.
 */
int ws4d_uri_init (struct ws4d_uri *u);

/**
 * Function to free all memory allocated for a specified ws4d_uri. This functions
 * expects all unused fields to be set to NULL. Otherwise undefined behavior
 * is yielded.
 *
 * \param u Pointer to the struct, that shall be freed.
 */
void ws4d_uri_done (struct ws4d_uri *u);

/* Helper functions for the parsing process */
/**
 * Function indicates whether a specified string is an escaped ws4d_uri character.
 *
 * @code
 * escaped = "%" hex hex
 * hex     = digit | "A" | "B" | "C" | "D" | "E" | "F" | "a" | "b" | "c" | "d" |
 *           "e" | "f"
 * digit   = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
 * @endcode

 * @param s Null terminated string that shall be checked.
 * @return SOAP_OK if the specified string is a escaped ws4d_uri character, SOAP_ERR otherwise.
 */
int ws4d_is_escaped (const char *s);

/**
 * Function indicates whether a specified string is a valid hostname or not.
 *
 * @code
 * hostname   = *( domainname "." ) toplabel[ "." ]
 * domainname = alphanum | alphanum * ( alphanum | "-" ) alphanum
 * toplabel   = alpha | alpha * ( alphanum | "-" ) alphanum
 * @endcode
 *
 * @param s Null terminated string that shall be checked.
 * @return SOAP_OK if the specified string is a valid hostname, SOAP_ERR otherwise.
 */
int ws4d_is_hostname (const char *s);

/**
 * Function indicates whether a specified string is a valid ipv4 address or not.
 *
 * @code
 * IPv4address = 1 * digit "." 1 * digit "." 1 * digit "." 1 * digit
 * @endcode
 *
 * @param s Null terminated string that shall be checked.
 * @return SOAP_OK if the specified string is a valid ipv4 addr, SOAP_ERR otherwise.
 */
int ws4d_is_ipv4 (const char *s);

/**
 * Function indicates whether a specified string is a valid ipv6 address or not.
 *
 * @param s Null terminated string that shall be checked.
 * @return SOAP_OK if the specified string is a valid ipv6 addr, SOAP_ERR otherwise.
 */
int ws4d_is_ipv6 (const char *s);

/**
 * Function indicates whether a specified char is a mark character or not.
 *
 * @code
 * mark = "-" | "_" | "." | "!" | "~" | "*" | "'" | "(" | ")"
 * @endcode
 *
 * @param c Character that shall be checked.
 * @return SOAP_OK if the specified char is a mark, SOAP_ERR otherwise.
 */
int ws4d_is_urimark (char c);

/**
 * Function indicates whether a specified string is a valid path segment or not.
 *
 * @code
 * segment      = *pchar * ( ";" param )
 * param        = *pchar
 * pchar        = unreserved | escaped | ":" | "@" | "&" | "=" | "+" | "$" | ","
 * @endcode
 *
 * @todo Test if param's work.
 * @param s Null terminated string that shall be checked.
 * @return SOAP_OK if the specified string is a valid path segment, SOAP_ERR otherwise.
 */
int ws4d_is_urisegment (const char *s);

/**
 * Function indicates whether a specified char is an unreserved character or not.
 *
 * @code
 * unreserved = alphanum | mark
 * @endcode
 *
 * @param c Character that shall be checked.
 * @return SOAP_OK if the specified char is unreserved, SOAP_ERR otherwise.
 */
int ws4d_is_uriunreserved (char c);

/**
 * Function indicates whether a specified char is from set uric or not.
 *
 * @code
 * uric = reserved | unreserved | escaped
 * @endcode
 *
 * @param s Character that shall be checked.
 * @return SOAP_OK if the specified char from uric, SOAP_ERR otherwise.
 */
int ws4d_is_uric (const char *s);

/**
 * Function indicates whether a specified char occurs in a specified null
 * terminated string, which is also specified.
 *
 * @code
 * if (ws4d_is_notin (c, "$%&"))
 * { }
 * @endcode
 *
 * @param c Character that shall be checked.
 * @param s Set of characters c is compared with.
 * @return SOAP_OK if the specified char from uric, SOAP_ERR otherwise.
 */
int ws4d_is_notin (char c, const char *s);

/**
 * Function indicates whether a specified null terminated string is a valid
 * ws4d_uri scheme according to the syntax from the specification.
 *
 * @code
 * scheme = alpha * ( alpha | digit | "+" | "-" | "." )
 * @endcode
 *
 * @param s String representation of the scheme, that shall be validated.
 * @return SOAP_OK if specified scheme is well-formed, SOAP_ERR otherwise.
 */
int ws4d_uri_validate_scheme (const char *s);

/**
 * Function indicates whether a specified null terminated string is a valid
 * ws4d_uri userinfo according to the syntax from the specification.
 *
 * @code
 * userinfo = * ( unreserved | escaped | ";" | ":" | "&" | "=" | "+" | "$" | "," )
 * @endcode
 *
 * @param s String representation of the userinfo, that shall be validated.
 * @return SOAP_OK if specified userinfo is well-formed, SOAP_ERR otherwise.
 */
int ws4d_uri_validate_userinfo (const char *s);

/**
 * Function indicates whether a specified null terminated string is a valid
 * ws4d_uri host according to the syntax from the specification.
 *
 * @code
 * host = hostname | IPv4address
 * @endcode
 *
 * @param s String representation of the host, that shall be validated.
 * @return SOAP_OK if specified host is well-formed, SOAP_ERR otherwise.
 */
int ws4d_uri_validate_host (const char *s);

/**
 * Function indicates whether a specified null terminated string is a valid
 * ws4d_uri port according to the syntax from the specification.
 *
 * @code
 * port = *digit
 * @endcode
 *
 * @param s String representation of the port, that shall be validated.
 * @return SOAP_OK if specified port is well-formed, SOAP_ERR otherwise.
 */
int ws4d_uri_validate_port (const char *s);

/**
 * Function indicates whether a specified null terminated string is a valid
 * ws4d_uri path according to the syntax from the specification.
 *
 * @code
 * path     = [ abs_path | opaque_part ]
 * abs_path = segment *( "/" segment )
 * @endcode
 *
 * @note Only abs_path's are supported at the moment.
 * @param s String representation of the path, that shall be validated.
 * @return SOAP_OK if specified path is well-formed, SOAP_ERR otherwise.
 */
int ws4d_uri_validate_path (const char *s);

/**
 * Function indicates whether a specified null terminated string is a valid
 * ws4d_uri query according to the syntax from the specification.
 *
 * @code
 * query = *uric
 * @endcode
 *
 * @param s String representation of the query, that shall be validated.
 * @return SOAP_OK if specified query is well-formed, SOAP_ERR otherwise.
 */
int ws4d_uri_validate_query (const char *s);

/**
 * Function indicates whether a specified null terminated string is a valid
 * ws4d_uri fragment according to the syntax from the specification.
 *
 * @code
 * fragment = *uric
 * @endcode
 *
 * @param s String representation of the fragment, that shall be validated.
 * @return SOAP_OK if specified fragment is well-formed, SOAP_ERR otherwise.
 */
int ws4d_uri_validate_fragment (const char *s);

/**
 * <p>Function to parse the string representation of an ws4d_uri in to a structure
 * for a more convenient handling. The caller can influence which parts of the
 * ws4d_uri are recognized during the parsing. To indicate this the flag parameter
 * can be used.</p>
 *
 * <p>Supported flags:</p>
 * <ul>
 *   <li>WITH_SCHEME</li>
 *   <li>WITH_USERINFO</li>
 *   <li>WITH_HOST</li>
 *   <li>WITH_PORT</li>
 *   <li>WITH_PATH</li>
 *   <li>WITH_QUERY</li>
 *   <li>WITH_FRAGMENT</li>
 *   <li>WITH_ALL</li>
 * </ul>
 * <p>To pass the flags combine them with the bitwise OR operator( | ), e.g.</p>
 * @code ws4d_parse_uri (ws4d_uri, res, WITH_SCHEME | WITH_PORT); @endcode
 *
 * <p><strong>Notes:</strong></p>
 * <ul>
 *   <li>All fields of the result struct are set to NULL initially, so
 *       previously stored information are lost and memory can be leaked.</li>
 *   <li>All ws4d_uri parts that were not found in the specified string are set to
 *       NULL, even if this</li>
 *   <li>This function allocates memory with calloc, so don't forget to free a
 *       result struct after usage. Use ws4d_uri_done for that.</li>
 * </ul>
 *
 * @param uri String representation of the ws4d_uri, that shall be parsed.
 * @param result Pointer to the struct where the result is stored
 * @param flags Variable which stores the
 *
 * @return SOAP_OK if the ws4d_uri was well-formed or SOAP_ERR otherwise.
 */
int ws4d_parse_uri (const char *uri, struct ws4d_uri *result, int flags);

/**
 * <p>This function implements the http://schemas.xmlsoap.org/ws/2005/04/discovery/rfc2396
 * probe matching rule from ws-discovery, which is an obligatory part of every
 * dpws device. It is defined in chapter 5.1 of the ws-discovery specification
 * and accepts ws4d_uri according to rfc 2396.</p>
 *
 * <p>To compare the specified ws4d_uri's the ws4d ws4d_uri parsing logic is used. If one
 * or both ws4d_uri's are malformed they do not match.</p>
 *
 * <p><strong>Note:</strong> The relationship between s1 and s2 is not symmetric;
 * if s1 matches s2 this implies not that s2 also matches s1. This is due to the
 * fact, that the spec requires the path of s1 to be a segment wise prefix of the
 * path of s2. To ensure a proper matching the shorter ws4d_uri should be used as s1.
 * </p>
 *
 * @param s1 First ws4d_uri that shall be compared.
 * @param s2 Second ws4d_uri that shall be compared.
 *
 * @return <ul>
 *           <li>SOAP_OK if ws4d_uri's match</li>
 *           <li>SOAP_SYNTAX_ERROR if s1 or s2 is malformed</li>
 *           <li>SOAP_ERR if ws4d_uri's do not match</li>
 *         </ul>
 */
int ws4d_cmp_uri (const char *s1, const char *s2);

/**
 * TODO: add documentation
 */
int ws4d_uri_tostrlen (struct ws4d_uri *res);

/**
 * TODO: add documentation
 */
int ws4d_uri_tostr (struct ws4d_uri *res, char *uri, size_t size);

/** @} */

#endif
