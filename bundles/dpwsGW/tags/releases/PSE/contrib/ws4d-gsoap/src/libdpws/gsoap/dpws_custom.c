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
#include "soap_misc.h"

#include "dpwsH.h"
#include <stdarg.h>


void
soap_default_wsdp__QNameListType (struct soap *soap, char **a)
{
  soap_default_string (soap, a);
}

void
soap_serialize_wsdp__QNameListType (struct soap *soap, char *const *a)
{
  soap_serialize_string (soap, a);
}

int
soap_out_wsdp__QNameListType (struct soap *soap, const char *tag,
                              int id, char *const *a, const char *type)
{
  char **outstring = NULL;

  if (!(outstring = (char **) soap_malloc (soap, sizeof (char *))))
    return SOAP_EOM;
  *outstring = NULL;

  if (soap_out_transform_qnames (soap, outstring, a))
    return soap->error;

  return soap_outstring (soap, tag, id, outstring, type,
                         SOAP_TYPE_wsdp__QNameListType);
}

char **
soap_in_wsdp__QNameListType (struct soap *soap,
                             const char *tag, char **a, const char *type)
{
  char **instring = NULL;

  if (soap_element_begin_in (soap, tag, 1, type))
    {
      if (!tag || *tag != '-' || soap->error != SOAP_NO_TAG)
        return NULL;
      soap->error = SOAP_OK;
    }

  if (!a)
    if (!(a = (char **) soap_malloc (soap, sizeof (char *))))
      return NULL;

  if (!(instring = (char **) soap_malloc (soap, sizeof (char *))))
    return NULL;

  if (soap->body)
    {
      *instring = soap_string_in (soap, 1, -1, -1);
      if (!instring
          || !(char *) soap_id_enter (soap, soap->id, *instring,
                                      SOAP_TYPE_wsdp__QNameListType,
                                      sizeof (char *), 0, NULL, NULL, NULL))
        return NULL;
    }

  if (*soap->href)
    instring =
      (char **) soap_id_lookup (soap, soap->href, (void **) instring,
                                SOAP_TYPE_wsdp__QNameListType,
                                sizeof (char **), 0);

  if (soap_in_expand_qnames (soap, instring, a))
    return NULL;

  if (soap->body && soap_element_end_in (soap, tag))
    return NULL;

  return a;
}

void
soap_default__wsdp__Types (struct soap *soap, char **a)
{
  soap_default_string (soap, a);
}

void
soap_serialize__wsdp__Types (struct soap *soap, char *const *a)
{
  soap_serialize_string (soap, a);
}

int
soap_out__wsdp__Types (struct soap *soap, const char *tag, int id,
                       char *const *a, const char *type)
{
  return soap_outstring (soap, tag, id, a, type, SOAP_TYPE__wsdp__Types);
}

char **
soap_in__wsdp__Types (struct soap *soap, const char *tag,
                      char **a, const char *type)
{
  return soap_instring (soap, tag, a, type, SOAP_TYPE__wsdp__Types, 1, -1,
                        -1);
}
