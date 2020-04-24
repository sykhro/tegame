#include "bus.h"
#include "nes.h"

uint8_t bus::read(uint16_t addr) {
    return {};
}

void bus::write(uint16_t addr, uint8_t data) {}

void bus::sync() {
    m_ppu->tick();
    m_ppu->tick();
    m_ppu->tick();
    m_apu->tick();
}
