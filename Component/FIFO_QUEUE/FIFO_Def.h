
#ifndef __FIFO_DEF_H__
#define __FIFO_DEF_H__

#include "GlobalDef.h"



#define EXTERN_CLASS                typedef struct {\
                                        uint8_t chMask[sizeof(struct {

#define END_EXTERN_CLASS(__NAME)        })];\
                                    }__NAME;

#define DEF_CLASS                   typedef struct {
#define END_DEF_CLASS(__NAME)       }__##__NAME;\
                                    typedef struct {\
                                        uint8_t chMask[sizeof(__##__NAME)];\
                                    }__NAME;

#define CLASS(__NAME)               __##__NAME

#define PRIVATE    static

//! вдIARЮЊР§зг
#define NO_INIT    __attribute__((section("NO_INIT"),zero_init))   //__no_init

#define TYPE_CONVERT(__ADDR,__TYPE)    (*((__TYPE *)(__ADDR)))



#endif

