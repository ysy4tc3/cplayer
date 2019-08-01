#ifndef _CFCONST_H_
#define _CFCONST_H_

#define SUCCESS 0
#define ERROR_OUT_OF_BOUNDARY -1
#define ERROR_INVAILABLE_BOUNDARY -2
#define ERROR_NULL_POINTER -3

const char* cf_error_str(int code)
{
    switch(code){
        case 0:
            return "Function works well.";
            break;
        case -1:
            return "Coordinate out of boundary.";
            break;
        case -2:
            return "Invailable boundary.";
            break;
        case -3:
            return "Null pointer.";
            break;
        default:
            return "\0";
    }
}

#endif