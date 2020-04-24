#pragma once

#include <array>
#include <memory>
#include "common/bitfield.h"

struct cpu_regs {
    uint8_t A{0};
    uint8_t X{0};
    uint8_t Y{0};
    uint8_t S{0xFF};
    uint16_t PC{0x0}; /* This should be set by reading the reset vector */
    union {
        uint8_t r{0x34};

        bitfield<uint8_t, 0> C;
        bitfield<uint8_t, 1> Z;
        bitfield<uint8_t, 2> I;
        bitfield<uint8_t, 3> D;
        bitfield<uint8_t, 5> B;
        bitfield<uint8_t, 6> V;
        bitfield<uint8_t, 7> N;
    } P;
};

enum class cross_policy { free, penalty };
enum class brk_reason { generic, irq, nmi, reset };

class nes;
class bus;

class r2a03 final {
   public:
    r2a03(bus *b = nullptr) : m_bus(b) {}

    void init(bus *b) { m_bus = b; }
    void step();

    [[nodiscard]] bool active() const noexcept { return m_active; }
    void stop() noexcept { m_active = false; }

    [[nodiscard]] cpu_regs &regs() noexcept { return m_regs; };

    [[nodiscard]] uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);

    [[nodiscard]] uint8_t pop() { return read(0x100 | ++m_regs.S); }
    void push(uint8_t val) { write(0x100 | m_regs.S--, val); }

    bool pending_reset = true;
    bool nmi = false;

    void sync();

   private:
    template <typename Instr>
    void imm(Instr);

    template <cross_policy penalty_check = cross_policy::penalty, typename Instr>
    void abs(Instr, uint8_t ofs = 0);

    template <typename Instr>
    void inx(Instr);

    template <cross_policy penalty_check = cross_policy::penalty, typename Instr>
    void iny(Instr);

    template <typename Instr>
    void zp(Instr, uint8_t ofs = 0);

    bool m_active = true;

    bus *m_bus{};

    cpu_regs m_regs{};
    std::array<uint8_t, 0x800> m_ram{};
};
