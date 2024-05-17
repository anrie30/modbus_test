#ifndef MODBUSCONTEXT_H
#define MODBUSCONTEXT_H

#include <modbus/modbus.h>

struct ModbusContext
{
    modbus_mapping_t* mb_mapping = nullptr;
    modbus_t *context = nullptr;
    int accept = 0;
    uint32_t timeoutSec = 2;
    uint32_t timeoutUSec = 0;
    char *host = nullptr;
    int port = 0;
//    int registers = 0;
};

#endif // MODBUSCONTEXT_H
