#ifndef ECU_PROTOCOL_H
#define ECU_PROTOCOL_H

#include <stdint.h>
extern "C" {
    #include <crc16_ccitt.h>
}

#define ECU_CMD_ADDR_COUNT 3
#define ECU_SERVICE_DATA_COUNT 3
#define ECU_DATA_COUNT 256
#define ECU_CRC_COUNT 2
#define ECU_FULL_FRAME_COUNT (ECU_CMD_ADDR_COUNT+ECU_SERVICE_DATA_COUNT+ECU_DATA_COUNT+ECU_CRC_COUNT)

typedef uint8_t ecu_data_t;
typedef uint32_t ecu_crc_t;

#pragma pack(1)
typedef struct {
    uint8_t cmd;
    uint16_t addr;
} ecu_cmd_addr_t;

typedef struct {
    uint16_t start;
    uint8_t count;
} ecu_service_data_t;

typedef struct {
    ecu_cmd_addr_t cmd_addr;
    ecu_service_data_t service_data;
    ecu_data_t data[ECU_DATA_COUNT+ECU_CRC_COUNT];
} ecu_frame_t;
#pragma pack()

#endif // ECU_PROTOCOL_H
