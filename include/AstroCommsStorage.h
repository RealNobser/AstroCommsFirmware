#ifndef __ASTROCOMMSSTORAGE_H__
#define __ASTROCOMMSSTORAGE_H__

#include <Arduino.h>
#include "config.h"


/*
* Storage Map in EEPROM:
* 
* Address   Content
* =========|=========
*  0x00     SW-/Config-Version (10 = 1.0, 11 = 1.1 ...)

*/


#define ADDR_ASTROCOMMSVERSION  0x00
#define ADDR_FLTHYMAPBASE       0x01

#define MAP_BLOCK_SIZE          MAX_FLTHY_CMD_SIZE + 0x04    // 6 Bytes Command + 4 bytes timeout = 10 Bytes block.

class AstroCommsStorage
{
    public:
        AstroCommsStorage();

        uint8_t getConfigVersion();
        void setConfigVersion(const uint8_t Version);

        void getSequenceMap(const uint8_t index, char* & flthy_cmd, uint32_t & flthy_rst_time);
        void setSequenceMap(const uint8_t index, const char* flthy_cmd, const uint32_t flthy_rst_time);
};

#endif