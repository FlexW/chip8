#pragma once

#include <array>
#include <memory>
#include <random>
#include <stack>

#include "keyboard.hpp"
#include "renderer.hpp"

namespace Chip8
{

using byte_t  = unsigned char;
using dbyte_t = unsigned short;

/**
 * @brief The cpu of the chip8 simulator.
 *
 * This is the place where the memory lives and the instructions get
 * executed. A description of the chip8 architecture can be found on
 * http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#2.2
 */
class Cpu
{
public:
  Cpu(std::shared_ptr<Renderer> renderer, std::unique_ptr<Keyboard> keyboard);

  /**
   * Init the cpu. Load sprites. Init the renderer.
   */
  void init();

  /**
   * Load a program into the cpu memory.
   *
   * Throws a exception if the program is to long.
   *
   * @param program Program to load
   */
  void load_program(const std::vector<byte_t> &program);

  /**
   * Run one cpu cycle.
   */
  void cycle();

  bool is_paused() { return paused; }

private:
  const dbyte_t program_start = 0x200;

  /**
   * 4096 KB of memory.
   */
  std::array<byte_t, 4096> memory{};

  /**
   * 16 8-bit registers.
   */
  std::array<byte_t, 16> v_registers{};

  /**
   * Register for storing memory addresses.
   */
  dbyte_t i_register{};

  /**
   * Timer delay register.
   */
  byte_t timer_delay_register{};

  /**
   * Sound timer register.
   */
  byte_t sound_delay_register{};

  /**
   * Stack pointer register.
   */
  byte_t sp_register{};

  /**
   * Program counter register.
   */
  dbyte_t pc_register = program_start;

  /**
   * Stack.
   */
  std::stack<dbyte_t> stack{};

  bool paused = false;

  std::random_device                    random_device;
  std::default_random_engine            random_engine;
  std::uniform_int_distribution<byte_t> uniform_dist;

  std::shared_ptr<Renderer> renderer{};
  std::unique_ptr<Keyboard> keyboard{};

  void load_sprites();

  dbyte_t get_next_instruction();

  void increase_program_counter();

  void execute_instruction(const dbyte_t opcode);

  void update_timers();
};

} // namespace Chip8
