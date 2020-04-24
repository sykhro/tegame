#pragma once
#include "bitfield.h"

#include <cstdint>
#include <fstream>
#include <algorithm>
#include <optional>
#include <vector>

constexpr char magic[] = {0x4E, 0x45, 0x53, 0x1A};

struct NESHdr {
    uint8_t rom_banks;   // 16KB banks
    uint8_t vrom_banks;  // 8KB banks
    union {
        uint8_t r;
        bitfield<uint8_t, 0> mirror;   // 0 = V-Mirror, 1 = H-Mirror
        bitfield<uint8_t, 1> bat_ram;  // Battery
        bitfield<uint8_t, 2> fscreen;  // 4 screen layout
        bitfield<uint8_t, 3> trainer;  // Trainer
        bitfield<uint8_t, 4, 4> lomap;
    } sw;

    union {
        uint8_t r;
        bitfield<uint8_t, 0> vs_sys;
        bitfield<uint8_t, 4, 4> himap;  // Higher nibble of mapper number
    } sw2;

    uint8_t ram_banks;  // 8KB RAM banks
    uint8_t mode;       // PAL == 1
};

struct NESRom {
    struct NESHdr header;
    std::vector<char> rom, vrom;
};

std::optional<NESRom> parse_rom(const char *path) {
    std::fstream file(path);

    /* Check the first 4 bytes to see if the magic is correct */
    char newmagic[4];
    file.read(newmagic, 4);
    if (!std::equal(std::begin(newmagic), std::end(newmagic), magic)) {
        return std::nullopt;
    }

    NESRom newrom;
    auto &newhead = newrom.header;

    file.read(reinterpret_cast<char *>(&(newhead.rom_banks)), 1);
    file.read(reinterpret_cast<char *>(&(newhead.vrom_banks)), 1);
    file.read(reinterpret_cast<char *>(&(newhead.sw.r)), 1);
    file.read(reinterpret_cast<char *>(&(newhead.sw2.r)), 1);
    file.read(reinterpret_cast<char *>(&(newhead.ram_banks)), 1);
    file.read(reinterpret_cast<char *>(&(newhead.mode)), 1);

    /* Skip zeroes */
    file.ignore(6);

    newrom.rom.resize(newhead.rom_banks * 0x4000);
    file.read(reinterpret_cast<char *>(newrom.rom.data()), newhead.rom_banks * 0x4000);

    newrom.vrom.resize(newhead.vrom_banks * 0x2000);
    file.read(reinterpret_cast<char *>(newrom.vrom.data()), newhead.vrom_banks * 0x2000);

    return newrom;
}
