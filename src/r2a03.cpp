#include "r2a03.h"

#include <type_traits>
#include <map>
#include "instructions.h"
#include "bus.h"

void r2a03::sync() {
    return m_bus->sync();
}

uint8_t r2a03::read(uint16_t addr) {
    sync();

    if (addr < 0x2000) {
        return m_ram[addr & 0x7FF];
    }

    if (m_bus) {
        return m_bus->read(addr);
    }

    return {};
}

void r2a03::write(uint16_t addr, uint8_t data) {
    sync();

    if (pending_reset) {
        return;
    }

    if (addr < 0x2000) {
        m_ram[addr & 0x7FF] = data;
    }

    if (m_bus) {
        m_bus->write(addr, data);
    }
}

void r2a03::step() {
    int opcode = read(m_regs.PC++);
    if (pending_reset) {
        opcode = 0x100;
    }

    switch (opcode) {
        case 0x100:
            BRK<brk_reason::reset>(*this);
            break;
        case 0x00:
            BRK(*this);
            break;
        case 0x01:
            inx(ORA);
            break;
        case 0x03:
            inx(ASO);
            break;
        case 0x04:
            SKB(*this);
            break;
        case 0x05:
            zp(ORA);
            break;
        case 0x06:
            zp(ASL);
            break;
        case 0x07:
            zp(ASO);
            break;
        case 0x08:
            PHP(*this);
            break;
        case 0x09:
            imm(ORA);
            break;
        case 0x0A:
            ASLacc(*this);
            break;
        case 0x0C:
            SKW(*this);
            break;
        case 0x0D:
            abs(ORA);
            break;
        case 0x0E:
            abs(ASL);
            break;
        case 0x0F:
            abs(ASO);
            break;
        case 0x10:
            BFC(7, *this);
            break;  // BPL
        case 0x11:
            iny(ORA);
            break;
        case 0x13:
            iny(ASO);
            break;
        case 0x14:
            SKB(*this);
            break;
        case 0x15:
            zp(ORA, m_regs.X);
            break;
        case 0x16:
            zp(ASL, m_regs.X);
            break;
        case 0x17:
            zp(ASO, m_regs.X);
            break;
        case 0x18:
            CLC(*this);
            break;
        case 0x19:
            abs(ORA, m_regs.Y);
            break;
        case 0x1A:
            NOP(*this);
            break;
        case 0x1B:
            abs(ASO, m_regs.Y);
            break;
        case 0x1C:
            SKW(*this);
            break;
        case 0x1D:
            abs(ORA, m_regs.X);
            break;
        case 0x1E:
            abs(ASL, m_regs.X);
            break;
        case 0x1F:
            abs(ASO, m_regs.X);
            break;
        case 0x20:
            JSR(*this);
            break;
        case 0x21:
            inx(AND);
            break;
        case 0x23:
            inx(RLA);
            break;
        case 0x24:
            zp(BIT);
            break;
        case 0x25:
            zp(AND);
            break;
        case 0x26:
            zp(ROL);
            break;
        case 0x27:
            zp(RLA);
            break;
        case 0x28:
            PLP(*this);
            break;
        case 0x29:
            imm(AND);
            break;
        case 0x2A:
            ROLacc(*this);
            break;
        case 0x2C:
            abs(BIT);
            break;
        case 0x2D:
            abs(AND);
            break;
        case 0x2E:
            abs(ROL);
            break;
        case 0x2F:
            abs(RLA);
            break;
        case 0x30:
            BFS(7, *this);
            break;  // BMI
        case 0x31:
            iny(AND);
            break;
        case 0x33:
            iny(RLA);
            break;
        case 0x34:
            SKB(*this);
            break;
        case 0x35:
            zp(AND, m_regs.X);
            break;
        case 0x36:
            zp(ROL, m_regs.X);
            break;
        case 0x37:
            zp(RLA, m_regs.X);
            break;
        case 0x38:
            SEC(*this);
            break;
        case 0x39:
            abs(AND, m_regs.Y);
            break;
        case 0x3A:
            NOP(*this);
            break;
        case 0x3B:
            abs(RLA, m_regs.Y);
            break;
        case 0x3C:
            SKW(*this);
            break;
        case 0x3D:
            abs(AND, m_regs.X);
            break;
        case 0x3E:
            abs(ROL, m_regs.X);
            break;
        case 0x3F:
            abs(RLA, m_regs.X);
            break;
        case 0x40:
            RTI(*this);
            break;
        case 0x41:
            inx(EOR);
            break;
        case 0x43:
            inx(LSE);
            break;
        case 0x44:
            SKB(*this);
            break;
        case 0x45:
            zp(EOR);
            break;
        case 0x46:
            zp(LSR);
            break;
        case 0x47:
            zp(LSE);
            break;
        case 0x48:
            PHA(*this);
            break;
        case 0x49:
            imm(EOR);
            break;
        case 0x4A:
            LSRacc(*this);
            break;
        case 0x4C:
            abs(JMP);
            break;
        case 0x4D:
            abs(EOR);
            break;
        case 0x4E:
            abs(LSR);
            break;
        case 0x4F:
            abs(LSE);
            break;
        case 0x50:
            BFC(6, *this);
            break;  // BVC
        case 0x51:
            iny(EOR);
            break;
        case 0x53:
            iny(LSE);
            break;
        case 0x54:
            SKB(*this);
            break;
        case 0x55:
            zp(EOR, m_regs.X);
            break;
        case 0x56:
            zp(LSR, m_regs.X);
            break;
        case 0x57:
            zp(LSE, m_regs.X);
            break;
        case 0x58:
            CLI(*this);
            break;
        case 0x59:
            abs(EOR, m_regs.Y);
            break;
        case 0x5A:
            NOP(*this);
            break;
        case 0x5B:
            abs(LSE, m_regs.Y);
            break;
        case 0x5C:
            SKW(*this);
            break;
        case 0x5D:
            abs(EOR, m_regs.X);
            break;
        case 0x5E:
            abs(LSR, m_regs.X);
            break;
        case 0x5F:
            abs(LSE, m_regs.X);
            break;
        case 0x60:
            RTS(*this);
            break;
        case 0x61:
            inx(ADC);
            break;
        case 0x63:
            inx(RRA);
            break;
        case 0x64:
            SKB(*this);
            break;
        case 0x65:
            zp(ADC);
            break;
        case 0x66:
            zp(ROR);
            break;
        case 0x67:
            zp(RRA);
            break;
        case 0x68:
            PLA(*this);
            break;
        case 0x69:
            imm(ADC);
            break;
        case 0x6A:
            RORacc(*this);
            break;
        case 0x6C:
            JMPind(*this);
            break;
        case 0x6D:
            abs(ADC);
            break;
        case 0x6E:
            abs(ROR);
            break;
        case 0x6F:
            abs(RRA);
            break;
        case 0x70:
            BFS(6, *this);
            break;  // BVS
        case 0x71:
            iny(ADC);
            break;
        case 0x73:
            iny(RRA);
            break;
        case 0x74:
            SKB(*this);
            break;
        case 0x75:
            zp(ADC, m_regs.X);
            break;
        case 0x76:
            zp(ROR, m_regs.X);
            break;
        case 0x77:
            zp(RRA, m_regs.X);
            break;
        case 0x78:
            SEI(*this);
            break;
        case 0x79:
            abs(ADC, m_regs.Y);
            break;
        case 0x7A:
            NOP(*this);
            break;
        case 0x7B:
            abs(RRA, m_regs.Y);
            break;
        case 0x7C:
            SKW(*this);
            break;
        case 0x7D:
            abs(ADC, m_regs.X);
            break;
        case 0x7E:
            abs(ROR, m_regs.X);
            break;
        case 0x7F:
            abs(RRA, m_regs.X);
            break;
        case 0x80:
            SKB(*this);
            break;
        case 0x81:
            inx(STA);
            break;
        case 0x83:
            inx(AXS);
            break;
        case 0x84:
            zp(STY);
            break;
        case 0x85:
            zp(STA);
            break;
        case 0x86:
            zp(STX);
            break;
        case 0x87:
            zp(AXS);
            break;
        case 0x88:
            DEY(*this);
            break;
        case 0x8A:
            TXA(*this);
            break;
        case 0x8C:
            abs(STY);
            break;
        case 0x8E:
            abs(STX);
            break;
        case 0x8D:
            abs(STA);
            break;
        case 0x8F:
            abs(AXS);
            break;
        case 0x90:
            BFC(0, *this);
            break;  // BCC
        case 0x91:
            iny(STA);
            break;
        case 0x94:
            zp(STY, m_regs.X);
            break;
        case 0x95:
            zp(STA, m_regs.X);
            break;
        case 0x96:
            zp(STX, m_regs.Y);
            break;
        case 0x97:
            zp(AXS, m_regs.Y);
            break;
        case 0x98:
            TYA(*this);
            break;
        case 0x99:
            abs(STA, m_regs.Y);
            break;
        case 0x9A:
            TXS(*this);
            break;
        case 0x9D:
            abs(STA, m_regs.X);
            break;
        case 0xA0:
            imm(LDY);
            break;
        case 0xA1:
            inx(LDA);
            break;
        case 0xA2:
            imm(LDX);
            break;
        case 0xA3:
            inx(LAX);
            break;
        case 0xA4:
            zp(LDY);
            break;
        case 0xA5:
            zp(LDA);
            break;
        case 0xA6:
            zp(LDX);
            break;
        case 0xA7:
            zp(LAX);
            break;
        case 0xA8:
            TAY(*this);
            break;
        case 0xA9:
            imm(LDA);
            break;
        case 0xAA:
            TAX(*this);
            break;
        case 0xAC:
            abs(LDY);
            break;
        case 0xAD:
            abs(LDA);
            break;
        case 0xAE:
            abs(LDX);
            break;
        case 0xAF:
            abs(LAX);
            break;
        case 0xB0:
            BFS(0, *this);
            break;  // BCS
        case 0xB1:
            iny(LDA);
            break;
        case 0xB3:
            iny(LAX);
            break;
        case 0xB4:
            zp(LDY, m_regs.X);
            break;
        case 0xB5:
            zp(LDA, m_regs.X);
            break;
        case 0xB6:
            zp(LDX, m_regs.Y);
            break;
        case 0xB7:
            zp(LAX, m_regs.Y);
            break;
        case 0xB8:
            CLV(*this);
            break;
        case 0xB9:
            abs(LDA, m_regs.Y);
            break;
        case 0xBA:
            TSX(*this);
            break;
        case 0xBC:
            abs(LDY, m_regs.X);
            break;
        case 0xBD:
            abs(LDA, m_regs.X);
            break;
        case 0xBE:
            abs(LDX, m_regs.Y);
            break;
        case 0xBF:
            abs(LAX, m_regs.Y);
            break;
        case 0xC0:
            imm(CPY);
            break;
        case 0xC1:
            inx(CMP);
            break;
        case 0xC3:
            inx(DCP);
            break;
        case 0xC4:
            zp(CPY);
            break;
        case 0xC5:
            zp(CMP);
            break;
        case 0xC6:
            zp(DEC);
            break;
        case 0xC7:
            zp(DCP);
            break;
        case 0xC8:
            INY(*this);
            break;
        case 0xC9:
            imm(CMP);
            break;
        case 0xCA:
            DEX(*this);
            break;
        case 0xCC:
            abs(CPY);
            break;
        case 0xCD:
            abs(CMP);
            break;
        case 0xCE:
            abs(DEC);
            break;
        case 0xCF:
            abs(DCP);
            break;
        case 0xD0:
            BFC(1, *this);
            break;  // BNE
        case 0xD1:
            iny(CMP);
            break;
        case 0xD3:
            iny(DCP);
            break;
        case 0xD4:
            SKB(*this);
            break;
        case 0xD5:
            zp(CMP, m_regs.X);
            break;
        case 0xD6:
            zp(DEC, m_regs.X);
            break;
        case 0xD7:
            zp(DCP, m_regs.X);
            break;
        case 0xD8:
            CLD(*this);
            break;
        case 0xD9:
            abs(CMP, m_regs.Y);
            break;
        case 0xDA:
            NOP(*this);
            break;
        case 0xDB:
            abs(DCP, m_regs.Y);
            break;
        case 0xDC:
            SKW(*this);
            break;
        case 0xDD:
            abs(CMP, m_regs.X);
            break;
        case 0xDE:
            abs(DEC, m_regs.X);
            break;
        case 0xDF:
            abs(DCP, m_regs.X);
            break;
        case 0xE0:
            imm(CPX);
            break;
        case 0xE1:
            inx(SBC);
            break;
        case 0xE3:
            inx(INS);
            break;
        case 0xE4:
            zp(CPX);
            break;
        case 0xE5:
            zp(SBC);
            break;
        case 0xE6:
            zp(INC);
            break;
        case 0xE7:
            zp(INS);
            break;
        case 0xE8:
            INX(*this);
            break;
        case 0xE9:
            imm(SBC);
            break;
        case 0xEA:
            NOP(*this);
            break;
        case 0xEB:
            imm(SBC);
            break;
        case 0xEC:
            abs(CPX);
            break;
        case 0xED:
            abs(SBC);
            break;
        case 0xEE:
            abs(INC);
            break;
        case 0xEF:
            abs(INS);
            break;
        case 0xF0:
            BFS(1, *this);
            break;
        case 0xF1:
            iny(SBC);
            break;
        case 0xF3:
            iny(INS);
            break;
        case 0xF4:
            SKB(*this);
            break;
        case 0xF5:
            zp(SBC, m_regs.X);
            break;
        case 0xF6:
            zp(INC, m_regs.X);
            break;
        case 0xF7:
            zp(INS, m_regs.X);
            break;
        case 0xF8:
            SED(*this);
            break;
        case 0xF9:
            abs(SBC, m_regs.Y);
            break;
        case 0xFA:
            NOP(*this);
            break;
        case 0xFB:
            abs(INS, m_regs.Y);
            break;
        case 0xFC:
            SKW(*this);
            break;
        case 0xFD:
            abs(SBC, m_regs.X);
            break;
        case 0xFE:
            abs(INC, m_regs.X);
            break;
        case 0xFF:
            abs(INS, m_regs.X);
            break;
    }

    pending_reset = false;
}

template <typename Instr>
void r2a03::imm(Instr ins) {
    auto address = m_regs.PC++;
    ins(address, *this);
}

template <cross_policy penalty_check, typename Instr>
void r2a03::abs(Instr ins, uint8_t offset) {
    auto lo = read(m_regs.PC++);
    auto address = [this, lo, offset]() {
        auto base = lo | read(m_regs.PC++) << 8;
        if constexpr (penalty_check == cross_policy::penalty) {
            if ((base & 0xFF00) != ((base + offset) & 0xFF00)) {
                sync();
            }
        }
        return base + offset;
    }();

    ins(address, *this);
}

template <typename Instr>
void r2a03::inx(Instr ins) {
    uint8_t naddr = (read(m_regs.PC++) + m_regs.X) % 0x100;
    uint8_t lo = read(naddr);
    uint8_t hi = read((naddr + 1) % 0x100);

    auto address = lo | hi << 8;
    ins(address, *this);
}

template <cross_policy penalty_check, typename Instr>
void r2a03::iny(Instr ins) {
    uint8_t naddr = read(m_regs.PC++) % 0x100;
    uint8_t lo = read(naddr);
    uint8_t hi = read((naddr + 1) % 0x100);

    auto address = [this, lo, hi]() {
        auto base = lo | hi << 8;
        if constexpr (penalty_check == cross_policy::penalty) {
            if ((base & 0xFF00) != ((base + m_regs.Y) & 0xFF00)) {
                sync();
            }
        }

        return base + m_regs.Y;
    }();
    ins(address, *this);
}

template <typename Instr>
void r2a03::zp(Instr ins, uint8_t index) {
    if (index) {
        sync();
    }

    auto address = (read(m_regs.PC++) + index) % 0x100;
    ins(address, *this);
}
