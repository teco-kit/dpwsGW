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

#ifndef _WSADDRESSING_H_
#define _WSADDRESSING_H_

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif

/**
 * gSoap WS-Addressing Plugin API
 *
 * @addtogroup ADDRESSING_API Addressing-API
 * @ingroup DPWS_WS_MODULES
 *
 * @{
 */

#include "soap_misc.h"

#include "ws4d_epr.h"

/**
 * @addtogroup APIMIHHandling Message information header handling functions
 * @ingroup ADDRESSING_API
 *
 * @{
 */


extern const char *wsa_faultAction;
extern const char *wsa_anonymousURI;

int wsa_register_handle (struct soap *soap);
struct message_id_s
{
  struct ws4d_list_node list;
  int valid;
  char *msg_id;
};
int wsa_message_isdup (struct soap *soap);
int wsa_header_set_To (struct soap *soap, const char *To);
char *wsa_header_get_To (struct soap *soap);
int wsa_header_set_From (struct soap *soap, const char *Address);
char *wsa_header_get_From (struct soap *soap);
int wsa_header_set_MessageId (struct soap *soap, const char *Id);
int wsa_header_check_MessageId (struct soap *soap);
char *wsa_header_get_MessageId (struct soap *soap);
int wsa_header_set_Action (struct soap *soap, const char *Action);
char *wsa_header_get_Action (struct soap *soap);
int wsa_header_set_RelatesTo (struct soap *soap, const char *MessageId);
char *wsa_header_get_RelatesTo (struct soap *soap);
int wsa_header_set_ReplyTo (struct soap *soap, const char *Address);
char *wsa_header_get_ReplyTo (struct soap *soap);
int wsa_header_set_FaultTo (struct soap *soap, const char *Address);
char *wsa_header_get_FaultTo (struct soap *soap);

int wsa_header_gen_oneway (struct soap *soap, const char *MessageId,
                           const char *To, const char *Action,
                           const char *FaultTo, size_t size);

int wsa_header_gen_request (struct soap *soap, const char *MessageId,
                            const char *To, const char *Action,
                            const char *FaultTo, const char *ReplyTo,
                            size_t size);

int wsa_response (struct soap *soap, const char *MessageId,
                             const char *To, const char *Action,
                             const char *RelatesTo, size_t size);

int wsa_header_gen_response (struct soap *soap, const char *MessageId,
                             const char *To, const char *Action,
                             const char *RelatesTo, size_t size);

int wsa_header_gen_fault (struct soap *soap, const char *MessageId,
                          const char *To, const char *Action,
                          const char *RelatesTo, size_t size);

int wsa_header_check_request (struct soap *soap);

int wsa_header_check_response (struct soap *soap);
/** @} */

/**
 * @addtogroup APIFaultHandling Fault handling functions
 * @ingroup ADDRESSING_API
 *
 * @{
 */
int wsa_sender_fault_subcode (struct soap *soap, const char *faultsubcode,
                              const char *faultstring,
                              const char *faultdetail);

int wsa_receiver_fault_subcode (struct soap *soap, const char *faultsubcode,
                                const char *faultstring,
                                const char *faultdetail);
/** @} */

/** @} */

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif /* _WSADDRESSING_H_ */
