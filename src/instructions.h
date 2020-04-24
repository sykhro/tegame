#pragma once

#include "r2a03.h"

static void process_zn(uint8_t data, cpu_regs &regs) {
    regs.P.Z = data == 0;
    regs.P.N = data & (1 << 7);
}

void AXS(uint16_t data_addr, r2a03 &context) {
    context.write(data_addr, context.regs().A & context.regs().X);
}

void ADC(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);
    uint16_t res = context.regs().A + data + context.regs().P.C;

    // V is set if the result is wrong (e.g. sum of positives = negative)
    context.regs().P.V = ~(context.regs().A ^ data) & (context.regs().A ^ res) & (1 << 7);
    // C is set if the result overflew
    context.regs().P.C = res > 0xFF;

    context.regs().A = res;
    process_zn(context.regs().A, context.regs());
}

void AND(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    context.regs().A &= data;
    process_zn(context.regs().A, context.regs());
}

void ASL(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    context.regs().P.C = data & (1 << 7);

    uint16_t res = data * 2;
    context.write(data_addr, static_cast<uint8_t>(res));

    context.sync();

    process_zn(res, context.regs());
}

void ASLacc(r2a03 &context) {
    context.regs().P.C = context.regs().A & (1 << 7);
    context.regs().A *= 2;

    context.sync();

    process_zn(context.regs().A, context.regs());
}

void ASO(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    context.regs().P.C = data & (1 << 7);

    uint16_t res = data * 2;
    context.write(data_addr, static_cast<uint8_t>(res));

    context.sync();

    context.regs().A |= res;
    process_zn(context.regs().A, context.regs());
}

void BFC(uint8_t flag, r2a03 &context) {
    int8_t jump = static_cast<int8_t>(context.read(context.regs().PC++));
    if (~context.regs().P.r & (1 << flag)) {
        /* One extra cycle if branch taken */
        int16_t dest = context.regs().PC + jump;
        context.sync();

        /* One extra cycle if new page */
        if ((dest & 0xFF00) != (context.regs().PC & 0xFF00)) {
            context.sync();
        }

        context.regs().PC = dest;
    }
}

void BFS(uint8_t flag, r2a03 &context) {
    int8_t jump = static_cast<int8_t>(context.read(context.regs().PC++));
    if (context.regs().P.r & (1 << flag)) {
        /* One extra cycle if branch taken */
        int16_t dest = context.regs().PC + jump;
        context.sync();

        /* One extra cycle if new page */
        if ((dest & 0xFF00) != (context.regs().PC & 0xFF00)) {
            context.sync();
        }

        context.regs().PC = dest;
    }
}

void BIT(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    context.regs().P.V = data & (1 << 6);
    context.regs().P.N = data & (1 << 7);
    context.regs().P.Z = (context.regs().A & data) == 0;
}

template <brk_reason reason = brk_reason::generic>
void BRK(r2a03 &context) {
    uint16_t cur = context.regs().PC++;

    context.sync();

    context.push(cur >> 8);
    context.push(cur);

    uint8_t flags_to_save = [context]() mutable -> uint8_t {
        if constexpr (reason == brk_reason::generic || reason == brk_reason::reset) {
            return context.regs().P.r | (1 << 4) | (1 << 5);
        } else if constexpr (reason == brk_reason::nmi) {
            return context.regs().P.r | 0x20;
        }

        return context.regs().P.r;
    }();
    context.push(flags_to_save);

    if constexpr (reason == brk_reason::nmi) {
        context.regs().PC = context.read(0xFFFA);
        context.regs().P.I = 1;
        context.regs().PC |= context.read(0xFFFB) << 8;
    } else if constexpr (reason == brk_reason::generic || reason == brk_reason::irq) {
        context.regs().PC = context.read(0xFFFE);
        context.regs().P.I = 1;
        context.regs().PC |= context.read(0xFFFF) << 8;
    } else {
        context.regs().PC = context.read(0xFFFC);
        context.regs().P.I = 1;
        context.regs().PC |= context.read(0xFFFD) << 8;
    }

    if (reason != brk_reason::reset) {
        context.regs().P.r = (flags_to_save | (1 << 2)) & 0xCF;  // (11001111)
    }
}

void CLC(r2a03 &context) {
    context.sync();
    context.regs().P.C = 0;
}

void CLI(r2a03 &context) {
    context.sync();
    context.regs().P.I = 0;
}

void CLV(r2a03 &context) {
    context.sync();
    context.regs().P.V = 0;
}

void CLD(r2a03 &context) {
    context.sync();
    context.regs().P.D = 0;
}

void CMP(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    uint8_t res = context.regs().A - data;
    context.regs().P.C = context.regs().A >= data;
    process_zn(res, context.regs());
}

void CPX(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    uint8_t res = context.regs().X - data;
    context.regs().P.C = context.regs().X >= data;
    process_zn(res, context.regs());
}

void CPY(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    uint8_t res = context.regs().Y - data;
    context.regs().P.C = context.regs().Y >= data;
    process_zn(res, context.regs());
}

void DCP(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    uint8_t res = data - 1;
    context.sync();
    context.write(data_addr, res);

    uint8_t res2 = context.regs().A - res;
    context.regs().P.C = context.regs().A >= res2;
    process_zn(res2, context.regs());
}

void DEC(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    uint8_t res = data - 1;
    context.sync();
    context.write(data_addr, res);
    process_zn(res, context.regs());
}

void DEX(r2a03 &context) {
    context.sync();
    context.regs().X--;
    process_zn(context.regs().X, context.regs());
}

void DEY(r2a03 &context) {
    context.sync();
    context.regs().Y--;
    process_zn(context.regs().Y, context.regs());
}

void EOR(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    context.regs().A ^= data;
    process_zn(context.regs().A, context.regs());
}

void LAX(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    context.regs().A = context.regs().X = data;
    process_zn(context.regs().A, context.regs());
}

void LDA(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    context.regs().A = data;
    process_zn(context.regs().A, context.regs());
}

void LDX(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    context.regs().X = data;
    process_zn(context.regs().X, context.regs());
}

void LDY(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    context.regs().Y = data;
    process_zn(context.regs().Y, context.regs());
}

void LSE(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    context.regs().P.C = data & 1;
    uint16_t res = data >> 1;
    context.write(data_addr, static_cast<uint8_t>(res));
    context.sync();

    context.regs().A ^= res;
    process_zn(context.regs().A, context.regs());
}

void LSR(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    context.regs().P.C = data & 1;

    uint16_t res = data >> 1;
    context.write(data_addr, static_cast<uint8_t>(res));
    context.sync();

    process_zn(res, context.regs());
}

void LSRacc(r2a03 &context) {
    context.regs().P.C = context.regs().A & 1;
    context.regs().A >>= 1;

    context.sync();

    process_zn(context.regs().A, context.regs());
}

void INC(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    uint16_t res = data + 1;
    context.sync();
    context.write(data_addr, res);
    process_zn(res, context.regs());
}

void INS(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    uint8_t res = data + 1;
    context.sync();
    context.write(data_addr, res);

    /* Now SBC the result */
    res = ~res;
    uint16_t res2 = context.regs().A + res + context.regs().P.C;

    context.regs().P.V = ~(context.regs().A ^ res) & (context.regs().A ^ res) & (1 << 7);
    context.regs().P.C = res2 > 0xFF;

    context.regs().A = res2;
    process_zn(context.regs().A, context.regs());
}

void INX(r2a03 &context) {
    context.sync();
    context.regs().X++;
    process_zn(context.regs().X, context.regs());
}

void INY(r2a03 &context) {
    context.sync();
    context.regs().Y++;
    process_zn(context.regs().Y, context.regs());
}

void JMP(uint16_t data_addr, r2a03 &context) {
    context.regs().PC = data_addr;
}

void JMPind(r2a03 &context) {
    uint16_t addr = context.read(context.regs().PC++);
    addr |= context.read(context.regs().PC++) << 8;

    uint8_t vec_lo = context.read(addr);
    /* CPU bug causes indirect vector falling on page boundary to be
       fetched from the beginning of the same page */
    uint16_t vec_hi = (addr & 0xFF00) + ((addr + 1) % 0x100);

    uint16_t target = context.read(vec_hi) << 8 | vec_lo;
    context.regs().PC = target;
}

void JSR(r2a03 &context) {
    uint8_t lo = context.read(context.regs().PC++);
    uint8_t hi = context.read(context.regs().PC++);

    context.sync();

    uint16_t tmp = context.regs().PC - 1;
    context.push(tmp >> 8);
    context.push(static_cast<uint8_t>(tmp));

    context.regs().PC = hi << 8 | lo;
}

/* todo: inline in dispatch table */
void NOP(r2a03 &context) {
    context.sync();
}

void ORA(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    context.regs().A |= data;
    process_zn(data, context.regs());
}

void PHA(r2a03 &context) {
    uint8_t acc = context.regs().A;
    context.sync();
    context.push(acc);
}

void PHP(r2a03 &context) {
    /* Quirk: PHP pushes the status with the breakpoint bit set */
    uint8_t tmp = context.regs().P.r | 0x30;
    context.sync();
    context.push(tmp);
}

void PLA(r2a03 &context) {
    uint8_t val = context.pop();
    context.sync();

    context.regs().A = val;
    process_zn(context.regs().A, context.regs());

    context.sync();
}

void PLP(r2a03 &context) {
    context.sync();
    context.sync();
    /* Account for PHP quirk */
    context.regs().P.r = (context.pop() | 0x30) - 0x10;
}

void RLA(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    uint8_t res = data << 1 | context.regs().P.C;
    context.regs().P.C = data & (1 << 7);
    context.write(data_addr, res);

    context.sync();

    context.regs().A &= res;
    process_zn(context.regs().A, context.regs());
}

void ROL(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    uint8_t res = data << 1 | context.regs().P.C;
    context.regs().P.C = data & (1 << 7);
    context.write(data_addr, res);
    context.sync();

    process_zn(res, context.regs());
}

void ROLacc(r2a03 &context) {
    context.sync();

    uint8_t newcarry = context.regs().A & (1 << 7);
    context.regs().A = context.regs().A << 1 | context.regs().P.C;
    context.regs().P.C = newcarry;

    process_zn(context.regs().A, context.regs());
}

void ROR(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    uint8_t res = data >> 1 | (context.regs().P.C << 7);
    context.regs().P.C = data & 1;
    context.write(data_addr, res);
    context.sync();

    process_zn(res, context.regs());
}

void RORacc(r2a03 &context) {
    context.sync();

    uint8_t newcarry = context.regs().A & 1;
    context.regs().A = context.regs().A >> 1 | context.regs().P.C << 7;
    context.regs().P.C = newcarry;

    process_zn(context.regs().A, context.regs());
}

void RTI(r2a03 &context) {
    context.sync();
    uint8_t flags = (context.pop() | 0x30) - 0x10;
    context.sync();

    context.regs().PC = context.pop();
    context.regs().PC |= context.pop() << 8;

    context.regs().P.r = flags;
}

void RTS(r2a03 &context) {
    context.sync();
    context.sync();

    uint16_t oldaddr = context.pop() | context.pop() << 8;
    context.regs().PC = oldaddr + 1;

    context.sync();
}

void RRA(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);
    uint8_t res = data >> 1 | (context.regs().P.C << 7);
    context.regs().P.C = data & 1;
    context.write(data_addr, res);

    context.sync();

    uint16_t res2 = context.regs().A + res + context.regs().P.C;

    context.regs().P.V = ~(context.regs().A ^ res) & (context.regs().A ^ res2) & (1 << 7);
    context.regs().P.C = res2 > 0xFF;

    context.regs().A = res2;
    process_zn(context.regs().A, context.regs());
}

void SBC(uint16_t data_addr, r2a03 &context) {
    uint8_t data = context.read(data_addr);

    data = ~data;
    /* The rest is just the same as ADC */
    uint16_t res = context.regs().A + data + context.regs().P.C;

    context.regs().P.V = ~(context.regs().A ^ data) & (context.regs().A ^ res) & (1 << 7);
    context.regs().P.C = res > 0xFF;

    context.regs().A = res;
    process_zn(context.regs().A, context.regs());
}

void SED(r2a03 &context) {
    context.sync();
    context.regs().P.D = 1;
}

void SEC(r2a03 &context) {
    context.sync();
    context.regs().P.C = 1;
}

void SEI(r2a03 &context) {
    context.sync();
    context.regs().P.I = 1;
}

void SKB(r2a03 &context) {
    (void)context.read(context.regs().PC++);
    context.sync();
}

void SKW(r2a03 &context) {
    (void)context.read(context.regs().PC++);
    (void)context.read(context.regs().PC++);
    context.sync();
}

void STA(uint16_t data_addr, r2a03 &context) {
    context.write(data_addr, context.regs().A);
}

void STY(uint16_t data_addr, r2a03 &context) {
    context.write(data_addr, context.regs().Y);
}

void STX(uint16_t data_addr, r2a03 &context) {
    context.write(data_addr, context.regs().X);
}

void TAX(r2a03 &context) {
    context.sync();
    context.regs().X = context.regs().A;
    process_zn(context.regs().X, context.regs());
}

void TAY(r2a03 &context) {
    context.sync();
    context.regs().Y = context.regs().A;
    process_zn(context.regs().Y, context.regs());
}

void TXA(r2a03 &context) {
    context.sync();
    context.regs().A = context.regs().X;
    process_zn(context.regs().A, context.regs());
}

void TYA(r2a03 &context) {
    context.sync();
    context.regs().A = context.regs().Y;
    process_zn(context.regs().A, context.regs());
}

void TXS(r2a03 &context) {
    context.sync();
    context.regs().S = context.regs().X;
}

void TSX(r2a03 &context) {
    context.sync();
    context.regs().X = context.regs().S;
    process_zn(context.regs().X, context.regs());
}
