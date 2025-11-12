#ifndef CHIP_8_CHIP_8_H
#define CHIP_8_CHIP_8_H
#include <cstdint>
#include <string>

struct Chip8 {
    uint8_t memory[0x1000]{};
    uint16_t pc = 0x200;
    uint8_t V[0x10]{}; // registers
    uint16_t I{}; // index register(also called address register) size 12 bits
    uint16_t stack[100]{};
    int16_t sp = -1;  // position of top element of the stack
    uint8_t delayTimer = 0, soundTimer = 0;
    bool display[64 * 32]{};
    uint16_t keyPressed = 0xFF;  // 0xFF for no input

};

void startChip8(const std::string &filename);
void fetchDecodeExecuteInstruction();
bool* getDisplay();
void setInput(uint8_t value);

#endif