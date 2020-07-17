#ifndef ECU_PROTOCOL_H
#define ECU_PROTOCOL_H

#include "crc16_ccitt.h"

#define ECU_PROTOCOL_HEAD_COUNT 3
#define ECU_PROTOCOL_CRC_COUNT 2
#define ECU_PROTOCOL_FRAME_COUNT 255
#define ECU_PROTOCOL_DATA_COUNT (ECU_PROTOCOL_FRAME_COUNT - ECU_PROTOCOL_HEAD_COUNT) //crc included

#define ECU_SESSION_LAYER_ID_DEF    ((uint8_t)0)
#define ECU_SESSION_LAYER_ID_READ   ((uint8_t)1)
#define ECU_SESSION_LAYER_ID_WRITE  ((uint8_t)2)
#define ECU_SESSION_LAYER_ID_ACK    ((uint8_t)3)
#define ECU_SESSION_LAYER_ID_RST    ((uint8_t)4)
#define ECU_SESSION_LAYER_ID_FIN    ((uint8_t)5)

#define SIMPLE_PROTOCOL_ID_RW_COUNT ((uint8_t)5)

#pragma pack(1)
typedef struct {
    uint8_t addr;
    uint8_t id;
    uint8_t count;
} simple_protocol_head_t;

typedef struct {
    simple_protocol_head_t head;
    uint8_t data[ECU_PROTOCOL_DATA_COUNT];
} simple_protocol_frame_t;

typedef struct {
    void *port;
    void (*transfer)(void* port,uint8_t* data,uint16_t count);
} simple_protocol_port_t;

typedef struct {
    simple_protocol_frame_t frame;
    simple_protocol_port_t device;
} simple_protocol_transfer_t;

typedef struct {
    uint8_t addr; //0 if master
    uint8_t id;
    uint16_t count;
    uint16_t count_end;
    uint16_t crc_read;
    uint16_t crc_calc;
} simple_protocol_service_t;

typedef struct {
    void *user_pointer;
    void (*callback)(void *user_pointer,void *protocol);
} simple_protocol_callback_t;

typedef struct {
    simple_protocol_transfer_t read;
    simple_protocol_transfer_t write;
    simple_protocol_service_t service;
} simple_protocol_link_layer_t;

typedef struct {
    uint16_t addr;
    uint16_t start;
    uint8_t count;
} simple_protocol_id_rw_t;
#pragma pack()

extern void simple_protocol_init(simple_protocol_link_layer_t* link);
extern void simple_protocol_service_init(simple_protocol_link_layer_t* link);
extern void simple_protocol_handler(simple_protocol_link_layer_t* link,uint8_t bytes_available,volatile void** addr_ptrs);
extern void simple_protocol_send_frame(simple_protocol_link_layer_t* link,uint8_t addr,uint8_t id,uint8_t count,void* data);

#endif // ECU_PROTOCOL_H
