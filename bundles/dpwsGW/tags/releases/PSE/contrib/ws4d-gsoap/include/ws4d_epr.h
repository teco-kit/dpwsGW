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


#ifndef WS4D_EPR_H_
#define WS4D_EPR_H_

/**
 * Endpoint Reference
 *
 * @addtogroup APIEndpoint Endpoint Reference
 * @ingroup WS4D_UTILS
 *
 * @{
 */

/**
 * Structure of Enpoint Reference
 */
struct ws4d_epr
{
  struct ws4d_list_node list;
  void *elementof;
  char *Address;
  ws4d_qnamelist PortType;
  char *ServiceName;
  char *PortName;
  int valid;
  ws4d_alloc_list alist;
  struct ws4d_list_node copyof, copies;
  struct ws4d_list_node plugins;
#ifdef WITH_MUTEXES
    WS4D_MUTEX (lock);
#endif
};

/**
 * Macro to get allocation list from an Enpoint Reference
 */
#define ws4d_epr_get_alist(epr) \
  (&epr->alist)

/**
 * TODO: add documentation
 */
int ws4d_epr_init (struct ws4d_epr *epr);

/**
 * TODO: add documentation
 */
int ws4d_epr_done (struct ws4d_epr *epr);

#define ws4d_epr_reset ws4d_epr_done

/**
 * Function to allocate one or more Enpoint Reference List Element structures
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 *
 * epr = ws4d_epr_alloc(1, &alloclist);
 *
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param count number of eprs to allocate, should be at least 1
 * @param alist allocation list to allocate memory
 * @return pointer to newly allocated epr or NULL on failure
 */
struct ws4d_epr *ws4d_epr_alloc (int count, ws4d_alloc_list * alist);

/**
 * Function to free all memory allocated by plugins und dergister plugins
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 *
 * ws4d_epr_free(1, epr);
 *
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr Endpoint Reference to free
 * @param count number of EPRs to free
 * @return WS4D_OK on success, an error code otherwise
 */
int ws4d_epr_free (int count, struct ws4d_epr *epr);

/**
 * Macro to test if an Endpoint Reference is valid
 *
 * @param epr Eindpoint to validate
 */
#define ws4d_epr_isvalid(epr) \
  (epr && ((epr)->valid == 1))

/**
 * Function to validate an Endpoint Reference
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 *
 * ws4d_epr_validate(epr);
 *
 * ws4d_epr_free(1, epr);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr Eindpoint to validate
 * @return WS4D_OK on success, an error code otherwise
 */
int ws4d_epr_validate (struct ws4d_epr *epr);

/**
 * Function to invalidate an Endpoint Reference
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 * ws4d_epr_validate(epr);
 *
 * ws4d_epr_invalidate(epr);
 *
 * ws4d_epr_free(1, epr);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr Eindpoint to invalidate
 * @return WS4D_OK on success, an error code otherwise
 */
int ws4d_epr_invalidate (struct ws4d_epr *epr);

/**
 * Function to set the Address of an Endpoint Reference
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 *
 * ws4d_epr_set_Addrs(epr, "urn:uuid:[uuid]");
 *
 * ws4d_epr_free(1, epr);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr Enpoint Reference to change Address
 * @param Addrs Address to set
 * @return WS4D_OK on success, an error code otherwise
 */
int ws4d_epr_set_Addrs (struct ws4d_epr *epr, const char *Addrs);

/**
 * Function to get the Address of an Endpoint Reference
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 * ws4d_epr_set_Addrs(epr, "urn:uuid:[uuid]");
 *
 * printf("epr: %s\n", ws4d_epr_get_Addrs(epr));
 *
 * ws4d_epr_free(1, epr);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr Enpoint Reference
 * @return Address of Endpoint
 */
const char *ws4d_epr_get_Addrs (struct ws4d_epr *epr);

/**
 * Function to get the port types of an Endpoint Reference
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr
 * ws4d_qnamelist *types;
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 *
 * types = ws4d_epr_get_PortTypes(epr);
 * if (types)
 * {
 *   printf("epr has types: %s\n", ws4d_qnamelist_tostring(types, &alloclist));
 * }
 *
 * ws4d_epr_free(1, epr);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr Enpoint Reference to get port types
 * @return pointer to ws4d_qnamelist structure on success, an error code otherwise
 */
ws4d_qnamelist *ws4d_epr_get_PortTypes (struct ws4d_epr *epr);

/**
 * Function to set the service name of an Endpoint Reference
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 *
 * ws4d_epr_set_ServiceName(epr, "TestService");
 *
 * ws4d_epr_free(1, epr);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr Enpoint Reference to change service name
 * @param ServiceName service name to set
 * @return WS4D_OK on success, an error code otherwise
 */
int ws4d_epr_set_ServiceName (struct ws4d_epr *epr, const char *ServiceName);

/**
 * Function to get the service name of an Endpoint Reference
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 * ws4d_epr_set_ServiceName(epr, "TestService");
 *
 * printf("epr has service name: %s\n", ws4d_epr_get_ServiceName(epr));
 *
 * ws4d_epr_free(1, epr);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr Enpoint Reference
 * @return service name of Endpoint
 */
const char *ws4d_epr_get_ServiceName (struct ws4d_epr *epr);

/**
 * Function to set the port name of an Endpoint Reference
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 *
 * ws4d_epr_set_PortName(epr, "TestPort");
 *
 * ws4d_epr_free(1, epr);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr Enpoint Reference to change port name
 * @param PortName port name to set
 * @return WS4D_OK on success, an error code otherwise
 */
int ws4d_epr_set_PortName (struct ws4d_epr *epr, const char *PortName);

/**
 * Function to get the port name of an Endpoint Reference
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 * ws4d_epr_set_PortName(epr, "TestPort");
 *
 * printf("epr has port name: %s\n", ws4d_epr_get_PortName(epr));
 *
 * ws4d_epr_free(1, epr);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr Enpoint Reference
 * @return port name of Endpoint
 */
const char *ws4d_epr_get_PortName (struct ws4d_epr *epr);

/**
 * Function to copy the content of one Endpoint Reference structure to another structure
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr1, *epr2;
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr1 = ws4d_epr_alloc(1, &alloclist);
 * epr2 = ws4d_epr_alloc(1, &alloclist);
 *
 * ws4d_epr_set_Addrs(epr1, "urn:uuid:[uuid]");
 *
 * printf("epr1: %s\n", ws4d_epr_get_Addrs(epr1));
 * printf("epr2: %s\n", ws4d_epr_get_Addrs(epr2));
 *
 * ws4d_epr_copy(epr2, epr1);
 *
 * printf("epr1: %s\n", ws4d_epr_get_Addrs(epr1));
 * printf("epr2: %s\n", ws4d_epr_get_Addrs(epr2));
 *
 * ws4d_epr_free(1, epr1);
 * ws4d_epr_free(1, epr2);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param dst destination Endpoint Reference structure
 * @param src source Endpoint Reference structure
 * @return WS4D_OK on success, an error code otherwise
 */

int ws4d_epr_copy (struct ws4d_epr *dst, struct ws4d_epr *src);

/**
 * Function to lock an enpoint reference structure
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 *
 * ws4d_epr_lock(epr);
 *
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr Endpoint Reference to lock
 */
#ifdef WITH_MUTEXES
void ws4d_epr_lock (struct ws4d_epr *epr);
#else
#define ws4d_epr_lock(epr)
#endif

/**
 * Function to unlock an enpoint reference structure
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 *
 * ws4d_epr_lock(epr);
 * //critical path
 * ws4d_epr_unlock(epr);
 *
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr Endpoint Reference to lock
 */
#ifdef WITH_MUTEXES
void ws4d_epr_unlock (struct ws4d_epr *epr);
#else
#define ws4d_epr_unlock(epr)
#endif

/**
 * Endpoint Reference List Element Plugin Mechanism
 *
 * This is an example from the epr key value plugin.
 * @code
 * //epr plugin id
 * #define WS4D_EPRKVPLUGIN_ID "WS4D-EPR-KeyValuePlugin-0.1"
 * const char *ws4d_eprkv_plugin_id = WS4D_EPRKVPLUGIN_ID;
 *
 * struct ws4d_eprkv_plugin_s
 * {
 *   struct ws4d_list_node kvlist;
 * #ifdef WITH_MUTEXES
 *   WS4D_MUTEX(mutex);
 * #endif
 * };
 *
 * //forward declarations
 * INLINE struct ws4d_eprkv_plugin_s
 *   *ws4d_eprkv_plugindata (struct ws4d_epr *epr);
 *
 * int ws4d_eprkv_fcopy (struct ws4d_epr *epr, struct ws4d_epr_plugin *dst,
 *                       struct ws4d_epr_plugin *src);
 *
 * void ws4d_eprkv_fdelete (struct ws4d_epr *epr, struct ws4d_epr_plugin *p);
 *
 * int ws4d_eprkv_fcreate (struct ws4d_epr *epr,
 *                         struct ws4d_epr_plugin *plugin, void *arg);
 *
 * // callbacks for epr plugin functions
 * static struct ws4d_epr_plugin_cb ws4d_eprkv_callbacks =
 *   {ws4d_eprkv_fdelete, ws4d_eprkv_fcopy, NULL};
 *
 * // get epr kpv plugin data from an epr
 * INLINE struct ws4d_eprkv_plugin_s *
 * ws4d_eprkv_plugindata (struct ws4d_epr *epr)
 * {
 *   return (struct ws4d_eprkv_plugin_s *)
 *     ws4d_epr_get_plugindata (epr, ws4d_eprkv_plugin_id);
 * }
 *
 * // copy callback for plugin copy/update functionality
 * int
 * ws4d_eprkv_fcopy (struct ws4d_epr *epr, struct ws4d_epr_plugin *dst,
 *                   struct ws4d_epr_plugin *src)
 * {
 *   struct ws4d_eprkv_plugin_s *dst_data = NULL, *src_data = (struct ws4d_eprkv_plugin_s *) src->data;
 *
 *   // test parameters
 *   ws4d_assert (src_data, WS4D_ERR);
 *
 *   // set plugin id and callbacks
 *   dst->id = ws4d_eprkv_plugin_id;
 *   dst->cb = &ws4d_eprkv_callbacks;
 *
 *   // allocate plugin data structure if it is not already available
 *   if (dst->data)
 *     {
 *       dst_data = dst->data;
 *     }
 *   else
 *     {
 *       dst_data = ws4d_malloc (sizeof (struct ws4d_eprkv_plugin_s));
 *       if (!dst_data)
 *         {
 *           return WS4D_EOM;
 *         }
 *
 *       // initialize plugin data structure
 *       WS4D_INIT_LIST(&dst_data->kvlist);
 *       ws4d_mutex_init(&dst_data->mutex);
 *     }
 *
 *   // copy plugin data from src epr to dst epr
 *   ws4d_mutex_lock(&src_data->mutex);
 *   ws4d_mutex_lock(&dst_data->mutex);
 *   ws4d_kv_duplist(src_data->kvlist, dst_data->kvlist, ws4d_epr_get_alist(epr));
 *   ws4d_mutex_unlock(&dst_data->mutex);
 *   ws4d_mutex_unlock(&src_data->mutex);
 *
 *   // set plugin data structure as destination plugin data
 *   dst->data = dst_data;
 *
 *   return WS4D_OK;
 * }
 *
 * // delete callback for epr reset and free functionality
 * void
 * ws4d_eprkv_fdelete (struct ws4d_epr *epr, struct ws4d_epr_plugin *p)
 * {
 *   struct ws4d_eprkv_plugin_s *plugin = p->data;
 *
 *   // if there is plugin data to free
 *   if (plugin)
 *     {
 *       // free plugin data structure elements
 *       ws4d_mutex_destroy (&plugin->mutex);
 *       ws4d_kv_listclear (&plugin->kvlist);
 *       // free plugin data structure
 *       ws4d_free(plugin);
 *     }
 * }
 *
 * // callback for ws4d_epr_register_plugin() function to create plugin
 * int
 * ws4d_eprkv_fcreate (struct ws4d_epr *epr,
 *                        struct ws4d_epr_plugin *plugin, void *arg)
 * {
 *   struct ws4d_eprkv_plugin_s *data = NULL;
 *
 *   // set plugin id and callbacks
 *   plugin->id = ws4d_eprkv_plugin_id;
 *   plugin->cb = &ws4d_eprkv_callbacks;
 *
 *   // allocate plugin data structure
 *   data = ws4d_malloc (sizeof (struct ws4d_eprkv_plugin_s));
 *   if (!data)
 *     {
 *       return WS4D_EOM;
 *     }
 *
 *   // initialize plugin data structure
 *   WS4D_INIT_LIST(&data->kvlist);
 *   ws4d_mutex_init(&data->mutex);
 *
 *   // set plugin data structure as plugin data
 *   plugin->data = data;
 *
 *   return WS4D_OK;
 * }
 *
 * // function to create plugin and avoid duplicates
 * int
 * ws4d_eprkv_plugin (struct ws4d_epr *epr)
 * {
 *   struct ws4d_eprkv_plugin_s *plugin = ws4d_eprkv_plugindata (epr);
 *
 *   // check if plugin is already registered
 *   if (plugin)
 *     {
 *       return WS4D_OK;
 *     }
 *
 *   // register plugin
 *   return ws4d_epr_register_plugin (epr, ws4d_eprkv_fcreate, NULL);
 * }
 * @endcode
 *
 * @addtogroup APIEndpointPlugin Endpoint Reference Plugin Mechanism
 * @ingroup APIEndpoint
 *
 * @{
 */

struct ws4d_epr_plugin;

struct ws4d_epr_plugin_cb
{
  void (*fdelete) (struct ws4d_epr * epr, struct ws4d_epr_plugin * p);
  int (*fcopy) (struct ws4d_epr * epr, struct ws4d_epr_plugin * dst,
                struct ws4d_epr_plugin * src);
  int (*finvalidate) (struct ws4d_epr * epr, struct ws4d_epr_plugin * p);
};

struct ws4d_epr_plugin
{
  const char *id;
  void *data;
  struct ws4d_epr_plugin_cb *cb;
  struct ws4d_list_node list;
};

/**
 * Function to register a plugin at an epr
 *
 * @param epr epr to register plugin
 * @param fcreate function to create plugin
 * @param arg arguments for fcreate function
 *
 * @return WS4D_OK on success, an error code otherwise
 */
int ws4d_epr_register_plugin (struct ws4d_epr *epr,
                              int (*fcreate) (struct ws4d_epr * epr,
                                              struct ws4d_epr_plugin *,
                                              void *), void *arg);

/**
 * Function to get a plugin of an epr by plugin id
 *
 * @param epr epr to get plugin
 * @param id plugin id
 *
 * @return pointer to ws4d_epr_plugin structure
 */
struct ws4d_epr_plugin *ws4d_epr_get_plugin (struct ws4d_epr *epr,
                                             const char *id);

/**
 * Function to get the plugin data of an epr by plugin id
 *
 * @param epr epr to get plugin data
 * @param id plugin id
 *
 * @return pointer to the data a plugin points to
 */
void *ws4d_epr_get_plugindata (struct ws4d_epr *epr, const char *id);

/**
 * Function to remove a plugin of an epr
 *
 * @param epr epr to remove plugin
 * @param id plugin id
 *
 * @return WS4D_OK on success, an error code otherwise
 */
int ws4d_epr_remove_plugin (struct ws4d_epr *epr, const char *id);

/**
 * Function to remove all plugins of an epr
 *
 * @param epr epr to remove all plugins
 *
 * @return WS4D_OK on success, an error code otherwise
 */
void ws4d_epr_remove_plugins (struct ws4d_epr *epr);

/** @} */

/** @} */

#endif /*WS4D_EPR_H_ */
