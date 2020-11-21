#pragma once

#include <memory>
#include <string>
#include <vector>

#include "cpu.hpp"
#include "glfw_window.hpp"
#include "renderer.hpp"
#include "window.hpp"

namespace Chip8
{

/**
 * @brief Simulator. Use this class to create the simulator.
 */
class Simulator
{
public:
  Simulator();

  /**
   * Load a program from disk into the cpu's memory.
   */
  void load_program(const std::string &filepath);

  /**
   * Execute the currently loaded program.
   */
  void execute();

  /**
   * Terminate the emulator.
   */
  void terminate();

private:
  uint32_t fps = 60;

  std::shared_ptr<Renderer> renderer{};
  std::shared_ptr<Window>   window{};
  std::unique_ptr<Cpu>      cpu{};

  std::vector<byte_t> load_program_from_disk(const std::string &filepath);
};

} // namespace Chip8
