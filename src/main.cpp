#include <chrono>
#include <iostream>
#include <thread>

#include "chip8/Chip8.hpp"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    return 0;
  }
  chips::Chip8 chip;
  chip.load_ROM(argv[1]);
  while (true) {
    chip.cycle();
    std::string out;
    for (int i = 0; i < chip.VIDEO_HEIGHT; ++i) {
      for (int j = 0; j < chip.VIDEO_WIDTH; ++j) {
        out += (chip.video[i * chip.VIDEO_WIDTH + j] ? "⬜" : "⬛");
      }
      out += '\n';
    }
    std::cout << out;

    for (int i = 0; i < 16; ++i) {
      chip.keypad[i] = false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
    std::cout << "\x1B[2J\x1B[H";
  }
  return 0;
}
