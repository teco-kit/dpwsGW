#ifndef ___STATUS_BIN2DOM_H
#define ___STATUS_BIN2DOM_H 1
#include <bitsio/write_bits.h>
#include <string.h>
#include <bitsio/write_bits_buf.h>
#include "sens_types.h"
#include <ptr_stack.h>

int Status_dom2bin_run(struct sens_SSimpStatus *, struct WRITER_STRUCT *);
#endif
