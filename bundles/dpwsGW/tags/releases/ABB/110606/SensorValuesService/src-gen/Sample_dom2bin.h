#ifndef ___SAMPLE_BIN2DOM_H
#define ___SAMPLE_BIN2DOM_H 1
#include <bitsio/write_bits.h>
#include <string.h>
#include <bitsio/write_bits_buf.h>
#include "sens_types.h"
#include <ptr_stack.h>

int Sample_dom2bin_run(struct sens_SSimpSample *, struct WRITER_STRUCT *);
#endif
