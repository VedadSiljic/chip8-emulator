#include "chip-8.h"

#include <algorithm>
#include <iostream>

Chip8 context;

void loadFontsIntoMemory() {
    uint8_t fonts[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,
        0x20, 0x60, 0x20, 0x20, 0x70,
        0xF0, 0x10, 0xF0, 0x80, 0xF0,
        0xF0, 0x10, 0xF0, 0x10, 0xF0,
        0x90, 0x90, 0xF0, 0x10, 0x10,
        0xF0, 0x80, 0xF0, 0x10, 0xF0,
        0xF0, 0x80, 0xF0, 0x90, 0xF0,
        0xF0, 0x10, 0x20, 0x40, 0x40,
        0xF0, 0x90, 0xF0, 0x90, 0xF0,
        0xF0, 0x90, 0xF0, 0x10, 0xF0,
        0xF0, 0x90, 0xF0, 0x90, 0x90,
        0xE0, 0x90, 0xE0, 0x90, 0xE0,
        0xF0, 0x80, 0x80, 0x80, 0xF0,
        0xE0, 0x90, 0x90, 0x90, 0xE0,
        0xF0, 0x80, 0xF0, 0x80, 0xF0,
        0xF0, 0x80, 0xF0, 0x80, 0x80
    };

    std::copy_n(fonts, sizeof(fonts), context.memory + 0x50);
}

void fetchDecodeExecuteInstruction() {
    const uint16_t opcode = context.memory[context.pc] << 8 | context.memory[context.pc + 1];
    context.pc += 2;

    switch (opcode >> 12) {
        case 0x0:
            if (opcode == 0x00E0) std::fill_n(context.display, 64 * 32, false);
            break;
        case 0x1:
            context.pc = opcode & 0x0FFF;
            break;
        case 0x6:
            context.V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            break;
        case 0x7:
            context.V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            break;
        case 0xA:
            context.I = opcode & 0x0FFF;
            break;
        case 0xD: {
            const uint8_t x = context.V[(opcode & 0x0F00) >> 8];
            const uint8_t y = context.V[(opcode & 0x00F0) >> 4];
            const uint8_t n = opcode & 0x000F;
            context.V[0xF] = 0x0;

            for (uint8_t i = 0; i < n; i++) {
                if (y + i >= 32) break;
                const uint8_t spriteRow = context.memory[context.I + i];
                for (uint8_t j = 0; j < 8; j++) {
                    if (x + j >= 64) break;
                    if (!(context.display[64 * (y + i) + x + j] ^= (spriteRow >> (7 - j)) & 0x1)) context.V[0xF] = 0x1; // if result of xor is 0 set F flag to 1
                }
            }

            break;
        }
        default:
            std::cout << "Opcode not implemented" << std::endl;
    }
}