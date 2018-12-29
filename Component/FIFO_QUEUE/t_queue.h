/***************************************************************************
 *   Copyright(C)2009-2012 by Gorgon Meducer<Embedded_zhuoran@hotmail.com> *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _USE_TEMPLATE_QUEUE_H_
#define _USE_TEMPLATE_QUEUE_H_

#include "FIFO_Def.h"
#include <stdbool.h>

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define END_DEF_QUEUE
#define END_DEF_QUEUE_U8
#define END_DEF_QUEUE_U16
#define END_DEF_QUEUE_U32

#define END_DEF_SAFE_QUEUE
#define END_DEF_SAFE_QUEUE_U8
#define END_DEF_SAFE_QUEUE_U16
#define END_DEF_SAFE_QUEUE_U32

/*============================ MACROFIED FUNCTIONS ===========================*/
#define NONE_ATOM_ACCESS(__CODE)        {__CODE;}

#define QUEUE_MUTEX(__NAME, __QUEUE)                                        \
            __NAME##_queue_mutex(__QUEUE) 

#define QUEUE_INIT(__NAME, __QUEUE, __BUFFER, __SIZE)                       \
            __NAME##_queue_init((__QUEUE), (__BUFFER), (__SIZE))

#define DEQUEUE(__NAME, __QUEUE, __ADDR)                                    \
            __NAME##_dequeue((__QUEUE),(__ADDR))

#define ENQUEUE(__NAME, __QUEUE, __VALUE)                                   \
            __NAME##_enqueue((__QUEUE), (__VALUE))

#define PEEK_QUEUE(__NAME, __QUEUE, __ADDR)                                 \
            __NAME##_queue_peek((__QUEUE),(__ADDR))
            

            
#define QUEUE(__NAME)   __NAME##_queue_t

#define DEF_QUEUE_EX(__NAME, __TYPE, __PTR_TYPE, __MUTEX_TYPE, __ATOM_ACCESS)  \
DEF_CLASS                                                                   \
    __TYPE          *ptBuffer;                                              \
    __PTR_TYPE      tSize;                                                  \
    __PTR_TYPE      tHead;                                                  \
    __PTR_TYPE      tTail;                                                  \
    __PTR_TYPE      tCounter;                                               \
    __MUTEX_TYPE    tMutex;                                                 \
END_DEF_CLASS(__NAME##_queue_t)                                             \
                                                                            \
__MUTEX_TYPE *__NAME##_queue_mutex(__NAME##_queue_t *ptQueue)               \
{                                                                           \
    CLASS(__NAME##_queue_t) *ptQ = (CLASS(__NAME##_queue_t) *)ptQueue;      \
    if ( NULL == ptQueue)  {                                                \
        return NULL;                                                        \
    }                                                                       \
    return &(ptQ->tMutex);                                                  \
}                                                                           \
                                                                            \
bool __NAME##_queue_init(__NAME##_queue_t *ptQueue, __TYPE *ptBuffer, __PTR_TYPE tSize) \
{                                                                           \
    CLASS(__NAME##_queue_t) *ptQ = (CLASS(__NAME##_queue_t) *)ptQueue;      \
    if (NULL == ptQueue || NULL == ptBuffer || 0 == tSize) {                \
        return false;                                                       \
    }                                                                       \
                                                                            \
    ptQ->ptBuffer = ptBuffer;                                               \
    ptQ->tSize = tSize;                                                     \
    ptQ->tHead = 0;                                                         \
    ptQ->tTail = 0;                                                         \
    ptQ->tCounter = 0;                                                      \
                                                                            \
    return true;                                                            \
}                                                                           \
                                                                            \
bool __NAME##_enqueue(__NAME##_queue_t *ptQueue, __TYPE tObj)               \
{                                                                           \
    bool bResult = false;                                                   \
    CLASS(__NAME##_queue_t) *ptQ = (CLASS(__NAME##_queue_t) *)ptQueue;      \
    if (NULL == ptQ) {                                                      \
        return false;                                                       \
    }                                                                       \
                                                                            \
    __ATOM_ACCESS(                                                          \
        do {                                                                \
            if ((ptQ->tHead == ptQ->tTail) && (0 != ptQ->tCounter)) {       \
                break;                                                      \
            }                                                               \
                                                                            \
            ptQ->ptBuffer[ptQ->tTail++] = tObj;                             \
            if (ptQ->tTail >= ptQ->tSize) {                                 \
                ptQ->tTail = 0;                                             \
            }                                                               \
            ptQ->tCounter++;                                                \
            bResult = true;                                                 \
        } while (false);                                                    \
    )                                                                       \
                                                                            \
    return bResult;                                                         \
}                                                                           \
                                                                            \
bool __NAME##_queue_peek(__NAME##_queue_t *ptQueue, __TYPE *ptObj)          \
{                                                                           \
    bool bResult = false;                                                   \
    CLASS(__NAME##_queue_t) *ptQ = (CLASS(__NAME##_queue_t) *)ptQueue;      \
    if (NULL == ptQ) {                                                      \
        return false;                                                       \
    }                                                                       \
                                                                            \
    __ATOM_ACCESS(                                                          \
        do {                                                                \
            if ((ptQ->tHead == ptQ->tTail) && (!ptQ->tCounter)) {           \
                break;                                                      \
            }                                                               \
            if (NULL != ptObj) {                                            \
                *ptObj = ptQ->ptBuffer[ptQ->tHead];                         \
            }                                                               \
            bResult = true;                                                 \
        } while (false);                                                    \
    )                                                                       \
                                                                            \
    return bResult;                                                         \
}                                                                           \
bool __NAME##_dequeue(__NAME##_queue_t *ptQueue, __TYPE *ptObj)             \
{                                                                           \
    bool bResult = false;                                                   \
    CLASS(__NAME##_queue_t) *ptQ = (CLASS(__NAME##_queue_t) *)ptQueue;      \
    if (NULL == ptQ) {                                                      \
        return false;                                                       \
    }                                                                       \
                                                                            \
    __ATOM_ACCESS(                                                          \
        do {                                                                \
            if ((ptQ->tHead == ptQ->tTail) && (!ptQ->tCounter)) {           \
                break;                                                      \
            }                                                               \
            if (NULL != ptObj) {                                            \
                *ptObj = ptQ->ptBuffer[ptQ->tHead];                         \
            }                                                               \
            ptQ->tHead++;                                                   \
            if (ptQ->tHead >= ptQ->tSize) {                                 \
                ptQ->tHead = 0;                                             \
            }                                                               \
            ptQ->tCounter--;                                                \
            bResult = true;                                                 \
        } while (false);                                                    \
    )                                                                       \
                                                                            \
    return bResult;                                                         \
}                                                                           \
bool __NAME##_is_full(__NAME##_queue_t *ptQueue)             \
{                                                                           \
    bool bResult = false;                                                   \
    CLASS(__NAME##_queue_t) *ptQ = (CLASS(__NAME##_queue_t) *)ptQueue;      \
    if (NULL == ptQ) {                                                      \
        return false;                                                       \
    }                                                                       \
                                                                            \
    __ATOM_ACCESS(                                                          \
        do {                                                                \
            if ((ptQ->tHead == ptQ->tTail) && (ptQ->tCounter)) {           \
            bResult = true;                                                 \
            break;                                                      \
            }                                                               \
            \
            } while (false);                                                    \
    )                                                                       \
                                                                            \
    return bResult;                                                         \
}                                                                           \
bool __NAME##_is_empty(__NAME##_queue_t *ptQueue)             \
{                                                                           \
    bool bResult = false;                                                   \
    CLASS(__NAME##_queue_t) *ptQ = (CLASS(__NAME##_queue_t) *)ptQueue;      \
    if (NULL == ptQ) {                                                      \
        return false;                                                       \
    }                                                                       \
                                                                            \
    __ATOM_ACCESS(                                                          \
        do {                                                                \
            if ((ptQ->tHead == ptQ->tTail) && (!ptQ->tCounter)) {           \
            bResult = true;                                                 \
            break;                                                      \
            }                                                               \
            \
            } while (false);                                                    \
    )                                                                       \
                                                                            \
    return bResult;                                                         \
}                                                                           \

#define QUEUE_IS_EMPTY(__NAME, __QUEUE)                                   \
            __NAME##_is_empty((__QUEUE))
            
#define QUEUE_IS_FULL(__NAME, __QUEUE)                                   \
            __NAME##_is_full((__QUEUE))

#define DEF_SAFE_QUEUE(__NAME, __TYPE, __PTR_TYPE, __MUTEX_TYPE)            \
        DEF_QUEUE_EX(__NAME, __TYPE, __PTR_TYPE, __MUTEX_TYPE, SAFE_ATOM_CODE)

#define DEF_SAFE_QUEUE_U8(__NAME, __PTR_TYPE,__MUTEX_TYPE)                  \
        DEF_SAFE_QUEUE(__NAME, uint8_t, __PTR_TYPE, __MUTEX_TYPE)

#define DEF_SAFE_QUEUE_U16(__NAME, __PTR_TYPE, __MUTEX_TYPE)                \
        DEF_SAFE_QUEUE(__NAME, uint16_t, __PTR_TYPE, __MUTEX_TYPE)

#define DEF_SAFE_QUEUE_U32(__NAME, __PTR_TYPE, __MUTEX_TYPE)                \
        DEF_SAFE_QUEUE(__NAME, uint32_t __PTR_TYPE, __MUTEX_TYPE)

#define DEF_QUEUE(__NAME, __TYPE, __PTR_TYPE, __MUTEX_TYPE)                 \
        DEF_QUEUE_EX(__NAME, __TYPE, __PTR_TYPE, __MUTEX_TYPE, NONE_ATOM_ACCESS)

#define DEF_QUEUE_U8(__NAME, __PTR_TYPE,__MUTEX_TYPE)                       \
        DEF_QUEUE(__NAME, uint8_t, __PTR_TYPE, __MUTEX_TYPE)

#define DEF_QUEUE_U16(__NAME, __PTR_TYPE, __MUTEX_TYPE)                     \
        DEF_QUEUE(__NAME, uint16_t, __PTR_TYPE, __MUTEX_TYPE)

#define DEF_QUEUE_U32(__NAME, __PTR_TYPE, __MUTEX_TYPE)                     \
        DEF_QUEUE(__NAME, uint32_t __PTR_TYPE, __MUTEX_TYPE)





/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


#endif
