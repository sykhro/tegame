#include "nes.h"

void nes::load(NESRom &&rom) {
    m_active_rom = rom.header;

}
