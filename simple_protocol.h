#ifndef ECU_PROTOCOL_H
#define ECU_PROTOCOL_H

#include "crc16_ccitt.h"

#define SIMPLE_PROTOCOL_LINK_HEAD_COUNT 3
#define SIMPLE_PROTOCOL_LINK_CRC_COUNT 2
#define SIMPLE_PROTOCOL_LINK_FRAME_COUNT 255
#define SIMPLE_PROTOCOL_LINK_DATA_COUNT (SIMPLE_PROTOCOL_LINK_FRAME_COUNT - SIMPLE_PROTOCOL_LINK_HEAD_COUNT) //crc included

#define SIMPLE_PROTOCOL_ID_DEF                      ((uint8_t)0)
#define SIMPLE_PROTOCOL_ID_READ                     ((uint8_t)1)
#define SIMPLE_PROTOCOL_ID_CMD_WRITE                ((uint8_t)2)
#define SIMPLE_PROTOCOL_ID_CMD_READ                 ((uint8_t)3)
#define SIMPLE_PROTOCOL_ID_CMD_ACK                  ((uint8_t)4)

#define SIMPLE_PROTOCOL_ID_RW_HEAD_COUNT            ((uint8_t)5)

#pragma pack(1)
typedef struct {
    uint8_t addr;
    uint8_t id;
    uint8_t count;
} simple_protocol_head_t;

typedef struct {
    simple_protocol_head_t head;
    uint8_t data[SIMPLE_PROTOCOL_LINK_DATA_COUNT];
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
    simple_protocol_callback_t crc_err;
    simple_protocol_callback_t data_received;
    simple_protocol_callback_t data_transmitted;
    simple_protocol_callback_t ack_received;
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
extern void simple_protocol_cmd_read(simple_protocol_link_layer_t* link,uint8_t device_addr,uint16_t addr,uint16_t start,uint8_t count);
extern void simple_protocol_cmd_write(simple_protocol_link_layer_t* link,uint8_t device_addr,uint16_t addr,uint16_t start,uint8_t count);

#endif // ECU_PROTOCOL_H
