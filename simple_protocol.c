#include "simple_protocol.h"
#include <string.h>

void simple_protocol_init(simple_protocol_link_layer_t* link) {
    memset(link,0,sizeof (simple_protocol_link_layer_t));
}

void simple_protocol_service_init(simple_protocol_link_layer_t* link) {
    link->service.id = SIMPLE_PROTOCOL_ID_DEF;
    link->service.count = 0;
    link->service.count_end = SIMPLE_PROTOCOL_LINK_HEAD_COUNT;
}

void simple_protocol_callback_handler(simple_protocol_callback_t* callback,void* protocol) {
    if(callback->callback) { //адрес указателя не равен нулю
        callback->callback(callback->user_pointer,protocol); //вызов функции обработчик
    }
}

void simple_protocol_id_cmd_ack(simple_protocol_link_layer_t* link) {
    simple_protocol_callback_handler(&link->ack_received,link);
}

void simple_protocol_crc_err_handler(simple_protocol_link_layer_t* link) {
    simple_protocol_callback_handler(&link->crc_err,link);
}

void simple_protocol_wrong_id(simple_protocol_link_layer_t* link) {
    simple_protocol_callback_handler(&link->wrong_id,link);
}

void simple_protocol_data_frame_read(uint16_t addr,uint16_t start,uint8_t count,void* data,void** addr_ptrs) {
    memcpy(&((uint8_t*)(addr_ptrs[addr]))[start],data,count); //копирование из фрейма
}

void simple_protocol_data_frame_write(uint16_t addr,uint16_t start,uint8_t count,void* data,void** addr_ptrs) {
    memcpy(data,&((uint8_t*)(addr_ptrs[addr]))[start],count); //копирование во фрейм
}

void simple_protocol_crc_transfer_frame(simple_protocol_link_layer_t* link) {
    uint16_t crc_calc = crc16_ccitt((uint8_t*)(&link->write.frame),SIMPLE_PROTOCOL_LINK_HEAD_COUNT + link->write.frame.head.count); //вычисление контрольной суммы
    memcpy(&link->write.frame.data[link->write.frame.head.count],&crc_calc,SIMPLE_PROTOCOL_LINK_CRC_COUNT);//копирование контрольной суммы в хвост фрейма

    link->write.device.transfer
            (link->write.device.port,(uint8_t*)(&link->write.frame),
             SIMPLE_PROTOCOL_LINK_HEAD_COUNT + link->write.frame.head.count + SIMPLE_PROTOCOL_LINK_CRC_COUNT); //отправка фрейма
}

void simple_protocol_send_frame(simple_protocol_link_layer_t* link,uint8_t addr,uint8_t id,uint8_t count,void* data) {
    link->write.frame.head.addr = addr; //адрес
    link->write.frame.head.id = id; //идентификатор
    link->write.frame.head.count = count;//количество байт
    memcpy(&link->write.frame.data,data,count);//копирование данных во фрейм
    simple_protocol_crc_transfer_frame(link); //расчет,копирование контрольной суммны в хвост и отправка фрейма
}

void simple_protocol_cmd_read(simple_protocol_link_layer_t* link,uint8_t device_addr,uint16_t addr,uint16_t start,uint8_t count) {
    simple_protocol_id_rw_t read;
    read.addr = addr;
    read.start = start;
    read.count = count;
    simple_protocol_send_frame(link,device_addr,SIMPLE_PROTOCOL_ID_CMD_WRITE,SIMPLE_PROTOCOL_ID_RW_HEAD_COUNT,&read);
}

void simple_protocol_cmd_write(simple_protocol_link_layer_t* link,uint8_t device_addr,uint16_t addr,uint16_t start,uint8_t count) {
    simple_protocol_id_rw_t write;
    write.addr = addr;
    write.start = start;
    write.count = count;
    simple_protocol_send_frame(link,device_addr,SIMPLE_PROTOCOL_ID_CMD_READ,SIMPLE_PROTOCOL_ID_RW_HEAD_COUNT,&write);
}

void simple_protocol_id_cmd_write(simple_protocol_link_layer_t* link,void** addr_ptrs) { //запись и отправка фрейма (!) не тестировалось
    memcpy(link->write.frame.data,link->read.frame.data,SIMPLE_PROTOCOL_ID_RW_HEAD_COUNT); //копирование addr,start,count

    simple_protocol_id_rw_t write; //заголовок данных
    memcpy(&write,link->write.frame.data,SIMPLE_PROTOCOL_ID_RW_HEAD_COUNT); //получение addr,start,count

    link->write.frame.head.addr = link->read.frame.head.addr; //адрес устройства
    link->write.frame.head.id = SIMPLE_PROTOCOL_ID_READ; //идентификатор
    link->write.frame.head.count = SIMPLE_PROTOCOL_ID_RW_HEAD_COUNT + write.count;//количество байт без заголовка и контрольной суммы


    simple_protocol_data_frame_write(write.addr,write.start,write.count,&link->write.frame.data[SIMPLE_PROTOCOL_ID_RW_HEAD_COUNT],addr_ptrs);//копирование полезной информации

    simple_protocol_crc_transfer_frame(link); //расчет,копирование контрольной суммны в хвост и отправка фрейма
    simple_protocol_callback_handler(&link->data_transmitted,link); //калбек "данные отправлены"
}

void simple_protocol_id_read(simple_protocol_link_layer_t* link,void** addr_ptrs) { //обработчик чтения фрейма
    simple_protocol_id_rw_t read;
    memcpy(&read,link->read.frame.data,SIMPLE_PROTOCOL_ID_RW_HEAD_COUNT); //определение адреса,начального адреса и количества
    simple_protocol_data_frame_read(read.addr,read.start,read.count,&link->read.frame.data[SIMPLE_PROTOCOL_ID_RW_HEAD_COUNT],addr_ptrs); //чтение данных
    simple_protocol_send_frame(link,link->read.frame.head.addr,SIMPLE_PROTOCOL_ID_CMD_ACK,SIMPLE_PROTOCOL_ID_RW_HEAD_COUNT,&read); //send ack
    simple_protocol_callback_handler(&link->data_received,link); //калбек "данные получены"
}

void simple_protocol_id_cmd_read(simple_protocol_link_layer_t* link) { //обработчик команды запроса чтения
    simple_protocol_id_rw_t read;
    memcpy(&read,link->read.frame.data,SIMPLE_PROTOCOL_ID_RW_HEAD_COUNT);
    simple_protocol_send_frame(link,link->read.frame.head.addr,SIMPLE_PROTOCOL_ID_CMD_WRITE,SIMPLE_PROTOCOL_ID_RW_HEAD_COUNT,&read);
}

void simple_protocol_id_handler(simple_protocol_link_layer_t* link,void** addr_ptrs) {
    switch(link->read.frame.head.id) {
    case SIMPLE_PROTOCOL_ID_CMD_WRITE:  simple_protocol_id_cmd_write(link,addr_ptrs); //запись данных во фрейм
        break;
    case SIMPLE_PROTOCOL_ID_READ:       simple_protocol_id_read(link,addr_ptrs); //чтение данных из фрейма
        break;
    case SIMPLE_PROTOCOL_ID_CMD_READ:   simple_protocol_id_cmd_read(link); //запрос записи даных во фрейм
        break;
    case SIMPLE_PROTOCOL_ID_CMD_ACK:    simple_protocol_id_cmd_ack(link); //положительный ответ на запись
        break;
    default:
        break;
    }
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
    if(link->service.count_end != link->service.count) { //данные не обработаны
        if(bytes_available >= (link->service.count_end - link->service.count)) { //есть новая пачка данных
            link->read.device.transfer(link->read.device.port,
                    &((uint8_t*)(&link->read.frame))[link->service.count],
                    (link->service.count_end - link->service.count)); //чтение новых данных
            link->service.count = link->service.count_end; //сдвиг точки записи фрейма
            if(link->service.id) { //id определен
                if(link->service.addr) {//если слейв
                    if(link->service.addr == link->read.frame.head.addr) { //адрес совпал
                        simple_protocol_crc_handler(link,addr_ptrs); //обработать фрейм
                    }
                } else { //иначе мастер
                    simple_protocol_crc_handler(link,addr_ptrs); //обработать фрейм
                }
            } else {
                link->service.id = link->read.frame.head.id; //определение id
                link->service.count_end += link->read.frame.head.count + SIMPLE_PROTOCOL_LINK_CRC_COUNT; //определение оставшейся длины фрейма
            }
        }
    } else {
        simple_protocol_service_init(link); //инициализация приема нового фрейма
    }
}
