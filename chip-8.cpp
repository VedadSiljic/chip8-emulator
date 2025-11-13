#include "chip-8.h"

#include <algorithm>
#include <iostream>
#include <random>
#include <fstream>

Chip8 context;

bool *getDisplay() {
    return context.display;
}

void setInput(const uint8_t value) {
    context.keyPressed = value;
}

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
            else if (opcode == 0x00EE) context.pc = context.stack[context.sp--];
            break;
        case 0x1:
            context.pc = opcode & 0x0FFF;
            break;
        case 0x2:
            context.stack[++context.sp] = context.pc;
            context.pc = opcode & 0x0FFF;
            break;
        case 0x3: {
            const uint8_t nn = opcode & 0x00FF;
            if (context.V[(opcode & 0x0F00) >> 8] == nn) context.pc += 2;
            break;
        }
        case 0x4: {
            const uint8_t nn = opcode & 0x00FF;
            if (context.V[(opcode & 0x0F00) >> 8] != nn) context.pc += 2;
            break;
        }
        case 0x5:
            if (context.V[(opcode & 0x0F00) >> 8] == context.V[(opcode & 0x00F0) >> 4]) context.pc += 2;
            break;
        case 0x6:
            context.V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            break;
        case 0x7:
            context.V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            break;
        case 0x8:
            switch (opcode & 0x000F) {
                case 0x0:
                    context.V[(opcode & 0x0F00) >> 8] = context.V[(opcode & 0x00F0) >> 4];
                    break;
                case 0x1:
                    context.V[(opcode & 0x0F00) >> 8] |= context.V[(opcode & 0x00F0) >> 4];
                    context.V[0xF] = 0x0;
                    break;
                case 0x2:
                    context.V[(opcode & 0x0F00) >> 8] &= context.V[(opcode & 0x00F0) >> 4];
                    context.V[0xF] = 0x0;
                    break;
                case 0x3:
                    context.V[(opcode & 0x0F00) >> 8] ^= context.V[(opcode & 0x00F0) >> 4];
                    context.V[0xF] = 0x0;
                    break;
                case 0x4:
                    context.V[(opcode & 0x0F00) >> 8] += context.V[(opcode & 0x00F0) >> 4];
                    if (context.V[(opcode & 0x0F00) >> 8] + context.V[(opcode & 0x00F0) >> 4] > 0xFF)
                        context.V[0xF] = 0x1;
                    else context.V[0xF] = 0x0;
                    break;
                case 0x5:
                    context.V[(opcode & 0x0F00) >> 8] -= context.V[(opcode & 0x00F0) >> 4];
                    if (context.V[(opcode & 0x0F00) >> 8] >= context.V[(opcode & 0x00F0) >> 4]) context.V[0xF] = 0x1;
                    else context.V[0xF] = 0x0;
                    break;
                case 0x6:
                    context.V[0xF] = context.V[(opcode & 0x00F0) >> 4] & 0x01;
                    context.V[(opcode & 0x0F00) >> 8] = context.V[(opcode & 0x00F0) >> 4] >> 1;
                    break;
                case 0x7:
                    context.V[(opcode & 0x0F00) >> 8] =
                            context.V[(opcode & 0x00F0) >> 4] - context.V[(opcode & 0x0F00) >> 8];
                    if (context.V[(opcode & 0x0F00) >> 8] <= context.V[(opcode & 0x00F0) >> 4]) context.V[0xF] = 0x1;
                    else context.V[0xF] = 0x0;
                    break;
                case 0xE:
                    context.V[0xF] = context.V[(opcode & 0x00F0) >> 4] & 0x80; // get the rightmost bit
                    context.V[(opcode & 0x0F00) >> 8] = context.V[(opcode & 0x00F0) >> 4] << 1;
                    break;
            default:
                    std::cout << "Opcode 0x8 not implemented, opcode :" << std::uppercase << std::hex << opcode << std::endl;
            }
            break;
        case 0x9:
            if (context.V[(opcode & 0x0F00) >> 8] != context.V[(opcode & 0x00F0) >> 4]) context.pc += 2;
            break;
        case 0xA:
            context.I = opcode & 0x0FFF;
            break;
        case 0xB:
            context.pc = context.V[0x0] + (opcode & 0x0FFF);
            break;
        case 0xC: {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<uint8_t> dist(0, 255);
            const uint8_t random = dist(gen);

            context.V[(opcode & 0x0F00) >> 8] = random & (opcode & 0x00FF);
            break;
        }
        case 0xD: {
            const uint8_t x = context.V[(opcode & 0x0F00) >> 8] % 64;
            const uint8_t y = context.V[(opcode & 0x00F0) >> 4] % 32;
            const uint8_t n = opcode & 0x000F;
            context.V[0xF] = 0x0;

            for (uint8_t i = 0; i < n; i++) {
                if (y + i >= 32) break;
                const uint8_t spriteRow = context.memory[context.I + i];
                for (uint8_t j = 0; j < 8; j++) {
                    if (x + j >= 64) break;

                    if (context.display[64 * (y + i) + x + j] && (spriteRow >> (7 - j)) & 0x1) context.V[0xF] = 0x1;
                    context.display[64 * (y + i) + x + j] ^= (spriteRow >> (7 - j)) & 0x1;
                }
            }

            break;
        }
        case 0xE:
            if ((opcode & 0x00FF) == 0x9E) {
                if (context.V[(opcode & 0x0F00) >> 8] == context.keyPressed) context.pc += 2;
            }

            if ((opcode & 0x00FF) == 0xA1) {
                if (context.V[(opcode & 0x0F00) >> 8] != context.keyPressed) context.pc += 2;
            }
            break;
        case 0xF:
            switch (opcode & 0x00FF) {
                case 0x07:
                    context.V[(opcode & 0x0F00) >> 8] = context.delayTimer;
                    break;
                case 0x0A:
                    if (context.keyPressed == 0xFF) {
                        context.pc -= 2;
                        break;
                    }

                    context.V[(opcode & 0x0F00) >> 8] = context.keyPressed;
                    context.keyPressed = 0xFF;

                    break;
                case 0x15:
                    context.delayTimer = context.V[(opcode & 0x0F00) >> 8];
                    break;
                case 0x18:
                    context.soundTimer = context.V[(opcode & 0x0F00) >> 8];
                    break;
                case 0x1E:
                    context.I += context.V[(opcode & 0x0F00) >> 8];
                    break;
                case 0x29:
                    context.I = 0x50 + (context.V[(opcode & 0x0F00) >> 8] & 0x0F) * 5;
                    break;
                case 0x33:
                    context.memory[context.I] = context.V[(opcode & 0x0F00) >> 8] / 100;
                    context.memory[context.I + 1] = (context.V[(opcode & 0x0F00) >> 8] % 100) / 10;
                    context.memory[context.I + 2] = context.V[(opcode & 0x0F00) >> 8] % 10;
                    break;
                case 0x55:
                    for (uint32_t i = 0x0; i <= ((opcode & 0x0F00) >> 8); i++)
                        context.memory[context.I + i] = context.V[i];
                    context.I += ((opcode & 0x0F00) >> 8) + 1;
                    break;
                case 0x65:
                    for (uint32_t i = 0x0; i <= ((opcode & 0x0F00) >> 8); i++)
                        context.V[i] = context.memory[context.I + i];
                    context.I += ((opcode & 0x0F00) >> 8) + 1;
                    break;
                default:
                    std::cout << "Opcode 0xF not implemented, opcode : " << std::uppercase << std::hex << opcode << std::endl;
            }
            break;
        default:
            std::cout << "Opcode not implemented, opcode : " << std::uppercase << std::hex << opcode << std::endl;
    }
}

void loadRomIntoMemory(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    file.read(reinterpret_cast<char *>(context.memory + 0x200), 0x1000 - 0x200);
    file.close();
}

void startChip8(const std::string &filename) {
    loadFontsIntoMemory();
    loadRomIntoMemory(filename);
}

void decrementTimers() {
    if (context.delayTimer > 0) context.delayTimer--;
    if (context.soundTimer > 0) context.soundTimer--;
}