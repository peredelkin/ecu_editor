#ifndef ECU_PROTOCOL_H
#define ECU_PROTOCOL_H

#include <stdint.h>
#include <QSerialPort>

extern "C" {
    #include <crc16_ccitt.h>
}

#define ECU_CMD_ADDR_COUNT 3
#define ECU_SERVICE_DATA_COUNT 3
#define ECU_DATA_COUNT 128
#define ECU_CRC_COUNT 2
#define ECU_FULL_FRAME_COUNT (ECU_CMD_ADDR_COUNT+ECU_SERVICE_DATA_COUNT+ECU_DATA_COUNT+ECU_CRC_COUNT)

#define ECU_CMD_TYPE_DEF	((uint8_t)0x00)
#define ECU_CMD_WRITE		((uint8_t)0x10)
#define ECU_CMD_READ		((uint8_t)0x20)
#define ECU_CMD_MASK		((uint8_t)0xF0)

#define ECU_DATA_TYPE_8		((uint8_t)0x01)
#define ECU_DATA_TYPE_16	((uint8_t)0x02)
#define ECU_DATA_TYPE_32	((uint8_t)0x04)
#define ECU_DATA_TYPE_MASK	((uint8_t)0x0F)

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
    uint8_t data[ECU_DATA_COUNT+ECU_CRC_COUNT];
} ecu_frame_t;

typedef struct {
    uint16_t count;
    uint16_t count_end;
    ecu_frame_t frame;
} ecu_rw_t;

typedef struct {
    ecu_rw_t read;
    ecu_rw_t write;
    uint8_t cmd_type;
    uint16_t crc_calc;
    uint16_t crc_read;
} ecu_protocol_t;
#pragma pack()

class ECU_Protocol {

public:
    void read_frame_data(ecu_rw_t *ecu_r,volatile void **data);
    void write_frame_data(volatile void **data,uint8_t cmd,uint16_t addr,uint16_t start,uint8_t count);
    void handler(QSerialPort *serial,volatile void **directory);
    void send_frame(QSerialPort *serial);
private:
    ecu_protocol_t protocol;
};

#endif // ECU_PROTOCOL_H
