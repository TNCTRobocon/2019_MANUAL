#include "TACP.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "crc8.h"

void TACP_init(TACP_t* tacp){
    tacp->buffer=(uint8_t*)malloc(tacp->buffer_size*sizeof(uint8_t));
    tacp->pos=0;
    tacp->goal=0;
    tacp->func_running=false;
    tacp->receiving=false;
}

static void TACP_get(TACP_t* tacp,TACP_PACKET_t* pac){
    int i;
    free(pac->data);
    pac->pid=tacp->buffer[0]>>4;
    pac->flags[RST]=tacp->buffer[0]&0b1000;
    pac->flags[RTY]=tacp->buffer[0]&0b100;
    pac->flags[RET]=tacp->buffer[0]&0b10;
    pac->flags[SND]=tacp->buffer[0]&0b1;
    pac->length=tacp->buffer[1];
    pac->data=(uint8_t*)malloc(pac->length);
    for(i=0;i<pac->length;i++){
        pac->data[i]=tacp->buffer[i+2];
    }
}

int TACP_update(TACP_t* tacp){
    uint8_t temp;

    tacp->time++;
    if(tacp->func_running){
        return -1;
    }
    tacp->func_running=true;

    while(tacp->serial_available()){
        temp=tacp->serial_getc();
        if(tacp->receiving){
            tacp->buffer[tacp->pos]=temp;
            if(tacp->pos==1){
                tacp->goal=temp+2;
            }else if(tacp->pos==tacp->goal){
                tacp->receiving=false;
                if(temp!=CRC8_get(tacp->buffer,tacp->goal)){
                    break;
                }
                TACP_get(tacp,&tacp->rcpac);
                tacp->func_running=false;
                return 1;
            }
            tacp->pos++;
        }else if(temp!=0x00){
            tacp->receiving=true;
            tacp->time=0;
            tacp->buffer[0]=temp;
            tacp->pos=1;
        }
    }
    if(tacp->time>=tacp->timeout){
        tacp->receiving=false;
    }
    
    tacp->func_running=false;
    return 0;
}

int TACP_put(TACP_t* tacp,TACP_PACKET_t* pac){
    int i;
    uint8_t* temp=(uint8_t*)malloc((pac->length+3)*sizeof(uint8_t));
    if(tacp->func_running){
        return -1;
    }
    tacp->func_running=true;

    temp[0]=pac->pid<<4|pac->flags[RST]*0b1000|pac->flags[RTY]*0b100|pac->flags[RET]*0b10|pac->flags[SND]*0b1;
    temp[1]=pac->length;
    for(i=0;i<pac->length;i++){
        temp[i+2]=pac->data[i];
    }
    temp[pac->length+2]=CRC8_get(temp,pac->length+2);
    for(i=0;i<=(pac->length+2);i++){
        tacp->serial_putc(temp[i]);
    }
    free(temp);

    tacp->func_running=false;
    return 0;
}