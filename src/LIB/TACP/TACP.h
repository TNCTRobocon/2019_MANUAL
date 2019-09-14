#ifndef TACP_H_
#define TACP_H_

#include <stdint.h>
#include <stdbool.h>

enum{
    SND=0,
    RET,
    RTY,
    RST
};

typedef struct TACP_PACKET{
    uint8_t pid;
    uint8_t length;
    bool flags[4];
    uint8_t* data;
}TACP_PACKET_t;

typedef struct TACP{
    uint8_t (*serial_getc)(void);
    unsigned int (*serial_available)(void);
    void (*serial_putc)(uint8_t);
    unsigned int buffer_size;
    unsigned int timeout;

    uint8_t* buffer;
    unsigned int pos;
    unsigned int goal;
    unsigned int time;
    TACP_PACKET_t rcpac;
    bool func_running;
    bool receiving;
}TACP_t;

void TACP_init(TACP_t* tacp);
int TACP_update(TACP_t* tacp);
int TACP_put(TACP_t* tacp,TACP_PACKET_t* pac);

#endif