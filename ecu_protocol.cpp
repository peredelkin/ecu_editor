#include "ecu_protocol.h"

void ECU_Protocol::init() {
    protocol.read.count = 0;
    protocol.read.count_end = 0;
    protocol.write.count = 0;
    protocol.write.count_end = 0;
}

void ECU_Protocol::read_frame_data(ecu_rw_t *ecu_r,volatile void **data) {
    uint8_t type = ecu_r->frame.cmd_addr.cmd & ECU_DATA_TYPE_MASK;
    uint16_t write_start = (ecu_r->frame.service_data.start / type);
    uint16_t write_count = (ecu_r->frame.service_data.count / type) + 1;
    uint16_t point = 0;
    while (--write_count) {
        switch (type) {
        case ECU_DATA_TYPE_8: ((uint8_t*)(data[ecu_r->frame.cmd_addr.addr]))[write_start + point] =
                ((uint8_t*)(ecu_r->frame.data))[point];
            break;
        case ECU_DATA_TYPE_16: ((uint16_t*)(data[ecu_r->frame.cmd_addr.addr]))[write_start + point] =
                ((uint16_t*)(ecu_r->frame.data))[point];
            break;
        case ECU_DATA_TYPE_32: ((uint32_t*)(data[ecu_r->frame.cmd_addr.addr]))[write_start + point] =
                ((uint32_t*)(ecu_r->frame.data))[point];
            break;
        default:
            return;
        };
        point++;
    }
}

void ECU_Protocol::write_frame_data(volatile void **data,uint8_t cmd,uint16_t addr,uint16_t start,uint8_t count) {
    protocol.write.frame.cmd_addr.cmd = cmd;
    protocol.write.frame.cmd_addr.addr = addr;
    protocol.write.frame.service_data.start = start;
    protocol.write.frame.service_data.count = count;
    uint8_t cmd_type = protocol.write.frame.cmd_addr.cmd & ECU_CMD_MASK;
    switch (cmd_type) {
#ifdef ECU_PROTOCOL_MASTER
    case ECU_CMD_MASTER_WRITE:
#else
    case ECU_CMD_SLAVE_READ:
#endif
    {
        protocol.write.count = ECU_CMD_ADDR_COUNT + ECU_SERVICE_DATA_COUNT;
        *(uint16_t*) (&protocol.write.frame.data[0]) =
                crc16_ccitt((uint8_t*) (&protocol.write.frame), protocol.write.count);
    }
        break;
#ifdef ECU_PROTOCOL_MASTER
    case ECU_CMD_MASTER_READ:
#else
    case ECU_CMD_SLAVE_WRITE:
#endif
    {
        uint8_t data_type = protocol.write.frame.cmd_addr.cmd & ECU_DATA_TYPE_MASK;
        uint16_t read_start = (protocol.write.frame.service_data.start / data_type);
        uint16_t read_count = (protocol.write.frame.service_data.count / data_type) + 1;
        uint16_t point = 0;
        while (--read_count) {
            switch (data_type) {
            case ECU_DATA_TYPE_8: ((uint8_t*)(protocol.write.frame.data))[point] =
                    ((uint8_t*)(data[protocol.write.frame.cmd_addr.addr]))[read_start + point];
                break;
            case ECU_DATA_TYPE_16: ((uint16_t*)(protocol.write.frame.data))[point] =
                    ((uint16_t*)(data[protocol.write.frame.cmd_addr.addr]))[read_start + point];
                break;
            case ECU_DATA_TYPE_32: ((uint32_t*)(protocol.write.frame.data))[point] =
                    ((uint32_t*)(data[protocol.write.frame.cmd_addr.addr]))[read_start + point];
                break;
            default:
                return;
            };
            point++;
        }
        protocol.write.count = ECU_CMD_ADDR_COUNT + ECU_SERVICE_DATA_COUNT + protocol.write.frame.service_data.count;
        *(uint16_t*)(&protocol.write.frame.data[protocol.write.frame.service_data.count]) =
                crc16_ccitt((uint8_t*)(&protocol.write.frame),protocol.write.count);
    }
        break;
    default:
        break;
    };
    protocol.write.count += ECU_CRC_COUNT;
}

void ECU_Protocol::handler(QSerialPort *serial,volatile void **directory) {
    if(protocol.read.count_end != protocol.read.count) {
        if(serial->bytesAvailable() >= (protocol.read.count_end - protocol.read.count)) {
            serial->read(&(reinterpret_cast<char*>(&protocol.read.frame))[protocol.read.count],(protocol.read.count_end - protocol.read.count));
            protocol.read.count = protocol.read.count_end;
            switch (protocol.cmd_type) {
            case ECU_CMD_TYPE_DEF: {
                protocol.cmd_type = (uint8_t) ((protocol.read.frame.cmd_addr.cmd & ECU_CMD_MASK));
                switch (protocol.cmd_type) {
            #ifdef ECU_PROTOCOL_MASTER
                case ECU_CMD_MASTER_WRITE:
            #else
                case ECU_CMD_SLAVE_READ:
            #endif
                    protocol.read.count_end += protocol.read.frame.service_data.count + ECU_CRC_COUNT;
                    break;
            #ifdef ECU_PROTOCOL_MASTER
                case ECU_CMD_MASTER_READ:
            #else
                case ECU_CMD_SLAVE_WRITE:
            #endif
                    protocol.read.count_end += ECU_CRC_COUNT;
                    break;
                default:
                    break;
                };
            }
                break;
        #ifdef ECU_PROTOCOL_MASTER
            case ECU_CMD_MASTER_WRITE:
        #else
            case ECU_CMD_SLAVE_READ:
        #endif
            {
                protocol.crc_read = *(uint16_t*)(&protocol.read.frame.data[protocol.read.frame.service_data.count]);
                protocol.crc_calc = crc16_ccitt((uint8_t*)(&protocol.read.frame),protocol.read.count_end - ECU_CRC_COUNT);
                if(protocol.crc_read == protocol.crc_calc) {
                    read_frame_data(&protocol.read,directory);
                } else {
                    qDebug() << "CRC Error";
                }
            }
                break;
        #ifdef ECU_PROTOCOL_MASTER
            case ECU_CMD_MASTER_READ:
        #else
            case ECU_CMD_SLAVE_WRITE:
        #endif
            {
                protocol.crc_read = *(uint16_t*)(&protocol.read.frame.data[0]);
                protocol.crc_calc = crc16_ccitt((uint8_t*)(&protocol.read.frame),protocol.read.count_end - ECU_CRC_COUNT);
                if(protocol.crc_read == protocol.crc_calc) {
                    write_frame_data(
                            directory,protocol.read.frame.cmd_addr.cmd,
                            protocol.read.frame.cmd_addr.addr,
                            protocol.read.frame.service_data.start,
                            protocol.read.frame.service_data.count);
                    serial->write(reinterpret_cast<char*>(&protocol.write.frame),protocol.write.count);
                } else {
                    qDebug() << "CRC Error";
                }
            }
                break;
            default:
                break;
            };
        }
    } else {
        protocol.cmd_type = ECU_CMD_TYPE_DEF;
        protocol.read.count = 0;
        protocol.read.count_end = ECU_CMD_ADDR_COUNT + ECU_SERVICE_DATA_COUNT;
    }
}

void ECU_Protocol::send_frame(QSerialPort *serial) {
    serial->write(reinterpret_cast<char*>(&protocol.write.frame),protocol.write.count);
}
