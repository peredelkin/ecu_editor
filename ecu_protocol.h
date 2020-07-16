#ifndef ECU_PROTOCOL_H
#define ECU_PROTOCOL_H

#include "crc16_ccitt.h"

#define ECU_PROTOCOL_HEAD_COUNT 3
#define ECU_PROTOCOL_CRC_COUNT 2
#define ECU_PROTOCOL_FRAME_COUNT 255
#define ECU_PROTOCOL_DATA_COUNT (ECU_PROTOCOL_FRAME_COUNT - ECU_PROTOCOL_HEAD_COUNT) //crc included

#define ECU_ID_DEF                      ((uint8_t)0x00)
#define ECU_ID_READ                     ((uint8_t)0x01)
#define ECU_ID_WRITE                    ((uint8_t)0x02)
#define ECU_ID_ACK                      ((uint8_t)0x03)
#define ECU_ID_RST                      ((uint8_t)0x04)
#define ECU_ID_FIN                      ((uint8_t)0x05)

#pragma pack(1)
typedef struct {
    uint8_t addr;
    uint8_t id;
    uint8_t count;
} ecu_data_link_layer_head_t;

typedef struct {
    ecu_data_link_layer_head_t head;
    uint8_t data[ECU_PROTOCOL_DATA_COUNT];
} ecu_data_link_layer_frame_t;

typedef struct {
    void *port;
    void (*transfer)(void* port,uint8_t* data,uint16_t count);
} ecu_data_link_layer_port_t;

typedef struct {
    ecu_data_link_layer_frame_t frame;
    ecu_data_link_layer_port_t device;
} ecu_data_link_layer_transfer_t;

typedef struct {
    uint8_t addr; //0 if master
    uint8_t id;
    uint16_t count;
    uint16_t count_end;
    uint16_t crc_read;
    uint16_t crc_calc;
} ecu_data_link_layer_service_t;

typedef struct {
    void *user_pointer;
    void (*callback)(void *user_pointer,void *protocol);
} ecu_data_link_layer_callback_t;

typedef struct {
    ecu_data_link_layer_transfer_t read;
    ecu_data_link_layer_transfer_t write;
    ecu_data_link_layer_service_t service;
} ecu_data_link_layer_t;
#pragma pack()

extern void ecu_data_link_layer_init(ecu_data_link_layer_t* link);
extern void ecu_data_link_layer_service_init(ecu_data_link_layer_t* link);
extern void ecu_data_link_layer_handler(ecu_data_link_layer_t* link,uint8_t bytes_available);
extern void ecu_data_link_layer_send_frame(ecu_data_link_layer_t* link,uint8_t addr,uint8_t id,uint8_t count,void* data);

#endif // ECU_PROTOCOL_H
