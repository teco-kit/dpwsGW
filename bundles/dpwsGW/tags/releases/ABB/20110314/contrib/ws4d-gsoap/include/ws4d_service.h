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

#ifndef WS4D_SERVICE_H_
#define WS4D_SERVICE_H_

#include "ws4d_mutex.h"
#include "ws4d_epr.h"
#include "ws4d_qname.h"

/**
 * Service Endpoint Reference
 *
 * @addtogroup APIServiceEP Service Endpoint
 * @ingroup WS4D_UTILS
 *
 *
 * @{
 */

/**
 * Registers the service plugin at an epr
 *
 * @code
 * struct ws4d_epr epr;
 *
 * ws4d_epr_init(&epr);
 * ws4d_register_serviceep(&epr);
 * ws4d_epr_reset(&epr);
 * @endcode
 *
 * @param epr epr to register serviceep plugin at
 * @return WS4D_OK on success, an error code otherwise
 */
int ws4d_register_serviceep (struct ws4d_epr *epr);

/**
 * Adds a type to a service
 *
 * @code
 * struct ws4d_epr epr;
 * struct ws4d_qname *qname = {{NULL, NULL}, NULL, "exampletype1", "http://www.ws4d.org/ns/examplens1"};
 *
 * ws4d_epr_init(&epr);
 * ws4d_register_serviceep(&epr);
 *
 * ws4d_serviceep_addtype(&epr, type);
 *
 * ws4d_epr_reset(&epr);
 * @endcode
 *
 * @param service service to add a type
 * @param type service type to add as struct ws4d_qname
 *
 * @return WS4D_OK on success, an error code otherwise
 */
int ws4d_serviceep_addtype (struct ws4d_epr *service,
                            struct ws4d_qname *type);

/**
 * Adds types passed in string representation to a service
 *
 * @code
 * struct ws4d_epr epr;
 * char *qname_list = "\"http://www.ws4d.org/ns/examplens1\":exampletype1 \"http://www.ws4d.org/ns/examplens2\":exampletype2";
 *
 * ws4d_epr_init(&epr);
 * ws4d_register_serviceep(&epr);
 *
 * ws4d_serviceep_addtypestr(&epr, qname_list);
 *
 * ws4d_epr_reset(&epr);
 * @endcode
 *
 * @param service service to add types
 * @param types to add as white space separated list
 *
 * @return WS4D_OK on success, an error code otherwise
 */
int ws4d_serviceep_addtypestr (struct ws4d_epr *service, const char *types);

/**
 * Returns to qname list of the types of a service
 *
 * @code
 * struct ws4d_epr epr;
 * char *qname_list = "\"http://www.ws4d.org/ns/examplens1\":exampletype1 \"http://www.ws4d.org/ns/examplens2\":exampletype2";
 * register struct ws4d_qname *cur, *iter;
 *
 * ws4d_epr_init(&epr);
 * ws4d_register_serviceep(&epr);
 *
 * ws4d_serviceep_addtypestr(&epr, qname_list);
 * ws4d_qname_foreach(cur, iter, ws4d_serviceep_gettypelist(epr))
 * {
 *   printf("Type: (%s):%s\n", ws4d_qname_getNS(cur), ws4d_qname_getName(cur));
 * }
 *
 * ws4d_epr_reset(&epr);
 * @endcode
 *
 * @param service service to get list of types
 *
 * @return qname list on success, NULL otherwise
 */
ws4d_qnamelist *ws4d_serviceep_gettypelist (struct ws4d_epr *service);

/**
 * Set the ID of an service
 *
 * @code
 * struct ws4d_epr epr;
 *
 * ws4d_epr_init(&epr);
 * ws4d_register_serviceep(&epr);
 *
 * ws4d_serviceep_setid(&epr, "http://www.ws4d.org/srvs/ExampleService1");
 *
 * ws4d_epr_reset(&epr);
 * @endcode
 *
 * @param service service to get id
 * @param id id to set
 *
 * @return WS4D_OK on success, an error code otherwise
 */
int ws4d_serviceep_setid (struct ws4d_epr *service, const char *id);

/**
 * Get the ID of an service
 *
 * @code
 * struct ws4d_epr epr;
 *
 * ws4d_epr_init(&epr);
 * ws4d_register_serviceep(&epr);
 * ws4d_serviceep_setid(&epr, "http://www.ws4d.org/srvs/ExampleService1");
 *
 * printf("ServiceID: %s\n", ws4d_serviceep_getid(epr));
 *
 * ws4d_epr_reset(&epr);
 * @endcode
 *
 * @param service service to get id
 *
 * @return ID of service on success, or NULL otherwise
 */
char *ws4d_serviceep_getid (struct ws4d_epr *service);

/**
 * Match types with types of an service
 *
 * @code
 * struct ws4d_epr epr;
 * char *qname_list = "\"http://www.ws4d.org/ns/examplens1\":exampletype1 \"http://www.ws4d.org/ns/examplens2\":exampletype2";
 *
 * ws4d_epr_init(&epr);
 * ws4d_register_serviceep(&epr);
 * ws4d_serviceep_addtypestr(&epr, qname_list);
 *
 * if (ws4d_serviceep_matchesTypes(&epr, "\"http://www.ws4d.org/ns/examplens1\":exampletype1")
 * {
 *   printf("Types are matching\n");
 * }
 * else
 * {
 *   printf("Types are NOT matching\n");
 * }
 *
 * ws4d_epr_reset(&epr);
 * @endcode
 *
 * @param service service to match types
 * @param types types to match
 *
 * @return returns 1 on match, 0 otherwise
 */
int ws4d_serviceep_matchesTypes (struct ws4d_epr *service,
                                 ws4d_qnamelist * types);

/** @} */

#endif /*WS4D_SERVICE_H_ */
