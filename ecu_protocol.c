#include "ecu_protocol.h"
#include <string.h>

void ecu_protocol_init(ecu_protocol_t* protocol) {
    memset(protocol,0,sizeof (ecu_protocol_t));
}

void ecu_protocol_service_init(ecu_protocol_t* protocol) {
    protocol->service.id = ECU_ID_DEFENITION;
    protocol->service.count = 0;
    protocol->service.crc_calc = 0;
    protocol->service.crc_read = 0;
    protocol->service.count_end = ECU_PROTOCOL_HEAD_COUNT;
}

void ecu_protocol_handler(ecu_protocol_t* protocol,uint8_t bytes_available) {
    if(protocol->service.count_end != protocol->service.count) { //данные не обработаны
        if(bytes_available >= (protocol->service.count_end - protocol->service.count)) { //есть новая пачка данных
            protocol->read.device.transfer(protocol->read.device.port,
                    &((uint8_t*)(&protocol->read.frame))[protocol->service.count],
                    (protocol->service.count_end - protocol->service.count)); //чтение новых данных
            protocol->service.count = protocol->service.count_end; //сдвиг точки записи фрейма
            if(protocol->service.id) { //id определен
                protocol->service.crc_read = *(uint16_t*)(&protocol->read.frame.data[protocol->read.frame.head.count]); //чтение контрольной суммы из фрейма
                protocol->service.crc_calc = crc16_ccitt((uint8_t*)(&protocol->read.frame),protocol->service.count_end - ECU_PROTOCOL_CRC_COUNT); //расчет контрольной суммы фрейма
                if(protocol->service.crc_read == protocol->service.crc_calc) { //контрольная сумма совпадает
                    if(protocol->crc_correct.callback) { //адрес указателя не равен нулю
                        protocol->crc_correct.callback(protocol->crc_correct.user_pointer,protocol); //вызов функции обработчик
                    }
                } else {
                    if(protocol->crc_incorrect.callback) { //адрес указателя не равен нулю
                        protocol->crc_incorrect.callback(protocol->crc_incorrect.user_pointer,protocol); //вызов функции обработчик
                    }
                }
            } else {
                protocol->service.id = protocol->read.frame.head.id; //определение id
                protocol->service.count_end += protocol->read.frame.head.count + ECU_PROTOCOL_CRC_COUNT; //определение оставшейся длины фрейма
            }
        }
    } else {
        ecu_protocol_service_init(protocol); //инициализация приема нового фрейма
    }
}
