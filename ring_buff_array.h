

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char  *buf;             //指向队列数组的指针
    unsigned int length;    //长度
    unsigned int head;      //队头
    unsigned int tail;      //队尾
    int fill_cnt;           //队列计数
} ring_buf_t;

int ring_buf_init(ring_buf_t* r, char array[], size_t len)
{
    if (len <2 || array==NULL){
        return false;
    }

    r->buf = array;
    r->length = len;
    r->fill_cnt = 0;
    r->head = r->tail = 0;
    return true;
}

int ring_buf_put(ring_buf_t* r, char data)
{
    //当tail+1等于head时，说明队列已满
    if (r->fill_cnt >= r->length) {
        printf("BUF FULL!\n");
        return false;                  // 如果缓冲区满了，则返回错误
    }

    r->buf[r->tail] = data;
    r->tail++;
    r->fill_cnt++;
    //计算tail是否超出数组范围，如果超出则自动切换到0
    r->tail = r->tail % r->length;
    return true;
}

int ring_buf_get(ring_buf_t* r, char *c, unsigned int length)
{
    //当tail等于head时，说明队列空
    if (r->fill_cnt<=0) {
        printf("BUF EMPTY!\n");
        return false;
    }

    //最多只能读取r->length长度数据
    if (r->length < length) {
        length = r->length;
    }

    for (int i = 0; i<length; i++) {
        r->fill_cnt--;
        *c = r->buf[r->head++];                 // 返回数据给*c
        *c++;
        //计算head自加后的下标是否超出数组范围，如果超出则自动切换到0
        r->head = r->head % r->length;
    }

    return true;
}

