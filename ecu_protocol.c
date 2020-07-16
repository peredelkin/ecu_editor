#include "ecu_protocol.h"
#include <string.h>

void ecu_data_link_layer_init(ecu_data_link_layer_t* link) {
    memset(link,0,sizeof (ecu_data_link_layer_t));
}

void ecu_data_link_layer_service_init(ecu_data_link_layer_t* link) {
    link->service.id = ECU_ID_DEFENITION;
    link->service.count = 0;
    link->service.crc_calc = 0;
    link->service.crc_read = 0;
    link->service.count_end = ECU_PROTOCOL_HEAD_COUNT;
}

void ecu_data_link_layer_parse_frame(ecu_data_link_layer_t* link) {
    memcpy(&link->service.crc_read,&link->read.frame.data[link->read.frame.head.count],ECU_PROTOCOL_CRC_COUNT); //чтение контрольной суммы из фрейма
    link->service.crc_calc = crc16_ccitt((uint8_t*)(&link->read.frame),link->service.count_end - ECU_PROTOCOL_CRC_COUNT); //расчет контрольной суммы фрейма
    if(link->service.crc_read == link->service.crc_calc) { //контрольная сумма совпадает
        if(link->crc_correct.callback) { //адрес указателя не равен нулю
            link->crc_correct.callback(link->crc_correct.user_pointer,link); //вызов функции обработчик
        }
    } else {
        if(link->crc_incorrect.callback) { //адрес указателя не равен нулю
            link->crc_incorrect.callback(link->crc_incorrect.user_pointer,link); //вызов функции обработчик
        }
    }
}

void ecu_data_link_layer_handler(ecu_data_link_layer_t* link,uint8_t bytes_available) {
    if(link->service.count_end != link->service.count) { //данные не обработаны
        if(bytes_available >= (link->service.count_end - link->service.count)) { //есть новая пачка данных
            link->read.device.transfer(link->read.device.port,
                    &((uint8_t*)(&link->read.frame))[link->service.count],
                    (link->service.count_end - link->service.count)); //чтение новых данных
            link->service.count = link->service.count_end; //сдвиг точки записи фрейма
            if(link->service.id) { //id определен
                if(link->service.addr) {//если слейв
                    if(link->service.addr == link->read.frame.head.addr) { //адрес совпал
                        ecu_data_link_layer_parse_frame(link); //обработать фрейм
                    }
                } else { //иначе мастер
                    ecu_data_link_layer_parse_frame(link); //обработать фрейм
                }
            } else {
                link->service.id = link->read.frame.head.id; //определение id
                link->service.count_end += link->read.frame.head.count + ECU_PROTOCOL_CRC_COUNT; //определение оставшейся длины фрейма
            }
        }
    } else {
        ecu_data_link_layer_service_init(link); //инициализация приема нового фрейма
    }
}

void ecu_data_link_layer_send_frame(ecu_data_link_layer_t* link,uint8_t addr,uint8_t id,uint8_t count,void* data) {
    link->write.frame.head.addr = addr; //адрес
    link->write.frame.head.id = id; //идентификатора
    link->write.frame.head.count = count;//количество байт

    if(count) {
        memcpy(&link->write.frame.data,data,count);//копирование данных во фрейм
    }

    uint16_t crc_calc = crc16_ccitt((uint8_t*)(&link->write.frame),ECU_PROTOCOL_HEAD_COUNT + count); //вычисление контрольной суммы
    memcpy(&link->write.frame.data[link->write.frame.head.count],&crc_calc,ECU_PROTOCOL_CRC_COUNT);//копирование контрольной суммы в хвост фрейма

    link->write.device.transfer
            (link->write.device.port,(uint8_t*)(&link->write.frame),
             ECU_PROTOCOL_HEAD_COUNT + link->write.frame.head.count + ECU_PROTOCOL_CRC_COUNT); //отправка фрейма
}
