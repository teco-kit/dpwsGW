#ifndef LCMDEFS_H_
#define LCMDEFS_H_

/* LCM status definition */
#define LCM_OK         0
#define LCM_ERR       -1

#define LCM_INVALID     0
#define LCM_VALID       1

/* LCM ERROR Definition */
#define LCM_ERR_ID                     1
#define LCM_ERR_URN_IN_USE             2
#define LCM_ERR_CACHE_ERROR            3
#define LCM_ERR_SPECIAL_SIGN_NAME      4
#define LCM_ERR_SPECIAL_SIGN_FILE      5
#define LCM_ERR_CREAT_WORK_DIR         6
#define LCM_ERR_WORK_DIR_EXIST         7
#define LCM_ERR_SAVE_ATTACHMENT        8
#define LCM_ERR_ATTACHMENT_CRC         9
#define LCM_ERR_NO_MANIFEST           10
#define LCM_ERR_READ_MANIFEST         11
#define LCM_ERR_NO_VALID_IP           12
#define LCM_ERR_NO_SERVICE            13
#define LCM_ERR_INTERNAL_CACHE        14
#define LCM_ERR_NO_VALID_HANDLE       15
#define LCM_ERR_DELETE_DIR            16
#define LCM_ERR_LOAD_SO               17
#define LCM_ERR_RESOLVE_STATE         18
#define LCM_ERR_SO_FUNCTION           19
#define LCM_ERR_FORK                  20
#define LCM_ERR_VALID_PID             21
#define LCM_ERR_OPEN_WORK_DIR         22
#define LCM_ERR_NO_NEWER_VERSION      23
#define LCM_ERR_DEVICE_IS_NOT_RUNNING 24
#define LCM_ERR_THREAD_NOT_TERMINATE  25
#define LCM_ERR_MALLOC                26
#define LCM_ERR_METADATA              27
#define LCM_ERR_DIR_NOT_MOVED         28
#define LCM_ERR_ALREADY_RUNNING       29

struct device_error
{
  char string[255];
} device_error;

static struct device_error error_list[] = {
  {"Not specified"},
/*  1 */ {"ERROR: the given id ist not correct"},
/*  2 */ {"ERROR: could not install, service already exist"},
/*  3 */ {"ERROR: cache error"},
/*  4 */ {"ERROR: in the name an unexpected special sign was found"},
/*  5 */ {"ERROR: in the file-name an unexpected special sign was found"},
/*  6 */ {"ERROR: working directory for service could not be created"},
/*  7 */ 
    {"ERROR: working directory for service already exist, take an other service-name"},
/*  8 */ {"ERROR: Cannot save attachment"},
/*  9 */ {"ERROR: CRC not valid"},
/* 10 */ {"ERROR: no manifest file found"},
/* 11 */ {"ERROR: can not read manifest"},
/* 12 */ {"ERROR: no valid IP is given"},
/* 13 */ {"ERROR: no service file found"},
/* 14 */ 
    {"ERROR: could not install service, error while writing into internal cache"},
/* 15 */ {"ERROR: the given handle ist not valid"},
/* 16 */ {"ERROR: directory could not be deleted"},
/* 17 */ {"ERROR: can not load .so-file"},
/* 18 */ {"ERROR: resolve-Status not valid"},
/* 19 */ {"ERROR: function not found"},
/* 20 */ {"ERROR: fork().........."},
/* 21 */ {"ERROR: error in cache, no valid pid"},
/* 22 */ {"ERROR: Could not open the directory"},
/* 23 */ {"ERROR: the given version is not  newer"},
/* 24 */ {"ERROR: device is not running"},
/* 25 */ {"ERROR: the thread could not be terminate"},
/* 26 */ {"ERROR: malloc failed"},
/* 27 */ {"ERROR: Metadata not valid"},
/* 28 */ {"ERROR: Dir can not be moved"},
/* 29 */ {"ERROR: service is already running"},
/*  */ {""}

};


#define LCM_LEER           1
#define LCM_INSTALL        2
#define LCM_RESOLVE        3
#define LCM_RUNNING        4
#define LCM_NOT_SPECIFIED  5

struct device_state
{
  char string[50];
} device_state;

static struct device_state state_list[] = {
/*  0 */ {"ERROR!!!!    "},
/*  1 */ {"Empty        "},
/*  2 */ {"Installed    "},
/*  3 */ {"Resolved     "},
/*  4 */ {"Running      "},
/*  5 */ {"Not specified"},
};

#endif /* LCMDEFS_H_ */
