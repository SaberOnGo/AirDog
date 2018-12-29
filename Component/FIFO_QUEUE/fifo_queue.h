
#ifndef __FIFO_QUEUE_H__
#define __FIFO_QUEUE_H__


#include "T_queue.h"

#if 0
#ifndef bool
typedef enum
{
   e_false = 0,  e_true  = !e_false,
}bool;
#endif
#endif

bool pipe_read_word(uint16_t *pWord);
bool pipe_write_word(uint16_t word);

bool pipe_is_full(void);
bool pipe_is_empty(void);
bool pipe_peek_word(uint16_t * pWord);

bool pipe_write_byte(uint8_t byte);
bool pipe_byte_is_full(void);
bool pipe_byte_is_empty(void);
bool pipe_read_byte(uint8_t *p);
bool pipe_peek_byte(uint8_t * p);

void sht_queue_init(void);
void byte_queue_init(void);


#endif

