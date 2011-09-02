typedef unsigned int xsd__unsignedInt;

typedef char* xsd__IDREFS;

enum xsd__boolean_ { _false, _true };

/* typedef char* _XML; */

struct ws4d_dur
{       int     years;
        int     months;
        int     days;
        int     hours;
        int     minutes;
        int     seconds;
};

extern typedef struct ws4d_dur xsd__duration;


typedef char* _xml__lang;
