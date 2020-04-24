#include <iostream>
#include "fmt/format.h"

#include "nes.h"
#include "common/nesrom.h"

int main([[maybe_unused]] int argc, char *argv[]) {
    nes system;

    auto newrom = parse_rom(argv[1]);
    if(!newrom.has_value()) {
        return -1;
    }

    auto rom = newrom.value();
    auto &hdr = rom.header;

    fmt::print("Loaded ROM, mapper '{}'\n", (hdr.sw2.himap << 4) | hdr.sw.lomap);
    fmt::print("16k: {}, 8k: {}\n", hdr.rom_banks, hdr.vrom_banks);
    fmt::print("{} (m:{} b:{} t:{} 4:{})\n",
            hdr.sw.r, hdr.sw.mirror, hdr.sw.bat_ram, hdr.sw.trainer, hdr.sw.fscreen);
    fmt::print("{} (vs:{} h:{})\n",
            hdr.sw2.r, hdr.sw2.vs_sys, hdr.sw2.himap);
    fmt::print("8k ram: {} mode:{}\n\n", hdr.ram_banks, hdr.mode);

    system.load(std::move(rom));

    auto cpu = system.cpu();
    while(cpu->active()) {
        cpu->step();
        if(auto errcode = cpu->read(0x02); errcode != 0) {
            fmt::print("Stop! 0x02, {:#x}", errcode);
            cpu->stop();
        } else if(auto errcode = cpu->read(0x03); errcode != 0) {
            fmt::print("Stop! 0x03, {:#x}", errcode);
            cpu->stop();
        }
    }

}
