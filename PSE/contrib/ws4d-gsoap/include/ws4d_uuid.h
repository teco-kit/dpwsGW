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

#ifndef WS4D_UUID_H_
#define WS4D_UUID_H_

/**
 * @addtogroup WS4D_UUID Functions to generate UUIDs
 * @ingroup WS4D_UTILS
 *
 * @{
 */

extern const char *ws4d_uuid_schema_prefix;
extern const char *ws4d_uuid_schema_format;

#define WS4D_UUID_SIZE (37)
#define WS4D_UUID_SCHEMA_SIZE (46)
#define WS4D_UUID_SCHEMA_PREFIX ws4d_uuid_schema_prefix
#define WS4D_UUID_SCHEMA_PREFIX_LEN 9

#define ws4d_uuid_generate_schema(buf, uuid_str) \
  SNPRINTF (buf, WS4D_UUID_SCHEMA_SIZE, ws4d_uuid_schema_format, uuid_str)

/**
 * generates a random uuid
 *
 * @param out character string buffer where to put uuid
 */
void ws4d_uuid_generate_random (char *out);

/** @} */

#endif /*WS4D_UUID_H_ */
