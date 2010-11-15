/* ========================================================================
 *        Filename   : bits_io.h
 *        Description:
 *        Version    : 1.0
 *        (C)        : Dimitar Yordanov (domidimi [at] gmail [dot] com)
 *        ---- History ---------------------------------------------------
 *        03.08.2008 11:34    dy    initial version
 * ========================================================================
 */

#ifndef _BITS_IO_H_
#define _BITS_IO_H_


#ifdef __GNUC__
#include <sys/types.h>
#include <stdint.h>
#endif

#ifndef u_char
#define u_char unsigned char
#endif



#define LOW_N_BITS(a_res_byte , nUm) (a_res_byte & ((1 << (nUm)) - 1))
#define UP_N_BITS(a_res_byte , nUm) (a_res_byte & ~((1 << (8 - nUm)) - 1))
#define LOW_N_BITS_SHIFT(a_res_byte , nUm) (LOW_N_BITS(a_res_byte, nUm) << (8 - nUm))
#define UP_N_BITS_SHIFT(a_res_byte , nUm) UP_N_BITS(a_res_byte, nUm) >> (8 - nUm)

#endif

