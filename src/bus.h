#pragma once

#include <cstdint>

class ppu;
class apu;

class bus {
   public:
    bus(apu *a, ppu *p) : m_apu(a), m_ppu(p){};

    void sync();

    [[nodiscard]] uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);

   private:
    apu *m_apu;
    ppu *m_ppu;
};
