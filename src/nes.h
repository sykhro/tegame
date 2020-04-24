#pragma once

#include <memory>

#include "common/nesrom.h"
#include "r2a03.h"
#include "bus.h"


struct ppu {
    void tick() {}
};

struct apu {
    void tick() {}
};

class nes {
   public:
    nes()
        : m_cpu(std::make_unique<r2a03>()),
          m_ppu(std::make_unique<ppu>()),
          m_apu(std::make_unique<apu>()) {
        m_bus = std::make_unique<bus>(m_apu.get(), m_ppu.get());
        m_cpu->init(m_bus.get());
    };
    ~nes() = default;

    void load(NESRom &&);
    void run();

    r2a03 *cpu() {
        return m_cpu.get();
    }

   private:
    std::unique_ptr<r2a03> m_cpu;
    std::unique_ptr<bus> m_bus;
    std::unique_ptr<ppu> m_ppu;
    std::unique_ptr<apu> m_apu;

    NESHdr m_active_rom;

    int cycles{0};
};
