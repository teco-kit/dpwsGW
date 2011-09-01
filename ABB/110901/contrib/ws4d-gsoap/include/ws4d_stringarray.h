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

#ifndef WS4D_STRINGARRAY_H_
#define WS4D_STRINGARRAY_H_

/**
 * Function allocates memory for a xsd:Array of a defined size
 *
 * @param count number of elements in Array
 * @param alist allocation list to insert allocation
 * @returns pointer to newly-allocated xsd:Array or NULL if no memory
 */
char **ws4d_alloc_xsdArray (int count, ws4d_alloc_list * alist);

/**
 * Function frees a xsd:Array
 *
 * @param Array xsd:Array to free
 */
void ws4d_free_xsdArray (char **Array);

/**
 * Function converts an xsd:List from space spereated string representation to
 * character string array.
 *
 * @code
 * ws4d_alloc_list alloclist;
 * char *list = "string1 string2 string3", **array = NULL;
 * int i = 0;
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 * array = ws4d_xsdList_to_Array(list, &alloclost);
 *
 * while (array[i])
 * {
 *   printf("entry %d is : %s\n", i, array[i]);
 *   i++;
 * }
 * @endcode
 *
 * @param List space spereated string representation of xsd:List
 * @param alist list to insert allocation
 */
char **ws4d_xsdList_to_Array (const char *List, ws4d_alloc_list * alist);


/**
 * Function to match to xsd:List in array representation. The matching function must be specified
 *
 * @code
 * ws4d_alloc_list alloclist;
 * char *list1 = "test1 test2", *list2 = "test2 test1";
 * char **lista1 = NULL, **lista2=NULL;
 *
 * WS4D_ALLOCLIST_INIT(&alloclist);
 *
 * lista1 = ws4d_xsdList_to_Array(list1, &alloclost);
 * lista2 = ws4d_xsdList_to_Array(list2, &alloclost);
 *
 * if (ws4d_xsdArray_match(lista1, lista2, strcmp))
 * {
 *   printf("list1 and list2 match\n");
 * }
 * @endcode
 *
 * @param Types Array of port types
 * @param TypesToMatch Array of port types
 * @param soap_match_func function for matching
 *
 * @return 1 if TypesToMatch is a section of ServiceTypes or 0
 * otherwise
 */
int ws4d_xsdArray_match (char **Types, char **TypesToMatch,
                         int (*soap_match_func) (const char *s1,
                                                 const char *s2));

#endif /*WS4D_STRINGARRAY_H_ */
