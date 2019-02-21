

#include "ring_buff_array.h"

#define BUF_LEN 5
ring_buf_t rbuff;
char buf[BUF_LEN];

int main()
{
    ring_buf_Init(&rbuff, buf, sizeof(buf));

    printf("1、逐个读取数据测试\r\n");
    int length = 5;
    for (int i = 0; i < length; i++) {
        ring_buf_put(&rbuff, i);
    }

    char data;
    length = 5;
    for (int i = 0; i < length; i++) {
        ring_buf_get(&rbuff, &data, 1); //从BUFF读取1个字节
        printf("每次读取1个字节：buf pop : %d \r\n", data);  //打印该字节
    }

    printf("\n2、一次性读取测试\r\n");
    length = 5;
    for (int i = 0; i < length; i++) {
        ring_buf_put(&rbuff, '1' + i);
    }
    char data2[11] = { 0 };
    ring_buf_get(&rbuff, data2, 5);
    printf("一次性读取5个字节：buf pop : %s \r\n", data2);  //打印该字节

    printf("\n3、放入超过缓冲区长度（BUF_LEN+1）数据测试：\r\n");
    length = BUF_LEN + 1;
    for (int i = 0; i < length; i++){
        ring_buf_put(&rbuff, '1'+i);
    }

    char data3[BUF_LEN+1] = { 0 };
    ring_buf_get(&rbuff, data3, BUF_LEN + 1);
    printf("一次性读取（BUF_LEN+1）个字节测试：buf pop : %s \r\n", data3);  //打印该字节

    //4、测试读取空缓冲区
    printf("\n4、读取空缓冲区测试：\r\n");
    ring_buf_get(&rbuff, data3, 2); //从BUFF读取2个字节

    return 0;
}
