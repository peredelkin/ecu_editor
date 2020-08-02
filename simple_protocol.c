#include "simple_protocol.h"
#include <string.h>

void simple_protocol_service_init
(simple_protocol_link_layer_t* link) {
    link->service.id = SIMPLE_PROTOCOL_LINK_ID_DEF;
    link->service.count_current = 0;
    link->service.count_remain = SIMPLE_PROTOCOL_LINK_HEAD_COUNT;
    link->service.crc_calc = 0;
    link->service.crc_read = 0;
}

/**
 * @brief Обработчик колбеков
 * @param callback - указатель на колбек
 * @param protocol - указатель на протокол
 */
void simple_protocol_callback_handler
(simple_protocol_callback_t* callback,void* protocol) {
    if(callback->callback) {
        callback->callback(callback->user_pointer,protocol);
    }
}

/**
 * @brief Читает данные
 * @param data - данные для записи
 * @param addr - номр указателя из массива указателей
 * @param start - сдвиг
 * @param count - количество
 * @param addr_ptrs - указатель на массив указателей на переменные
 */
void simple_protocol_net_data_read
(uint8_t* data,uint16_t addr,uint16_t start,uint16_t count,volatile void** addr_ptrs) {
    memcpy(data,&((uint8_t*)(addr_ptrs[addr]))[start],count);
}

/**
 * @brief Записывает данные
 * @param data - данные для чтения
 * @param addr - номер указателя из массива указателей
 * @param start - сдвиг
 * @param count - количество
 * @param addr_ptrs - указатель на массив указателей на переменные
 */
void simple_protocol_net_data_write
(uint8_t* data,uint16_t addr,uint16_t start,uint16_t count,volatile void** addr_ptrs) {
    memcpy(&((uint8_t*)(addr_ptrs[addr]))[start],data,count);
}

/**
 * @brief Отправляет команду
 * @param link - протокол
 * @param dev_addr - адрес устройства
 * @param id - идентификатор
 * @param addr - адрес из массива указателей
 * @param start - смещение
 * @param count - количество
 */
void simple_protocol_net_cmd_send
(simple_protocol_link_layer_t* link,uint8_t dev_addr,uint8_t id,uint16_t addr,uint16_t start,uint16_t count) {
    link->write.data_head.id = id;
    link->write.data_head.addr = addr;
    link->write.data_head.start = start;
    link->write.data_head.count = count;
    memcpy(link->write.frame.data,&link->write.data_head,SIMPLE_PROTOCOL_NET_DATA_HEAD_COUNT);
    simple_protocol_link_send_frame(link,dev_addr,SIMPLE_PROTOCOL_LINK_ID_DATA_HEAD,SIMPLE_PROTOCOL_NET_DATA_HEAD_COUNT);
}

/**
 * @brief Отправляет запрос на чтение
 * @param link - указатель протокола
 * @param dev_addr - адрес устройства
 * @param addr - адрес из массива указателей
 * @param start - смещение
 * @param count - количество
 */
void simple_protocol_data_read
(simple_protocol_link_layer_t* link,uint8_t dev_addr,uint16_t addr,uint16_t start,uint16_t count) {
    simple_protocol_net_cmd_send(link,dev_addr,SIMPLE_PROTOCOL_NET_DATA_READ,addr,start,count);
}

/**
 * @brief Записывает данные
 * @param link - указатель протокола
 * @param dev_addr - адрес устройства
 * @param addr - адрес из массива указателей
 * @param start - смещение
 * @param count - количество
 */
void simple_protocol_data_write
(simple_protocol_link_layer_t* link,uint8_t dev_addr,uint16_t addr,uint16_t start,uint16_t count) {
    link->write.data_head.id = SIMPLE_PROTOCOL_NET_DATA_WRITE;
    link->write.data_head.addr = addr;
    link->write.data_head.start = start;
    link->write.data_head.count = count;
    memcpy(link->write.frame.data,&link->write.data_head,SIMPLE_PROTOCOL_NET_DATA_HEAD_COUNT);
    simple_protocol_net_data_read(&link->write.frame.data[SIMPLE_PROTOCOL_NET_DATA_HEAD_COUNT],addr,start,count,link->addr_ptrs);
    simple_protocol_link_send_frame(link,dev_addr,SIMPLE_PROTOCOL_LINK_ID_DATA_HEAD,SIMPLE_PROTOCOL_NET_DATA_HEAD_COUNT + count);
}

void simple_protocol_net_data_read_handler
(simple_protocol_link_layer_t* link,uint16_t addr,uint16_t start,uint16_t count) {
    simple_protocol_data_write (link,link->read.frame.head.addr,addr,start,count);
}

void simple_protocol_net_data_write_handler
(simple_protocol_link_layer_t* link,uint16_t addr,uint16_t start,uint16_t count) {
    simple_protocol_net_data_write(&link->read.frame.data[SIMPLE_PROTOCOL_NET_DATA_HEAD_COUNT],addr,start,count,link->addr_ptrs);
    simple_protocol_net_cmd_send(link,link->read.frame.head.addr,SIMPLE_PROTOCOL_NET_DATA_WRITTEN,addr,start,count);
    simple_protocol_callback_handler(&link->data_read,link);
}

void simple_protocol_net_data_written_handler
(simple_protocol_link_layer_t* link) {
    simple_protocol_callback_handler(&link->data_write,link);
}

/**
 * @brief Обработчик идентификаторов сетевого уровня
 * @param link - протокол
 */
void simple_protocol_link_id_data_head_handler
(simple_protocol_link_layer_t* link) {
    memcpy(&link->read.data_head,link->read.frame.data,SIMPLE_PROTOCOL_NET_DATA_HEAD_COUNT);
    switch (link->read.data_head.id) {
    case SIMPLE_PROTOCOL_NET_DATA_READ: simple_protocol_net_data_read_handler
                (link,link->read.data_head.addr,link->read.data_head.start,link->read.data_head.count);
        break;
    case SIMPLE_PROTOCOL_NET_DATA_WRITE: simple_protocol_net_data_write_handler
                (link,link->read.data_head.addr,link->read.data_head.start,link->read.data_head.count);
        break;
    case SIMPLE_PROTOCOL_NET_DATA_WRITTEN: simple_protocol_net_data_written_handler(link);
        break;
    default:
        break;
    }
}

/**
 * @brief Запускает соответствующие обработчики ,согласно индентификатору канального уровня.
 * @param link - указатель на протокол
 */
void simple_protocol_link_id_handler
(simple_protocol_link_layer_t* link) {
    switch (link->read.frame.head.id) {
    case SIMPLE_PROTOCOL_LINK_ID_DATA_HEAD: simple_protocol_link_id_data_head_handler(link);
        break;
    default:
        break;
    }
}

/**
 * @brief Обработчик ошибки контрольной суммы
 * @param link - указатель на протокол
 */
void simple_protocol_link_crc_err_handler
(simple_protocol_link_layer_t* link) {
    simple_protocol_callback_handler(&link->crc_err,link);
}

/**
 * @brief Расчитывает crc для приема и передачи
 * @param transfer - указатель приема или передачи
 * @return контрольная сумма
 */
uint16_t simple_protocol_link_frame_crc_calc(simple_protocol_transfer_t* transfer) {
    return crc16_ccitt((uint8_t*)(&transfer->frame),SIMPLE_PROTOCOL_LINK_HEAD_COUNT + transfer->frame.head.count);
}

/**
 * @brief Читает контрольную сумму из фрейма,сравнивает с расчетной и запускает обработчик идентификатора.
 * @param link - указатель на протокол
 */
void simple_protocol_link_crc_handler
(simple_protocol_link_layer_t* link) {
    memcpy(&link->service.crc_read,&link->read.frame.data[link->read.frame.head.count],SIMPLE_PROTOCOL_LINK_CRC_COUNT);
    link->service.crc_calc = simple_protocol_link_frame_crc_calc(&link->read);
    if(link->service.crc_read == link->service.crc_calc) {
        simple_protocol_link_id_handler(link);
    } else {
        simple_protocol_link_crc_err_handler(link);
    }
}

/**
 * @brief Принимает или отправляет данные
 * @param transfer - указатель приема или передачи
 * @param data - указатель данных приема или передачи
 * @param count - количество байт
 */
void simple_protocol_link_transfer(simple_protocol_transfer_t* transfer,uint8_t* data,uint16_t count) {
    transfer->device.transfer(transfer->device.port,data,count);
}

/**
 * @brief Читает заголовок,данные фрейма,сравнивает адрес слейва и запускает обработчик контрольной суммы.
 * @param link - указатель на протокол
 * @param bytes_available - доступное для чтения количество байт
 */
void simple_protocol_handler
(simple_protocol_link_layer_t* link,uint16_t bytes_available) {
    if(link->service.count_remain) {
        if(bytes_available >= link->service.count_remain) {
            simple_protocol_link_transfer(&link->read,&((uint8_t*)(&link->read.frame))[link->service.count_current],
                    link->service.count_remain);
            link->service.count_current += link->service.count_remain;
            if(link->service.id) {
                simple_protocol_service_init(link);
                if(link->service.addr) {
                    if(link->service.addr == link->read.frame.head.addr) {
                        simple_protocol_link_crc_handler(link);
                    }
                } else {
                    simple_protocol_link_crc_handler(link);
                }
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
 * @brief Заполняет заголовок канального уровня,вычисляет црц,копирует црц в хвост фрейма и отправляет.
 * @param link - указатель на протокол
 * @param addr - адрес слейва
 * @param id - идентификатор
 * @param count - количество байт без учета заголовка и контрольной суммы
 */
void simple_protocol_link_send_frame
(simple_protocol_link_layer_t* link,uint8_t addr,uint8_t id,uint16_t count) {
    link->write.frame.head.addr = addr;
    link->write.frame.head.id = id;
    link->write.frame.head.count = count;
    uint16_t crc_calc = simple_protocol_link_frame_crc_calc(&link->write);
    memcpy(&link->write.frame.data[link->write.frame.head.count],&crc_calc,SIMPLE_PROTOCOL_LINK_CRC_COUNT);
    simple_protocol_link_transfer
            (&link->write,(uint8_t*)(&link->write.frame),
             (SIMPLE_PROTOCOL_LINK_HEAD_COUNT + link->write.frame.head.count + SIMPLE_PROTOCOL_LINK_CRC_COUNT));
}






