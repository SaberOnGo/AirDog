
#include "fifo_queue.h"



/* ! 定义了一个专门的byte队列类型，
这个队列用uint16_t作为计数器类型，
所以，可以处理0~65535个元素, 用bool作为临界区变量类型*/
DEF_QUEUE_U16(word_queue, uint16_t, bool)
END_DEF_QUEUE_U16

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

/*! 用刚刚定义的新类型byte_queue来定义一个队列s_tBLQIn, 这里s表示static，说明变量是静态的,_t表示这是自定义数据类
型, BL是Bootloader的缩写, Q是Queue的缩写 In表示这是一个输入队列*/

NO_INIT   PRIVATE QUEUE(word_queue) s_tBLQIn;
//! 这是它的缓冲
NO_INIT PRIVATE uint16_t s_InBuffer[16];

//! 用刚刚定义的新类型byte_queue来定义一个队列s_BLQOut;
//NO_INIT PRIVATE QUEUE(byte_queue) s_tBLQOut;
//! 这是它的缓冲
//NO_INIT PRIVATE uint8_t s_chOutBuffer[64];


//! 用两个队列,In和Out封装成一个管道pipe, 后面的两个函数是管道的一端，
bool pipe_write_word(uint16_t word)
{
      return ENQUEUE(word_queue, &s_tBLQIn, word);
}

bool pipe_is_full(void)
{
    return QUEUE_IS_FULL(word_queue, &s_tBLQIn);

}

bool pipe_is_empty(void)
{
     return QUEUE_IS_EMPTY(word_queue,  &s_tBLQIn);
}

bool pipe_read_word(uint16_t *pWord)
{
      return DEQUEUE(word_queue, &s_tBLQIn, pWord);
}

bool pipe_peek_word(uint16_t * pWord)
{
     return PEEK_QUEUE(word_queue, &s_tBLQIn, pWord);
}


/*! 这是一个不折不扣如何使用队列的例子，亮点是 ENAQUEUE, PEEK_QUEUE和DEQUEUE, SERIAL_IN
? ? 和SERIAL_OUT是串行输入输出设备的接口，你直接认为是串口就好了
 */
 /*
PRIVATE STATE(BootLoader_Task) BEGIN

    do {
        uint8_t chByte;
        if (SERIAL_IN(&chByte)) {
            ENQUEUE(byte_queue, &s_tBLQIn, chByte);
        }
    } while (false);

    do {
        uint8_t chByte;
        if (PEEK_QUEUE(byte_queue, &s_tBLQOut, &chByte)) {
            if (SERIAL_OUT(chByte)) {
                DEQUEUE(byte_queue, &s_tBLQOut, &chByte);
            }
        }
    } while (false);
    ...

    REFLEXIVE_STATE;
END
*/

DEF_QUEUE_U8(byte_queue, uint8_t, bool)
END_DEF_QUEUE_U8

NO_INIT   PRIVATE QUEUE(byte_queue)s_tBQ;

//! 这是它的缓冲
NO_INIT PRIVATE uint8_t s_BQBuff[64];   


bool pipe_write_byte(uint8_t byte)
{
      return ENQUEUE(byte_queue, &s_tBQ, byte);
}

bool pipe_byte_is_full(void)
{
    return QUEUE_IS_FULL(byte_queue, &s_tBQ);

}

bool pipe_byte_is_empty(void)
{
     return QUEUE_IS_EMPTY(byte_queue,  &s_tBQ);
}

bool pipe_read_byte(uint8_t *p)
{
      return DEQUEUE(byte_queue, &s_tBQ, p);
}

bool pipe_peek_byte(uint8_t * p)
{
     return PEEK_QUEUE(byte_queue, &s_tBQ, p);
}

void byte_queue_init(void)
{
     QUEUE_INIT(byte_queue,  &s_tBQ,        s_BQBuff, sizeof(s_BQBuff) / sizeof(s_BQBuff[0]));
}

void sht_queue_init(void)
{
      QUEUE_INIT(word_queue, &s_tBLQIn,    s_InBuffer,  sizeof(s_InBuffer) / sizeof(s_InBuffer[0]));
}

