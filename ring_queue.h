#include <assert.h>
#include <string.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

typedef unsigned char uchar;

#define CAN_WRITE 0x00
#define CAN_READ 0x01
#define READING 0x02
#define WRITING 0x03

typedef struct tag
{
    uchar tag_value;
} TAG;


class RingQueue
{
public:
    RingQueue(int nmemb,int size)
    :_nmemb(nmemb),_size(size),
    _read_now(0),_write_now(0) {
        if ( nmemb <= 0 || size <=0 ) {
            assert(0);
        }
        _queue_p = NULL;
        _queue_p = new uchar[ nmemb * (sizeof(TAG) + size)];
        memset(_queue_p,0,nmemb * (sizeof(TAG) + size));

        socketpair(AF_UNIX, SOCK_SEQPACKET, 0, notify_fd);
    }

    ~RingQueue() {
        if (_queue_p) 
            delete []_queue_p;
    }

    uchar* solo_read() {
        uchar * g_p = 0;
        TAG * tag_p = 0;
        uchar *user_data = 0;

        g_p = queue_peek_nth(_queue_p,_read_now);
        tag_p = (TAG *)g_p;
        if (tag_p->tag_value == CAN_READ)
        {
            user_data = (uchar *)g_p + sizeof(TAG);
            tag_p->tag_value = READING;
        }
        return user_data;
    }

    void solo_read_over() {
        uchar * g_p = 0;
        TAG * tag_p = 0;

        g_p = queue_peek_nth(_queue_p,_read_now);
        tag_p = (TAG *)g_p;
        if (tag_p->tag_value == READING)
        {
            tag_p->tag_value = CAN_WRITE;
            _read_now = (_read_now + 1)% _nmemb; // once full back to index 0 
        }
    }

    uchar * solo_write() {
        uchar * g_p = 0;
        TAG * tag_p = 0;
        uchar *user_data = 0;

        g_p = queue_peek_nth(_queue_p,_write_now);
        tag_p = (TAG *)g_p;
        if (tag_p->tag_value == CAN_WRITE)
        {  
            user_data = (uchar *)g_p + sizeof(TAG);
            tag_p->tag_value = WRITING;
        }                
        return user_data;
    }

    void solo_write_over() {
        uchar * g_p = 0;
        TAG * tag_p = 0;

        g_p = queue_peek_nth(_queue_p,_write_now);
        tag_p = (TAG *)g_p;
        if (tag_p->tag_value == WRITING)
        {
            tag_p->tag_value = CAN_READ;
            _write_now = (_write_now + 1)% _nmemb;
        }
    }

private:
    uchar *queue_peek_nth(uchar *queue_p, int pos) {
        uchar *rst = 0;
        if (queue_p && pos < _nmemb)
        {
            rst = queue_p + pos * (sizeof(TAG) + _size);
        }
        return rst;
    }

private:
    int _nmemb;
    int _size;
    uchar* _queue_p;
    volatile int _read_now;
    volatile int _write_now;
public:
    int notify_fd[2];
};
