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

#ifndef WS4D_LOCALIZEDSTRING_H_
#define WS4D_LOCALIZEDSTRING_H_

/**
 * Localized String
 *
 * @addtogroup APILocalizeString Target Localized String
 * @ingroup WS4D_UTILS
 *
 * @{
 */

struct ws4d_locstring
{
  const char *string;
  const char *lang;
};

/**
 * macro to initialize a localized string
 */
#define ws4d_locstring_init_string(lang, string) {string, lang}

/**
 * function to extract one localization of a localized string
 *
 * @param string localized string
 * @param size size of localized string
 * @param lang language
 *
 * @return one localization of a localized string
 */
const char
  *ws4d_locstring_get (struct ws4d_locstring *string,
                       int size, const char *lang);

/**
 * TODO: add documentation
 */
struct ws4d_locstring
  *ws4d_locstring_dup (struct ws4d_locstring *string,
                       int size, ws4d_alloc_list * alist);

/**
 * TODO: add documentation
 */
void ws4d_locstring_free (struct ws4d_locstring *string, int size);

/** @} */

#endif /*WS4D_LOCALIZEDSTRING_H_ */
