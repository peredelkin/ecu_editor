#include "simple_protocol.h"
#include <string.h>

void simple_protocol_service_init(simple_protocol_link_layer_t* link) {
    link->service.id = SIMPLE_PROTOCOL_LINK_ID_DEF;
    link->service.count_current = 0;
    link->service.count_remain = SIMPLE_PROTOCOL_LINK_HEAD_COUNT;
}

void simple_protocol_callback_handler(simple_protocol_callback_t* callback,void* protocol) {
    if(callback->callback) {
        callback->callback(callback->user_pointer,protocol);
    }
}

void simple_protocol_link_id_data_head_handler(simple_protocol_link_layer_t* link,void** addr_ptrs) {
    memcpy(&link->data_head_read,link->read.frame.data,SIMPLE_PROTOCOL_NET_DATA_HEAD_COUNT);
    switch (link->data_head_read.id) {
    default:
        break;
    }
}

void simple_protocol_link_id_default_handler(simple_protocol_link_layer_t* link) {

}

/**
 * @brief Запускает соответствующие обработчики ,согласно индентификатору канального уровня.
 * @param link - указатель на протокол
 * @param addr_ptrs - массив указателей на переменные для чтения и записи
 */
void simple_protocol_link_id_handler(simple_protocol_link_layer_t* link,void** addr_ptrs) {
    switch (link->read.frame.head.id) {
    case SIMPLE_PROTOCOL_LINK_ID_DATA_HEAD: simple_protocol_link_id_data_head_handler(link,addr_ptrs);
        break;
    default: simple_protocol_link_id_default_handler(link);
        break;
    }
}

void simple_protocol_link_crc_err_handler(simple_protocol_link_layer_t* link) {
    simple_protocol_callback_handler(&link->crc_err,link);
}

/**
 * @brief Читает контрольную сумму из фрейма,сравнивает с расчетной и запускает обработчик идентификатора.
 * @param link - указатель на протокол
 * @param addr_ptrs - массив указателей на переменные для чтения и записи
 */
void simple_protocol_link_crc_handler(simple_protocol_link_layer_t* link,void** addr_ptrs) {
    memcpy(&link->service.crc_read,&link->read.frame.data[link->read.frame.head.count],SIMPLE_PROTOCOL_LINK_CRC_COUNT);
    link->service.crc_calc = crc16_ccitt((uint8_t*)(&link->read.frame),SIMPLE_PROTOCOL_LINK_HEAD_COUNT + link->read.frame.head.count);
    if(link->service.crc_read == link->service.crc_calc) {
        simple_protocol_link_id_handler(link,addr_ptrs);
    } else {
        simple_protocol_link_crc_err_handler(link);
    }
}

/**
 * @brief Читает заголовок,данные фрейма,сравнивает адрес слейва и запускает обработчик контрольной суммы.
 * @param link - указатель на протокол
 * @param bytes_available - доступное для чтения количество байт
 * @param addr_ptrs - массив указателей на переменные для чтения и записи
 */
void simple_protocol_handler(simple_protocol_link_layer_t* link,int16_t bytes_available,volatile void** addr_ptrs) {
    if(link->service.count_remain) {
        if(bytes_available >= link->service.count_remain) {
            link->read.device.transfer(link->read.device.port,&((uint8_t*)(&link->read.frame))[link->service.count_current],link->service.count_remain);
            link->service.count_current += link->service.count_remain;
            if(link->service.id) {
                if(link->service.addr) {
                    if(link->service.addr == link->read.frame.head.addr) {
                        simple_protocol_link_crc_handler(link,addr_ptrs);
                    }
                } else {
                    simple_protocol_link_crc_handler(link,addr_ptrs);
                }
                simple_protocol_service_init(link);
            } else {
                link->service.id = link->read.frame.head.id;
                link->service.count_remain = link->read.frame.head.count + SIMPLE_PROTOCOL_LINK_CRC_COUNT;
            }
        }
    } else {
        simple_protocol_service_init(link);
    }
}

/**
 * @brief Заполняет заголовок канального уровня,копирует данные,вычисляет црц,копирует црц в хвост фрейма и отправляет.
 * @param link - указатель на протокол
 * @param addr - адрес слейва
 * @param id - идентификатор
 * @param count - количество байт без учета заголовка и контрольной суммы
 * @param data - указатель на передаваемые данные
 */
void simple_protocol_link_send_frame(simple_protocol_link_layer_t* link,uint8_t addr,uint8_t id,uint16_t count,void* data) {
    link->write.frame.head.addr = addr;
    link->write.frame.head.id = id;
    link->write.frame.head.count = count;
    memcpy(link->write.frame.data,data,link->write.frame.head.count);
    uint16_t crc_calc = crc16_ccitt((uint8_t*)(&link->write.frame),SIMPLE_PROTOCOL_LINK_HEAD_COUNT + link->write.frame.head.count);
    memcpy(&link->write.frame.data[link->write.frame.head.count],&crc_calc,SIMPLE_PROTOCOL_LINK_CRC_COUNT);
    link->write.device.transfer(link->write.device.port,(uint8_t*)(&link->write.frame),(SIMPLE_PROTOCOL_LINK_HEAD_COUNT + link->write.frame.head.count + SIMPLE_PROTOCOL_LINK_CRC_COUNT));
}






