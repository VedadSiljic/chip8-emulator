#include "chip-8.h"

#include <algorithm>
#include <iostream>
#include <random>
#include <fstream>

Chip8 context;

bool *getDisplay() {
    return context.display;
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
                    break;
                case 0x2:
                    context.V[(opcode & 0x0F00) >> 8] &= context.V[(opcode & 0x00F0) >> 4];
                    break;
                case 0x3:
                    context.V[(opcode & 0x0F00) >> 8] ^= context.V[(opcode & 0x00F0) >> 4];
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
                    context.V[0xF] = context.V[(opcode & 0x0F00) >> 8] & 0x01;
                    context.V[(opcode & 0x0F00) >> 8] >>= 1;
                    break;
                case 0x7:
                    context.V[(opcode & 0x0F00) >> 8] =
                            context.V[(opcode & 0x00F0) >> 4] - context.V[(opcode & 0x0F00) >> 8];
                    if (context.V[(opcode & 0x0F00) >> 8] <= context.V[(opcode & 0x00F0) >> 4]) context.V[0xF] = 0x1;
                    else context.V[0xF] = 0x0;
                    break;
                case 0xE:
                    context.V[0xF] = context.V[(opcode & 0x0F00) >> 8] & 0x80; // get the rightmost bit
                    context.V[(opcode & 0x0F00) >> 8] <<= 1;
                    break;
                default:
                    std::cout << "Opcode 0x8 not implemented" << std::endl;
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
            uint8_t random = dist(gen);

            context.V[(opcode & 0x0F00) >> 8] = random & (opcode & 0x00FF);
            break;
        }
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
                    if (!(context.display[64 * (y + i) + x + j] ^= (spriteRow >> (7 - j)) & 0x1)) context.V[0xF] = 0x1;
                    // if result of xor is 0 set F flag to 1
                }
            }

            break;
        }
        case 0xF:
            switch (opcode & 0x00FF) {
                case 0x1E:
                    context.I += context.V[(opcode & 0x0F00) >> 8];
                    break;
                case 0x33:
                    context.memory[context.I] = context.V[(opcode & 0x0F00) >> 8] / 100;
                    context.memory[context.I + 1] = (context.V[(opcode & 0x0F00) >> 8] % 100) / 10;
                    context.memory[context.I + 2] = context.V[(opcode & 0x0F00) >> 8] % 10;
                    break;
                case 0x55:
                    for (uint32_t i = 0x0; i <= ((opcode & 0x0F00) >> 8); i++)
                        context.memory[context.I + i] = context.V[i];
                    break;
                case 0x65:
                    for (uint32_t i = 0x0; i <= ((opcode & 0x0F00) >> 8); i++)
                        context.V[i] = context.memory[context.I + i];
                    break;
                default:
                    std::cout << "Opcode 0xF not implemented" << std::endl;
            }
            break;
        default:
            std::cout << "Opcode not implemented" << std::endl;
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

    /*const auto INSTRUCTION_TIME_DELAY = std::chrono::nanoseconds(1428571);
    const auto TIMER_TIME_DELAY  = std::chrono::nanoseconds(16666667);
    auto lastInstructionTime = std::chrono::steady_clock::now();
    auto lastTimerTime = std::chrono::steady_clock::now();

    while (true) {
        if (std::chrono::steady_clock::now() - lastInstructionTime >= INSTRUCTION_TIME_DELAY) {
            lastInstructionTime = std::chrono::steady_clock::now();
            fetchDecodeExecuteInstruction();
        }

        if (std::chrono::steady_clock::now() - lastTimerTime >= TIMER_TIME_DELAY) {
            lastTimerTime = std::chrono::steady_clock::now();
            // decrement both timers

            render(context.display);
        }
    }*/
}
