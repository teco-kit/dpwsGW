#ifndef ___STATUS_BIN2DOM_H
#define ___STATUS_BIN2DOM_H 1
#include <bitsio/read_bits.h>
#include "sens_types.h"
typedef sens_SSimpStatus Status_t;

int Status_bin2dom_run(struct READER_STRUCT *reader, sens_SSimpStatus *dom);
#endif
