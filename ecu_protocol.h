#ifndef ECU_PROTOCOL_H
#define ECU_PROTOCOL_H

#include "crc16_ccitt.h"

#define ECU_PROTOCOL_FRAME_COUNT 256
#define ECU_PROTOCOL_HEAD_COUNT 2
#define ECU_PROTOCOL_DATA_COUNT (ECU_PROTOCOL_FRAME_COUNT - ECU_PROTOCOL_HEAD_COUNT) //включая CRC
#define ECU_PROTOCOL_CRC_COUNT 2

#define ECU_ID_DEFENITION               ((uint8_t)0x00)

#define ECU_ID_FILE_READ                ((uint8_t)0x01)
#define ECU_ID_FILE_WRITE               ((uint8_t)0x02)

#define ECU_ID_GROUP_DATA_READ          ((uint8_t)0x03)
#define ECU_ID_GROUP_DATA_WRITE         ((uint8_t)0x04)

#define ECU_ID_SINGLE_BY_MASK_READ      ((uint8_t)0x05)
#define ECU_ID_SINGLE_BY_MASK_SET       ((uint8_t)0x06)
#define ECU_ID_SINGLE_BY_MASK_CLEAR     ((uint8_t)0x07)
#define ECU_ID_SINGLE_BY_MASK_SWITCH    ((uint8_t)0x08)

#pragma pack(1)

typedef struct {
    uint8_t id;
    uint8_t count;
} ecu_protocol_head_t;

typedef struct {
    ecu_protocol_head_t head;
    uint8_t data[ECU_PROTOCOL_DATA_COUNT];
} ecu_protocol_frame_t;

typedef struct {
    void *port;
    void (*transfer)(void* port,uint8_t* data,uint16_t count);
} ecu_protocol_port_t;

typedef struct {
    ecu_protocol_frame_t frame;
    ecu_protocol_port_t device;
} ecu_protocol_transfer_t;

typedef struct {
    uint8_t id;
    uint16_t count;
    uint16_t count_end;
} ecu_protocol_service_t;

typedef struct {
    ecu_protocol_transfer_t read;
    ecu_protocol_transfer_t write;
    ecu_protocol_service_t service;
} ecu_protocol_t;

#pragma pack()

#endif // ECU_PROTOCOL_H
