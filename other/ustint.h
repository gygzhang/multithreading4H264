#ifndef __stdint_h
#define __stdint_h
    /*  macros for minimum-width integer constants */
#define INT8_C(x)   (x)
#define INT16_C(x)  (x)
#define INT32_C(x)  (x)
#define INT64_C(x)  (x)

#define UINT8_C(x)  (x ## u)
#define UINT16_C(x) (x ## u)
#define UINT32_C(x) (x ## u)
#define UINT64_C(x) (x ## u)

/*  macros for greatest-width integer constants */
#define INTMAX_C(x)  (x ## ll)
#define UINTMAX_C(x) (x ## ull)

#endif