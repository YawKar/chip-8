#include "Chip8.hpp"

#include <chrono>
#include <fstream>
#include <memory>

namespace chip {

Chip8::Chip8()
    : rand_gen(std::chrono::system_clock::now().time_since_epoch().count()),
      rand_byte(0, 255u) {
  program_counter = START_ADDRESS;
  for (uint32_t i = 0; i < FONTSET_SIZE; ++i) {
    memory[FONTSET_START_ADDRESS + i] = fontset[i];
  }
}

void Chip8::load_ROM(const std::string& filename) {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  if (file.is_open()) {
    std::streampos size = file.tellg();
    std::unique_ptr<char[]> buffer(new char[size]);
    file.seekg(0, std::ios::beg);
    file.read(buffer.get(), size);
    file.close();
    for (uint32_t offset = 0; offset < size; ++offset) {
      memory[START_ADDRESS + offset] = buffer[offset];
    }
  }
}

void Chip8::OP_00E0() {
  for (uint32_t i = 0; i < 64 * 32; ++i) {
    video[i] = 0;
  }
}
void Chip8::OP_00EE() { program_counter = stack[stack_pointer--]; }
void Chip8::OP_1NNN() { program_counter = opcode & 0xFFFu; }
void Chip8::OP_2NNN() {
  stack[++stack_pointer] = program_counter;
  program_counter = opcode & 0xFFFu;
}
void Chip8::OP_3XKK() {
  if (registers[(opcode & 0xF00u) >> 8] == (opcode & 0xFFu)) {
    program_counter += 2;
  }
}
void Chip8::OP_4XKK() {
  if (registers[(opcode & 0xF00u) >> 8] != (opcode & 0xFFu)) {
    program_counter += 2;
  }
}
void Chip8::OP_5XY0() {
  if (registers[(opcode & 0xF00u) >> 8] == registers[(opcode & 0xF0u) >> 4]) {
    program_counter += 2;
  }
}
void Chip8::OP_6XKK() { registers[(opcode & 0xF00u) >> 8] = (opcode & 0xFFu); }
void Chip8::OP_7XKK() { registers[(opcode & 0xF00u) >> 8] += (opcode & 0xFFu); }
void Chip8::OP_8XY0() {
  registers[(opcode & 0xF00u) >> 8] = registers[(opcode & 0xF0u) >> 4];
}
void Chip8::OP_8XY1() {
  registers[(opcode & 0xF00u) >> 8] |= registers[(opcode & 0xF0u) >> 4];
}
void Chip8::OP_8XY2() {
  registers[(opcode & 0xF00u) >> 8] &= registers[(opcode & 0xF0u) >> 4];
}
void Chip8::OP_8XY3() {
  registers[(opcode & 0xF00u) >> 8] ^= registers[(opcode & 0xF0u) >> 4];
}
void Chip8::OP_8XY4() {
  uint8_t prev_value = registers[(opcode & 0xF00u) >> 8];
  registers[(opcode & 0xF00u) >> 8] += registers[(opcode & 0xF0u) >> 4];
  registers[0xF] = prev_value > registers[(opcode & 0xF00u) >> 8];
}
void Chip8::OP_8XY5() {
  uint8_t prev_value = registers[(opcode & 0xF00u) >> 8];
  registers[(opcode & 0xF00u) >> 8] -= registers[(opcode & 0xF0u) >> 4];
  registers[0xF] = prev_value > registers[(opcode & 0xF00u) >> 8];
}
void Chip8::OP_8XY6() {
  registers[0xF] = (registers[(opcode & 0xF00u) >> 8] & 1);
  registers[(opcode & 0xF00u) >> 8] >>= 1;
}
void Chip8::OP_8XY7() {
  registers[(opcode & 0xF00u) >> 8] =
      registers[(opcode & 0xF0u) >> 4] - registers[(opcode & 0xF00u) >> 8];
  registers[0xF] =
      registers[(opcode & 0xF0u) >> 4] > registers[(opcode & 0xF00u) >> 8];
}
void Chip8::OP_8XYE() {
  registers[0xF] = (registers[(opcode & 0xF00u) >> 8] >> 16);
  registers[(opcode & 0xF00u) >> 8] <<= 1;
}
void Chip8::OP_9XY0() {
  if (registers[(opcode & 0xF00u) >> 8] != registers[(opcode & 0xF0u) >> 4]) {
    program_counter += 2;
  }
}
void Chip8::OP_ANNN() { index_register = (opcode & 0xFFFu); }
void Chip8::OP_BNNN() { program_counter = (opcode & 0xFFFu) + registers[0x0u]; }
void Chip8::OP_CXKK() {
  registers[(opcode & 0xF00u) >> 8] = (rand_byte(rand_gen) & (opcode & 0xFFu));
}
void Chip8::OP_DXYN() {
  const uint8_t n = (opcode & 0xFu);
  const uint8_t x = static_cast<uint8_t>(opcode & 0xF00u);
  const uint8_t y = (opcode & 0xF0u);
  registers[0xF] = 0;
  for (uint8_t offset = 0; offset < n; ++offset) {
    const uint8_t byte = memory[index_register + offset];
    for (int16_t bit = 7; bit >= 0; --bit) {
      uint32_t prev = video[((x + bit) + 8 * (y + offset)) % (64 * 32)];
      video[((x + bit) + 8 * (y + offset)) % (64 * 32)] ^= (byte >> bit);
      registers[0xF] |=
          prev > video[((x + bit) + 8 * (y + offset)) % (64 * 32)];
    }
  }
}
void Chip8::OP_EX9E() {
  if (keypad[registers[(opcode & 0xF00u) >> 8]]) {
    program_counter += 2;
  }
}
void Chip8::OP_EXA1() {
  if (!keypad[registers[(opcode & 0xF00u) >> 8]]) {
    program_counter += 2;
  }
}
void Chip8::OP_FX07() { registers[(opcode & 0xF00u) >> 8] = delay_timer; }
void Chip8::OP_FX0A() {
  for (uint8_t i = 0; i < 16; ++i) {
    if (keypad[i]) {
      registers[(opcode & 0xF00u) >> 8] = i;
      return;
    }
  }
  program_counter -= 2;
}
void Chip8::OP_FX15() { delay_timer = registers[(opcode & 0xF00u) >> 8]; }
void Chip8::OP_FX18() { sound_timer = registers[(opcode & 0xF00u) >> 8]; }
void Chip8::OP_FX1E() { index_register += registers[(opcode & 0xF00u) >> 8]; }
void Chip8::OP_FX29() {
  index_register = fontset[registers[(opcode & 0xF00u) >> 8] * 5];
}
void Chip8::OP_FX33() {
  uint16_t value = registers[(opcode & 0xF00u) >> 8];
  memory[index_register] = static_cast<uint8_t>(value / 100);
  memory[index_register + 1] = static_cast<uint8_t>((value % 100) / 10);
  memory[index_register + 2] = static_cast<uint8_t>(value % 10);
}
void Chip8::OP_FX55() {
  for (uint8_t i = 0; i <= registers[(opcode & 0xF00u) >> 8]; ++i) {
    memory[index_register + i * 2] = ((registers[i] & 0xFF00u) >> 8);
    memory[index_register + i * 2 + 1] = (registers[i] & 0x00FFu);
  }
}
void Chip8::OP_FX65() {
  for (uint8_t i = 0; i <= registers[(opcode & 0xF00u) >> 8]; ++i) {
    registers[i] = (memory[index_register + i * 2] << 8);
    registers[i] |= memory[index_register + i * 2 + 1];
  }
}
}  // namespace chip