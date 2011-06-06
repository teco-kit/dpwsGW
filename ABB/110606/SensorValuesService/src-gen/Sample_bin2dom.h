#ifndef ___SAMPLE_BIN2DOM_H
#define ___SAMPLE_BIN2DOM_H 1
#include <bitsio/read_bits.h>
#include "sens_types.h"
typedef sens_SSimpSample Sample_t;

int Sample_bin2dom_run(struct READER_STRUCT *reader, sens_SSimpSample *dom);
#endif
