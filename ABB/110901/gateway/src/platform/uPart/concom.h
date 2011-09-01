/*
 * Copyright (c) 2007
 * Telecooperation Office (TecO), Universitaet Karlsruhe (TH), Germany.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. Neither the name of the Universitaet Karlsruhe (TH) nor the names
 *    of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author(s): Philipp Scholl <scholl@teco.edu>
 */

#ifndef _CONCOM_CONCOM_H_
# define _CONCOM_CONCOM_H_

# include <stdint.h>



#define PACKED __attribute((__packed__))


/** concom tuple types are 2bytes long */
typedef uint16_t cc_type;

/** concom tuples are triplets of type, len and data fields. */
typedef struct cc_tuple
{
  cc_type type;
  uint8_t len;

  char buf[];
} PACKED cc_tuple;

typedef uint16_t char3to2;

#define HCC32(A,B,C) ((A-0x33)+((B-0x33)*40)+((C-0x33)*1600))
#define CC32(A,B,C) ((HCC32(A,B,C)/256)+((HCC32(A,B,C)%256)*256))

enum cc_range
{
	CC_RANGE_1cm,
	CC_RANGE_10cm,
	CC_RANGE_100cm,
	CC_RANGE_1000cm
};

/** location of a node */
typedef struct location
{
    uint8_t  root[16];
    char3to2 level[4][3];
    struct {
    	unsigned value:6;
    	enum cc_range range:2;
    }PACKED x,y,z;
    uint8_t deviation;
}PACKED location;


/** on-wire format for ethernet/UDP */
#define CC_VERSION 0

struct cc_pkt_header
{
	uint8_t  version;
	location loc;
	uint8_t  srcid[8];
    uint8_t  seqnr;
}PACKED;

typedef struct cc_pkt
{
    struct cc_pkt_header header;
    union payload{
    	cc_tuple first;
    	char  buf[64];
    } payload;
} PACKED cc_pkt;

#define cc_tuple_next(T) ((cc_tuple*)&(T)->buf[(T)->len])
#define cc_pkt_get_len(P,LAST) (&(LAST)->buf[len]-(P)->payload.buf)



#endif /* _CONCOM_H_ */
