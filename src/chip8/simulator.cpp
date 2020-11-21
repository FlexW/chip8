#include <fstream>
#include <ios>
#include <iterator>
#include <memory>
#include <sys/time.h>

#include "cpu.hpp"
#include "glfw_window.hpp"
#include "modern_keyboard.hpp"
#include "opengl_renderer.hpp"
#include "simulator.hpp"

namespace Chip8
{

int64_t get_current_time_millis()
{
  timeval t;
  gettimeofday(&t, nullptr);

  auto ret = t.tv_sec * 1000 + t.tv_usec / 1000;

  return ret;
}

Simulator::Simulator()
{
  auto glfw_window = std::make_shared<GlfwWindow>();
  window           = glfw_window;

  renderer      = std::make_shared<OpenGlRenderer>(glfw_window);
  auto keyboard = std::make_unique<ModernKeyboard>();

  cpu = std::make_unique<Cpu>(renderer, std::move(keyboard));
  cpu->init();
}

void Simulator::load_program(const std::string &filepath)
{
  const auto program = load_program_from_disk(filepath);
  cpu->load_program(program);
}

void Simulator::execute()
{
  double accumulator = 0.0;
  double old_time    = double(get_current_time_millis()) / 1000.0;

  while (!window->is_closed())
  {
    const double current_time = double(get_current_time_millis()) / 1000.0;
    const double delta_time   = current_time - old_time;
    old_time                  = current_time;

    accumulator += delta_time;

    while (accumulator > 1.0 / fps)
    {
      cpu->cycle();
      accumulator -= 1.0 / fps;
    }

    renderer->render();
    window->flush();
  }
}

std::vector<byte_t>
Simulator::load_program_from_disk(const std::string &filepath)
{
  std::ifstream in(filepath.c_str(), std::ios::in | std::ios::binary);
  in.exceptions(std::ios_base::badbit | std::ios_base::failbit |
                std::ios_base::eofbit);

  return std::vector<byte_t>((std::istreambuf_iterator<char>(in)),
                             std::istreambuf_iterator<char>());
}

void Simulator::terminate()
{
  renderer->terminate();
  window->terminate();
}

} // namespace Chip8
