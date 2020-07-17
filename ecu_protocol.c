#include "ecu_protocol.h"
#include <string.h>

void ecu_link_layer_init(ecu_link_layer_t* link) {
    memset(link,0,sizeof (ecu_link_layer_t));
}

void ecu_link_layer_service_init(ecu_link_layer_t* link) {
    link->service.id = ECU_SESSION_LAYER_ID_DEF;
    link->service.count = 0;
    link->service.crc_calc = 0;
    link->service.crc_read = 0;
    link->service.count_end = ECU_PROTOCOL_HEAD_COUNT;
}

void ecu_link_layer_callback_handler(ecu_protocol_callback_t* callback,void* protocol) {
    if(callback->callback) { //адрес указателя не равен нулю
        callback->callback(callback->user_pointer,protocol); //вызов функции обработчик
    }
}

void ecu_presentation_layer_read(uint16_t addr,uint16_t start,uint8_t count,void* data,volatile void** addr_ptrs) {
    memcpy(&((uint8_t*)(addr_ptrs[addr]))[start],data,count);
}

void ecu_session_layer_id_write(ecu_link_layer_t* link) { //запись фрейма

}

void ecu_session_layer_id_read(ecu_link_layer_t* link,volatile void** addr_ptrs) { //чтение фрейма
    ecu_presentation_layer_rw_t read;
    memcpy(&read,link->read.frame.data,ECU_PRESENTATION_LAYER_RW_COUNT);
    ecu_presentation_layer_read(read.addr,read.start,read.count,&link->read.frame.data[ECU_PRESENTATION_LAYER_RW_COUNT],addr_ptrs);
}

void ecu_session_layer_id_ack(ecu_link_layer_t* link) { //подтверждение

}

void ecu_session_layer_id_rst(ecu_link_layer_t* link) { //сброс

}

void ecu_session_layer_id_fin(ecu_link_layer_t* link) { //конец

}

void ecu_session_layer_id_handler(ecu_link_layer_t* link,volatile void** addr_ptrs) {
    switch(link->read.frame.head.id) {
    case ECU_SESSION_LAYER_ID_WRITE:  ecu_session_layer_id_write(link);
        break;
    case ECU_SESSION_LAYER_ID_READ:   ecu_session_layer_id_read(link,addr_ptrs);
        break;
    case ECU_SESSION_LAYER_ID_ACK:    ecu_session_layer_id_ack(link);
        break;
    case ECU_SESSION_LAYER_ID_RST:    ecu_session_layer_id_rst(link);
        break;
    case ECU_SESSION_LAYER_ID_FIN:    ecu_session_layer_id_fin(link);
        break;
    };
}

void ecu_link_layer_crc_err_handler(ecu_link_layer_t* link) {

}

void ecu_link_layer_crc_check(ecu_link_layer_t* link,volatile void** addr_ptrs) {
    memcpy(&link->service.crc_read,&link->read.frame.data[link->read.frame.head.count],ECU_PROTOCOL_CRC_COUNT); //чтение контрольной суммы из фрейма
    link->service.crc_calc = crc16_ccitt((uint8_t*)(&link->read.frame),link->service.count_end - ECU_PROTOCOL_CRC_COUNT); //расчет контрольной суммы фрейма
    if(link->service.crc_read == link->service.crc_calc) { //контрольная сумма совпадает
        ecu_session_layer_id_handler(link,addr_ptrs);
    } else {
        ecu_link_layer_crc_err_handler(link);
    }
}

void ecu_link_layer_handler(ecu_link_layer_t* link,uint8_t bytes_available,volatile void** addr_ptrs) {
    if(link->service.count_end != link->service.count) { //данные не обработаны
        if(bytes_available >= (link->service.count_end - link->service.count)) { //есть новая пачка данных
            link->read.device.transfer(link->read.device.port,
                    &((uint8_t*)(&link->read.frame))[link->service.count],
                    (link->service.count_end - link->service.count)); //чтение новых данных
            link->service.count = link->service.count_end; //сдвиг точки записи фрейма
            if(link->service.id) { //id определен
                if(link->service.addr) {//если слейв
                    if(link->service.addr == link->read.frame.head.addr) { //адрес совпал
                        ecu_link_layer_crc_check(link,addr_ptrs); //обработать фрейм
                    }
                } else { //иначе мастер
                    ecu_link_layer_crc_check(link,addr_ptrs); //обработать фрейм
                }
            } else {
                link->service.id = link->read.frame.head.id; //определение id
                link->service.count_end += link->read.frame.head.count + ECU_PROTOCOL_CRC_COUNT; //определение оставшейся длины фрейма
            }
        }
    } else {
        ecu_link_layer_service_init(link); //инициализация приема нового фрейма
    }
}

void ecu_link_layer_send_frame(ecu_link_layer_t* link,uint8_t addr,uint8_t id,uint8_t count,void* data) {
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
