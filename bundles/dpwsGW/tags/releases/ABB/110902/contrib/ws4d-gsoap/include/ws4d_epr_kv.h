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
 *
 *  Created on: 13.08.2008
 *      Author: Elmar Zeeb
 */

#ifndef WS4D_EPR_KV_H_
#define WS4D_EPR_KV_H_

/**
 * Key value pairs for eprs
 *
 * @addtogroup WsaEprKv Key value pairs for eprs
 * @ingroup WS4D_UTILS
 *
 * @{
 */

#define WS4D_KEYTYPE_NONE    (0)
#define WS4D_KEYTYPE_INTEGER (1)
#define WS4D_KEYTYPE_STRING  (2)
#define WS4D_KEYTYPE_VOID    (3)

/**
 * creates a new key without value
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr;
 * const char *test_key1 = "TestKey1";
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 *
 * ws4d_epr_create_key(epr, test_key1);
 *
 * ws4d_epr_free(1, epr);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr epr to create key
 * @param key key that is copied by reference
 * @return WS4D_OK on success, an error code otherwise
 */
int ws4d_epr_create_key (struct ws4d_epr *epr, const char *key);

/**
 * Check if a epr has a specific key
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr;
 * const char *test_key1 = "TestKey1";
 * const char *test_key2 = "TestKey2";
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 *
 * ws4d_epr_create_key(epr, test_key1);
 *
 * if (ws4d_epr_has_key(epr, test_key1))
 * {
 *   printf("epr has key %s", test_key1);
 * }
 * else
 * {
 *   printf("epr has no key %s", test_key1);
 * }
 *
 * if (ws4d_epr_has_key(epr, test_key2))
 * {
 *   printf("epr has key %s", test_key2);
 * }
 * else
 * {
 *   printf("epr has no key %s", test_key2);
 * }
 *
 * ws4d_epr_free(1, epr);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr epr that is checked for a key
 * @param key key that is used by reference
 * @return 1, if epr has the specified key, 0 otherwise
 */
int ws4d_epr_has_key (struct ws4d_epr *epr, const char *key);

/**
 * Check the value type of an key value pair
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr;
 * const char *test_key1 = "TestKey1";
 * int type;
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 *
 * ws4d_epr_create_key(epr, test_key1);
 * if (ws4d_epr_get_keytype(epr, test_key1, &type) == WS4D_OK)
 * {
 *   switch(type)
 *   {
 *     case WS4D_KEYTYPE_NONE:
 *       printf("key %s has type WS4D_KEYTYPE_NONE\n", test_key1);
 *       break;
 *     case WS4D_KEYTYPE_INTEGER:
 *       printf("key %s has type WS4D_KEYTYPE_INTEGER\n", test_key1);
 *       break;
 *     case WS4D_KEYTYPE_STRING:
 *       printf("key %s has type WS4D_KEYTYPE_STRING\n", test_key1);
 *       break;
 *     case WS4D_KEYTYPE_VOID:
 *       printf("key %s has type WS4D_KEYTYPE_VOID\n", test_key1);
 *       break;
 *     default:
 *       printf("key %s has type\n", test_key1);
 *   }
 * }
 * else
 * {
 *   printf("error getting type of key %s", test_key1);
 * }
 *
 * ws4d_epr_free(1, epr);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr epr that is checked for the type of a key
 * @param key key that is used by reference
 * @param type resulting key type is copied to this reference
 * @return WS4D_OK on success, an error code otherwise
 */
int ws4d_epr_get_keytype (struct ws4d_epr *epr, const char *key, int *type);

/**
 * Set the value of an existing integer key or create one if it doesn't exist
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr;
 * const char *int_key1 = "IntKey1";
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 *
 * ws4d_epr_set_intkey(epr, int_key1, 10);
 *
 * ws4d_epr_free(1, epr);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr epr to create or set the integer key
 * @param key key that is copied and used by reference
 * @param integer integer value of the integer key
 * @return WS4D_OK on success, an error code otherwise
 */
int ws4d_epr_set_intkey (struct ws4d_epr *epr, const char *key, int integer);

/**
 * Get the value of an existing integer key
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr;
 * const char *int_key1 = "IntKey1";
 * int integer = 0;
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 *
 * ws4d_epr_set_intkey(epr, int_key1, 10);
 * if (ws4d_epr_get_intkey(epr, int_key1, &integer) == WS4D_OK)
 * {
 *   printf("Key %s has an integer value of %d\n", int_key1, integer);
 * }
 * else
 * {
 *   printf("error getting value of key %s", int_key1);
 * }
 *
 * ws4d_epr_free(1, epr);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr epr to get the integer key value
 * @param key key that is used by reference
 * @param integer resulting key value is copied to this reference
 * @return WS4D_OK on success, an error code otherwise
 */
int ws4d_epr_get_intkey (struct ws4d_epr *epr, const char *key, int *integer);

/**
 * Set the value of an existing string key or create one if it doesn't exist
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr;
 * const char *string_key1 = "StringKey1";
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 *
 * ws4d_epr_set_stringkey(epr, string_key1, "TestString");
 *
 * ws4d_epr_free(1, epr);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr epr to create or set the string key
 * @param key key that is copied and used by reference
 * @param string string value of the integer key
 * @return WS4D_OK on success, an error code otherwise
 */
int
ws4d_epr_set_stringkey (struct ws4d_epr *epr, const char *key,
                        const char *string);

/**
 * Get the value of an existing string key
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr;
 * const char *string_key1 = "StringKey1";
 * char *string = NULL;
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 *
 * ws4d_epr_set_stringkey(epr, string_key1, "TestString");
 * if (ws4d_epr_get_stringkey(epr, string_key1, &string) == WS4D_OK)
 * {
 *   printf("Key %s has an string value of \"%s\"\n", string_key1, string);
 * }
 * else
 * {
 *   printf("error getting value of key %s", string_key1);
 * }
 *
 * ws4d_epr_free(1, epr);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr epr to get the string key value
 * @param key key that is used by reference
 * @param string resulting key value is copied to this reference
 * @return WS4D_OK on success, an error code otherwise
 */
int
ws4d_epr_get_stringkey (struct ws4d_epr *epr, const char *key,
                        const char **string);

/**
 * Set the value of an existing void key or create one if it doesn't exist
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr;
 * const char *void_key1 = "VoidKey1";
 *
 * struct custom_data_s
 * {
 *   int number;
 * }
 * struct custom_data_s custom_data;
 *
 * custom_data.number = 4;
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 *
 * ws4d_epr_set_voidkey(epr, void_key1, (void *) &custom_data, sizeof(struct custom_data_s));
 *
 * ws4d_epr_free(1, epr);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr epr to create or set the void key
 * @param key key that is copied and used by reference
 * @param data pointer to value of the void key
 * @param size size of void value
 * @return WS4D_OK on success, an error code otherwise
 */
int
ws4d_epr_set_voidkey (struct ws4d_epr *epr, const char *key, void *data,
                      size_t size);

/**
 * Get the value of an existing void key
 *
 * @code
 * ws4d_alloc_list alloclist;
 * struct ws4d_epr *epr;
 * const char *void_key1 = "VoidKey1";
 *
 * struct custom_data_s
 * {
 *   int number;
 * }
 * struct custom_data_s custom_data, *custom_data_p;
 * size_t size;
 *
 * custom_data.number = 4;
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 *
 * ws4d_epr_set_voidkey(epr, void_key1, (void *) &custom_data, sizeof(struct custom_data_s));
 * if (ws4d_epr_get_voidkey(epr, void_key1, (void *) &custom_data_p, &size) == WS4D_OK)
 * {
 *   printf("Key %s has an void value of size %d\n", void_key1, size);
 *   printf("custom_data.number = %d\n", custom_data_p->number);
 * }
 * else
 * {
 *   printf("error getting value of key %s", void_key1);
 * }
 *
 * ws4d_epr_free(1, epr);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr epr to get the void key value
 * @param key key that is used by reference
 * @param data resulting key value is copied to this reference
 * @param size size of void value is copied to this reference
 * @return WS4D_OK on success, an error code otherwise
 */
int
ws4d_epr_get_voidkey (struct ws4d_epr *epr, const char *key, void **data,
                      size_t * size);

/**
 * Remove a key
 *
 * @code
 * const char *test_key1 = "TestKey1";
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * epr = ws4d_epr_alloc(1, &alloclist);
 *
 * ws4d_epr_create_key(epr, test_key1);
 * if (ws4d_epr_remove_key(epr, test_key1) == WS4D_OK)
 * {
 *   printf("removed key %s\n", test_key1);
 * }
 * else
 * {
 *   printf("error removing key %s", test_key1);
 * }
 *
 * ws4d_epr_free(1, epr);
 * ws4d_alloclist_done(&alloclist);
 * @endcode
 *
 * @param epr epr to remove key
 * @param key key that is used by reference
 * @return WS4D_OK on success, an error code otherwise
 */
int ws4d_epr_remove_key (struct ws4d_epr *epr, const char *key);

/** @} */

#endif /* WS4D_EPR_KV_H_ */
