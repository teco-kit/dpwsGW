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

#ifndef WS4D_TARGET_H_
#define WS4D_TARGET_H_

/**
 * Target Endpoint
 *
 * @addtogroup APITargetEP Target Endpoint
 * @ingroup WS4D_UTILS
 *
 * @{
 */

#define WS4D_TARGET_INACTIVE   (0)
#define WS4D_TARGET_ACTIVE     (1)

/**
 * TODO: add documentation
 */
int ws4d_register_targetep (struct ws4d_epr *epr);

/**
 * TODO: add documentation
 */
int ws4d_targetep_set_XAddrs (struct ws4d_epr *epr, const char *XAddrs);

/**
 * TODO: add documentation
 */
const char *ws4d_targetep_get_XAddrs (struct ws4d_epr *epr);

/**
 * TODO: add documentation
 */
int ws4d_targetep_set_ProbeMsgId (struct ws4d_epr *epr,
                                  const char *ProbeMsgId);

/**
 * TODO: add documentation
 */
const char *ws4d_targetep_get_ProbeMsgId (struct ws4d_epr *epr);

/**
 * TODO: add documentation
 */
int ws4d_targetep_set_ResolveMsgId (struct ws4d_epr *epr,
                                    const char *ResolveMsgId);

/**
 * TODO: add documentation
 */
const char *ws4d_targetep_get_ResolveMsgId (struct ws4d_epr *epr);

/**
 * TODO: add documentation
 */
int ws4d_targetep_add_type (struct ws4d_epr *epr, struct ws4d_qname *type);

/**
 * TODO: add documentation
 */
int ws4d_targetep_add_typestr (struct ws4d_epr *epr, const char *typestr);

/**
 * TODO: add documentation
 */
int ws4d_targetep_del_type (struct ws4d_epr *epr, struct ws4d_qname *type);

/**
 * TODO: add documentation
 */
int ws4d_targetep_set_Types (struct ws4d_epr *epr, const char *Types);

/**
 * TODO: add documentation
 */
const char *ws4d_targetep_get_Types (struct ws4d_epr *epr);

/**
 * TODO: add documentation
 */
ws4d_qnamelist *ws4d_targetep_get_TypeList (struct ws4d_epr *epr);

/**
 * TODO: add documentation
 */
int ws4d_targetep_matches_Types (struct ws4d_epr *epr, const char **Types);

/**
 * TODO: add documentation
 */
int ws4d_targetep_add_Scope (struct ws4d_epr *epr, const char *Scope);

/**
 * TODO: add documentation
 */
int ws4d_targetep_del_Scope (struct ws4d_epr *epr, const char *Scope);

/**
 * TODO: add documentation
 */
int ws4d_targetep_set_Scopes (struct ws4d_epr *epr, const char *Scopes);

/**
 * TODO: add documentation
 */
const char *ws4d_targetep_get_Scopes (struct ws4d_epr *epr);

/**
 * TODO: add documentation
 */
int ws4d_targetep_matches_Scopes (struct ws4d_epr *epr, const char **Scopes,
                                  int (*scope_match_func) (const char *s1,
                                                           const char *s2));

/**
 * TODO: add documentation
 */
int ws4d_targetep_set_MetadataVersion (struct ws4d_epr *epr,
                                       int MetadataVersion);

/**
 * TODO: add documentation
 */
int ws4d_targetep_get_MetadataVersion (struct ws4d_epr *elem);

/**
 * TODO: add documentation
 */
int ws4d_targetep_changeMetadata (struct ws4d_epr *epr);

/**
 * TODO: add documentation
 */
int
ws4d_targetep_activate (struct ws4d_epr *epr, int MessageNumber,
                        int InstanceId, int MetadataVersion);

/**
 * TODO: add documentation
 */
int ws4d_targetep_deactivate (struct ws4d_epr *elem);

/**
 * TODO: add documentation
 */
int ws4d_targetep_isactive (struct ws4d_epr *elem);

/**
 * TODO: add documentation
 */
int ws4d_targetep_set_ts (struct ws4d_epr *epr, void *targetservice);

/**
 * TODO: add documentation
 */
void *ws4d_targetep_get_ts (struct ws4d_epr *epr);

/** @} */

#endif /*WS4D_TARGET_H_ */
