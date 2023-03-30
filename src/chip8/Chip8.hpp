#ifndef CHIP8_HPP
#define CHIP8_HPP
#include <cstdint>
#include <random>
#include <string>

namespace chips {
class Chip8 {
 public:
  static const uint32_t START_ADDRESS = 0x200;
  static const uint16_t VIDEO_HEIGHT = 32;
  static const uint16_t VIDEO_WIDTH = 64;
  static const uint32_t FONTSET_START_ADDRESS = 0x50;
  static const uint32_t FONTSET_SIZE = 80;
  static const uint32_t FONTSET_SYMBOL_HEIGHT = 5;
  const uint8_t fontset[FONTSET_SIZE]{
      0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
      0x20, 0x60, 0x20, 0x20, 0x70,  // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
      0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
      0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
      0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
      0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
      0xF0, 0x80, 0xF0, 0x80, 0x80   // F
  };

  uint8_t registers[16]{0};
  uint8_t stack_pointer;
  uint16_t index_register;
  uint16_t program_counter;
  uint8_t delay_timer;
  uint8_t sound_timer;
  uint16_t opcode;
  uint8_t memory[4096]{0};
  uint8_t keypad[16]{0};
  uint16_t stack[16]{0};
  uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT]{0};
  std::default_random_engine rand_gen;
  std::uniform_int_distribution<uint8_t> rand_byte;

  Chip8();

  void load_ROM(const std::string& filename);
  void cycle();
  void execute_opcode();
  void OP_00E0();
  void OP_00EE();
  void OP_1NNN();
  void OP_2NNN();
  void OP_3XKK();
  void OP_4XKK();
  void OP_5XY0();
  void OP_6XKK();
  void OP_7XKK();
  void OP_8XY0();
  void OP_8XY1();
  void OP_8XY2();
  void OP_8XY3();
  void OP_8XY4();
  void OP_8XY5();
  void OP_8XY6();
  void OP_8XY7();
  void OP_8XYE();
  void OP_9XY0();
  void OP_ANNN();
  void OP_BNNN();
  void OP_CXKK();
  void OP_DXYN();
  void OP_EX9E();
  void OP_EXA1();
  void OP_FX07();
  void OP_FX0A();
  void OP_FX15();
  void OP_FX18();
  void OP_FX1E();
  void OP_FX29();
  void OP_FX33();
  void OP_FX55();
  void OP_FX65();
};
}  // namespace chips

#endif  // CHIP8_HPP