#ifndef ___STATUSCONTROL_BIN2DOM_H
#define ___STATUSCONTROL_BIN2DOM_H 1
#include <bitsio/read_bits.h>
#include "sens_types.h"
typedef sens_SSimpControl StatusControl_t;

int StatusControl_bin2dom_run(struct READER_STRUCT *reader,
		sens_SSimpControl *dom);
#endif
