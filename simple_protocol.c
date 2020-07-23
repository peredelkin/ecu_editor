#include "simple_protocol.h"
#include <string.h>

void simple_protocol_service_init(simple_protocol_link_layer_t* link) {
    link->service.id = SIMPLE_PROTOCOL_ID_DEF;
    link->service.count_current = 0;
    link->service.count_remain = SIMPLE_PROTOCOL_LINK_HEAD_COUNT;
}

void simple_protocol_callback_handler(simple_protocol_callback_t* callback,void* protocol) {
    if(callback->callback) {
        callback->callback(callback->user_pointer,protocol);
    }
}

void simple_protocol_id_handler(simple_protocol_link_layer_t* link,void** addr_ptrs) {
    simple_protocol_callback_handler(&link->id_handler,link);
}

void simple_protocol_crc_err_handler(simple_protocol_link_layer_t* link) {
    simple_protocol_callback_handler(&link->crc_err,link);
}

void simple_protocol_crc_handler(simple_protocol_link_layer_t* link,void** addr_ptrs) {
    memcpy(&link->service.crc_read,&link->read.frame.data[link->read.frame.head.count],SIMPLE_PROTOCOL_LINK_CRC_COUNT); //чтение контрольной суммы из фрейма
    link->service.crc_calc = crc16_ccitt((uint8_t*)(&link->read.frame),SIMPLE_PROTOCOL_LINK_HEAD_COUNT + link->read.frame.head.count); //расчет контрольной суммы фрейма
    if(link->service.crc_read == link->service.crc_calc) { //контрольная сумма совпадает
        simple_protocol_id_handler(link,addr_ptrs);
    } else {
        simple_protocol_crc_err_handler(link);
    }
}

void simple_protocol_handler(simple_protocol_link_layer_t* link,int16_t bytes_available,volatile void** addr_ptrs) {
    if(link->service.count_remain) { //ожидаем чтение
        if(bytes_available >= link->service.count_remain) { //доступное количество больше или равно ожидаемого
            link->read.device.transfer(link->read.device.port,&((uint8_t*)(&link->read.frame))[link->service.count_current],link->service.count_remain);
            link->service.count_current += link->service.count_remain; //перенесем точку записи
            if(link->service.id) { //идентификатор определен и все данные прочинаты
                if(link->service.addr) { //слейв
                    if(link->service.addr == link->read.frame.head.addr) { //адрес слейва совпал с принятым адресом
                        simple_protocol_crc_handler(link,addr_ptrs);
                    }
                } else { //мастер
                    simple_protocol_crc_handler(link,addr_ptrs);
                }
                simple_protocol_service_init(link); //приготовиться для приема нового пакета
            } else {
                link->service.id = link->read.frame.head.id; //определяем идентификатор
                link->service.count_remain = link->read.frame.head.count + SIMPLE_PROTOCOL_LINK_CRC_COUNT; //определяем ожидаемый остаток
            }
        }
    } else {
        simple_protocol_service_init(link); //приготовиться для приема нового пакета
    }
}
