#include "AstroCommsStorage.h"
#include <EEPROM.h>

AstroCommsStorage::AstroCommsStorage()
{

}

byte AstroCommsStorage::getConfigVersion()
{
    return EEPROM.read(ADDR_ASTROCOMMSVERSION);
}

void AstroCommsStorage::setConfigVersion(const uint8_t version)
{
    EEPROM.update(ADDR_ASTROCOMMSVERSION, version);
}


void AstroCommsStorage::getSequenceMap(const uint8_t index, char* & flthy_cmd, uint32_t & flthy_rst_time)
{
    memset(flthy_cmd, 0x00, MAX_FLTHY_CMD_SIZE+1);

    EEPROM.get(ADDR_FLTHYMAPBASE+(index * MAP_BLOCK_SIZE), flthy_rst_time);

    for(uint8_t i = 0; i < MAX_FLTHY_CMD_SIZE; i++)
    {
        flthy_cmd[i] = EEPROM.read(ADDR_FLTHYMAPBASE + (index * MAP_BLOCK_SIZE) + sizeof(flthy_rst_time) + i);
    }
}

void AstroCommsStorage::setSequenceMap(const uint8_t index, const char* flthy_cmd, const uint32_t flthy_rst_time)
{
    if (strlen(flthy_cmd) > MAX_FLTHY_CMD_SIZE)
        return;

    EEPROM.put(ADDR_FLTHYMAPBASE+(index * MAP_BLOCK_SIZE), flthy_rst_time);

    for(uint8_t i = 0; i < strlen(flthy_cmd); i++)
    {
        EEPROM.update(ADDR_FLTHYMAPBASE + (index * MAP_BLOCK_SIZE) + sizeof(flthy_rst_time) + i, flthy_cmd[i]);
    }
}
