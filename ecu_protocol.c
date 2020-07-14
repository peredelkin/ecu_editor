#include "ecu_protocol.h"

void ecu_protocol_id_definition(ecu_protocol_t* protocol) {
    protocol->service.id = protocol->read.frame.head.id;
    protocol->service.count_end += protocol->read.frame.head.count + ECU_PROTOCOL_CRC_COUNT;
}

void ecu_protocol_handler(ecu_protocol_t* protocol,uint8_t bytes_available,volatile void **directory) {
    if(protocol->service.count_end != protocol->service.count) {
        if(bytes_available >= (protocol->service.count_end - protocol->service.count)) {
            protocol->read.device.transfer(protocol->read.device.port,
                    &((uint8_t*)(&protocol->read.frame))[protocol->service.count],
                    (protocol->service.count_end - protocol->service.count));
            protocol->service.count = protocol->service.count_end;
            if(protocol->service.id) {

            } else {
                ecu_protocol_id_definition(protocol);
            }
        }
    } else {
        protocol->service.id = ECU_ID_DEFENITION;
        protocol->service.count = 0;
        protocol->service.count_end = ECU_PROTOCOL_HEAD_COUNT;
    }
}
