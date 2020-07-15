#include "ecu_protocol.h"

void ecu_protocol_service_init(ecu_protocol_t* protocol) {
    protocol->service.id = ECU_ID_DEFENITION;
    protocol->service.count = 0;
    protocol->service.crc_calc = 0;
    protocol->service.crc_read = 0;
    protocol->service.count_end = ECU_PROTOCOL_HEAD_COUNT;
}

void ecu_protocol_handler(ecu_protocol_t* protocol,uint8_t bytes_available,volatile void **directory) {
    if(protocol->service.count_end != protocol->service.count) {
        if(bytes_available >= (protocol->service.count_end - protocol->service.count)) {
            protocol->read.device.transfer(protocol->read.device.port,
                    &((uint8_t*)(&protocol->read.frame))[protocol->service.count],
                    (protocol->service.count_end - protocol->service.count));
            protocol->service.count = protocol->service.count_end;
            if(protocol->service.id) {
                protocol->service.crc_read = *(uint16_t*)(&protocol->read.frame.data[protocol->read.frame.head.count]);
                protocol->service.crc_calc = crc16_ccitt((uint8_t*)(&protocol->read.frame),protocol->service.count_end - ECU_PROTOCOL_CRC_COUNT);
                if(protocol->service.crc_read == protocol->service.crc_calc) {
                    if(protocol->crc_correct.callback) {
                        protocol->crc_correct.callback(protocol->crc_correct.user_pointer,protocol);
                    }
                } else {
                    if(protocol->crc_incorrect.callback) {
                        protocol->crc_incorrect.callback(protocol->crc_incorrect.user_pointer,protocol);
                    }
                }
            } else {
                protocol->service.id = protocol->read.frame.head.id;
                protocol->service.count_end += protocol->read.frame.head.count + ECU_PROTOCOL_CRC_COUNT;
            }
        }
    } else {
        ecu_protocol_service_init(protocol);
    }
}
