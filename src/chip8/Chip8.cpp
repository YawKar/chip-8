#include "Chip8.hpp"

#include <fmt/core.h>

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

void Chip8::cycle() {
  opcode = ((memory[program_counter] << 8u) | memory[program_counter + 1]);
  program_counter += 2;
  execute_opcode();
  if (delay_timer > 0) delay_timer--;
  if (sound_timer > 0) sound_timer--;
}

void Chip8::execute_opcode() {
  switch (opcode >> 12u) {
    case 0:
      switch (opcode & 0x0FFFu) {
        case 0x0E0u:
          OP_00E0();
          break;
        case 0x0EEu:
          OP_00EE();
          break;
        default:
          throw std::runtime_error(
              fmt::format("Unknown opcode: {0:#06x}.", opcode));
          break;
      }
      break;
    case 1:
      OP_1NNN();
      break;
    case 2:
      OP_2NNN();
      break;
    case 3:
      OP_3XKK();
      break;
    case 4:
      OP_4XKK();
      break;
    case 5:
      OP_5XY0();
      break;
    case 6:
      OP_6XKK();
      break;
    case 7:
      OP_7XKK();
      break;
    case 8:
      switch (opcode & 0x000Fu) {
        case 0:
          OP_8XY0();
          break;
        case 1:
          OP_8XY1();
          break;
        case 2:
          OP_8XY2();
          break;
        case 3:
          OP_8XY3();
          break;
        case 4:
          OP_8XY4();
          break;
        case 5:
          OP_8XY5();
          break;
        case 6:
          OP_8XY6();
          break;
        case 7:
          OP_8XY7();
          break;
        case 0xEu:
          OP_8XYE();
          break;
        default:
          throw std::runtime_error(
              fmt::format("Unknown opcode: {0:#06x}.", opcode));
          break;
      }
      break;
    case 9:
      if (!(opcode & 0x000Fu)) {
        OP_9XY0();
      } else {
        throw std::runtime_error(
            fmt::format("Unknown opcode: {0:#06x}.", opcode));
      }
      break;
    case 0xAu:
      OP_ANNN();
      break;
    case 0xBu:
      OP_BNNN();
      break;
    case 0xCu:
      OP_CXKK();
      break;
    case 0xDu:
      OP_DXYN();
      break;
    case 0xEu:
      switch (opcode & 0x00FFu) {
        case 0x9Eu:
          OP_EX9E();
          break;
        case 0xA1u:
          OP_EXA1();
          break;
        default:
          throw std::runtime_error(
              fmt::format("Unknown opcode: {0:#06x}.", opcode));
          break;
      }
      break;
    case 0xFu:
      switch (opcode & 0x00FFu) {
        case 0x07u:
          OP_FX07();
          break;
        case 0x0Au:
          OP_FX0A();
          break;
        case 0x15u:
          OP_FX15();
          break;
        case 0x18u:
          OP_FX18();
          break;
        case 0x1Eu:
          OP_FX1E();
          break;
        case 0x29u:
          OP_FX29();
          break;
        case 0x33u:
          OP_FX33();
          break;
        case 0x55u:
          OP_FX55();
          break;
        case 0x65u:
          OP_FX65();
          break;
        default:
          throw std::runtime_error(
              fmt::format("Unknown opcode: {0:#06x}.", opcode));
          break;
      }
      break;
    default:
      throw std::runtime_error(
          fmt::format("Unknown opcode: {0:#06x}.", opcode));
      break;
  }
}

void Chip8::OP_00E0() {
  for (uint32_t i = 0; i < 64 * 32; ++i) {
    video[i] = 0;
  }
}
void Chip8::OP_00EE() {
  stack_pointer--;
  program_counter = stack[stack_pointer];
}
void Chip8::OP_1NNN() {
  uint16_t address = (opcode & 0x0FFFu);
  program_counter = address;
}
void Chip8::OP_2NNN() {
  stack[stack_pointer] = program_counter;
  stack_pointer++;
  uint16_t address = (opcode & 0x0FFFu);
  program_counter = address;
}
void Chip8::OP_3XKK() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  uint8_t kk = (opcode & 0x00FFu);
  if (registers[x] == kk) {
    program_counter += 2;
  }
}
void Chip8::OP_4XKK() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  uint8_t kk = (opcode & 0x00FFu);
  if (registers[x] != kk) {
    program_counter += 2;
  }
}
void Chip8::OP_5XY0() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  uint8_t y = ((opcode & 0x00F0u) >> 4u);
  if (registers[x] == registers[y]) {
    program_counter += 2;
  }
}
void Chip8::OP_6XKK() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  uint8_t kk = (opcode & 0x00FFu);
  registers[x] = kk;
}
void Chip8::OP_7XKK() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  uint8_t kk = (opcode & 0x00FFu);
  registers[x] += kk;
}
void Chip8::OP_8XY0() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  uint8_t y = ((opcode & 0x00F0u) >> 4u);
  registers[x] = registers[y];
}
void Chip8::OP_8XY1() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  uint8_t y = ((opcode & 0x00F0u) >> 4u);
  registers[x] |= registers[y];
}
void Chip8::OP_8XY2() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  uint8_t y = ((opcode & 0x00F0u) >> 4u);
  registers[x] &= registers[y];
}
void Chip8::OP_8XY3() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  uint8_t y = ((opcode & 0x00F0u) >> 4u);
  registers[x] ^= registers[y];
}
void Chip8::OP_8XY4() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  uint8_t y = ((opcode & 0x00F0u) >> 4u);
  uint16_t sum = registers[x] + registers[y];
  registers[0xFu] = sum > 255u;
  registers[x] = (sum & 0xFFu);
}
void Chip8::OP_8XY5() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  uint8_t y = ((opcode & 0x00F0u) >> 4u);
  registers[0xFu] = registers[x] > registers[y];
  registers[x] -= registers[y];
}
void Chip8::OP_8XY6() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  registers[0xFu] = (registers[x] & 0x1u);
  registers[x] >>= 1u;
}
void Chip8::OP_8XY7() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  uint8_t y = ((opcode & 0x00F0u) >> 4u);
  registers[0xFu] = registers[y] > registers[x];
  registers[x] = registers[y] - registers[x];
}
void Chip8::OP_8XYE() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  registers[0xFu] = (registers[x] >> 7u);
  registers[x] <<= 1u;
}
void Chip8::OP_9XY0() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  uint8_t y = ((opcode & 0x00F0u) >> 4u);
  if (registers[x] != registers[y]) {
    program_counter += 2;
  }
}
void Chip8::OP_ANNN() {
  uint16_t address = (opcode & 0x0FFFu);
  index_register = address;
}
void Chip8::OP_BNNN() {
  uint16_t address = (opcode & 0x0FFFu);
  program_counter = address + registers[0x0u];
}
void Chip8::OP_CXKK() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  uint8_t kk = (opcode & 0x00FFu);
  registers[x] = (rand_byte(rand_gen) & kk);
}
void Chip8::OP_DXYN() {
  const uint8_t height = (opcode & 0x000Fu);
  const uint8_t x = ((opcode & 0x0F00u) >> 8u);
  const uint8_t y = ((opcode & 0x00F0u) >> 4u);
  registers[0xFu] = 0u;

  for (uint32_t byte_idx = 0; byte_idx < height; ++byte_idx) {
    uint8_t byte = memory[index_register + byte_idx];
    for (uint8_t bit = 7u; bit != 255u; --bit) {
      uint8_t sprite_bit = ((byte >> bit) & 0x1u);
      uint32_t* video_bit = &video[(registers[x] + (7u - bit) +
                                    (registers[y] + byte_idx) * VIDEO_WIDTH) %
                                   (VIDEO_WIDTH * VIDEO_HEIGHT)];
      if (sprite_bit) {
        registers[0xFu] |= (sprite_bit && *video_bit);
        *video_bit ^= sprite_bit;
      }
    }
  }
}
void Chip8::OP_EX9E() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  if (keypad[registers[x]]) {
    program_counter += 2;
  }
}
void Chip8::OP_EXA1() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  if (!keypad[registers[x]]) {
    program_counter += 2;
  }
}
void Chip8::OP_FX07() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  registers[x] = delay_timer;
}
void Chip8::OP_FX0A() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  for (uint8_t i = 0; i < 16; ++i) {
    if (keypad[i]) {
      registers[x] = i;
      return;
    }
  }
  program_counter -= 2;
}
void Chip8::OP_FX15() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  delay_timer = registers[x];
}
void Chip8::OP_FX18() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  sound_timer = registers[x];
}
void Chip8::OP_FX1E() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  index_register += registers[x];
}
void Chip8::OP_FX29() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  index_register = static_cast<uint16_t>(FONTSET_START_ADDRESS +
                                         registers[x] * FONTSET_SYMBOL_HEIGHT);
}
void Chip8::OP_FX33() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  uint16_t value = registers[x];
  memory[index_register + 2] = static_cast<uint8_t>(value % 10u);
  value /= 10u;
  memory[index_register + 1] = static_cast<uint8_t>(value % 10u);
  value /= 10u;
  memory[index_register] = static_cast<uint8_t>(value % 10u);
}
void Chip8::OP_FX55() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  for (uint8_t i = 0; i <= x; ++i) {
    memory[index_register + i] = registers[i];
  }
}
void Chip8::OP_FX65() {
  uint8_t x = ((opcode & 0x0F00u) >> 8u);
  for (uint8_t i = 0; i <= x; ++i) {
    registers[i] = memory[index_register + i];
  }
}
}  // namespace chip