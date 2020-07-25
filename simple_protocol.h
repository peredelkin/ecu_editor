#ifndef SIMPLE_PROTOCOL_H
#define SIMPLE_PROTOCOL_H

#include "crc16_ccitt.h"

#define SIMPLE_PROTOCOL_LINK_HEAD_COUNT         4
#define SIMPLE_PROTOCOL_LINK_CRC_COUNT          2
#define SIMPLE_PROTOCOL_LINK_FRAME_COUNT        256
#define SIMPLE_PROTOCOL_LINK_DATA_COUNT (SIMPLE_PROTOCOL_LINK_FRAME_COUNT - SIMPLE_PROTOCOL_LINK_HEAD_COUNT) //crc included

#define SIMPLE_PROTOCOL_LINK_ID_DEF             0
#define SIMPLE_PROTOCOL_LINK_ID_DATA_HEAD       1

#define SIMPLE_PROTOCOL_NET_DATA_HEAD_COUNT     7

#pragma pack(1)
typedef struct {
    uint8_t addr;
    uint8_t id;
    uint16_t count;
} simple_protocol_frame_head_t;

typedef struct {
    simple_protocol_frame_head_t head;
    uint8_t data[SIMPLE_PROTOCOL_LINK_DATA_COUNT];
} simple_protocol_frame_t;

typedef struct {
    void *port;
    void (*transfer)(void* port,uint8_t* data,uint16_t count);
} simple_protocol_port_t;

typedef struct {
    uint8_t id;
    uint16_t addr;
    uint16_t start;
    uint16_t count;
} simple_protocol_data_head_t;

typedef struct {
    simple_protocol_data_head_t data_head;
    simple_protocol_frame_t frame;
    simple_protocol_port_t device;
} simple_protocol_transfer_t;

typedef struct {
    uint8_t addr;
    uint8_t id;
    uint16_t count_current;
    uint16_t count_remain;
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
    simple_protocol_callback_t crc_err;
    simple_protocol_callback_t link_id_default;
} simple_protocol_link_layer_t;
#pragma pack()

extern void simple_protocol_service_init(simple_protocol_link_layer_t* link);
extern void simple_protocol_handler(simple_protocol_link_layer_t* link,int16_t bytes_available,volatile void** addr_ptrs);
extern void simple_protocol_link_send_frame(simple_protocol_link_layer_t* link,uint8_t addr,uint8_t id,uint16_t count,void* data);

#endif // SIMPLE_PROTOCOL_H
