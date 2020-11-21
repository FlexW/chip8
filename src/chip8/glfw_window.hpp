#pragma once

// clang-format off
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
// clang-format on

#include "window.hpp"

#define WINDOW_WIDTH  1024
#define WINDOW_HEIGHT 768

namespace Chip8
{

class GlfwWindow : public Window
{
public:
  void create_window();

  bool is_closed() override;

  void flush() override;

  void terminate() override;

  void on_key(int key, int scancode, int action, int mods);

  void on_window_framebuffer_size(int width, int height);

  void on_close();

  int32_t get_width() const { return window_width; }

  int32_t get_height() const { return window_height; }

private:
  GLFWwindow *glfw_window{};

  int32_t window_width  = WINDOW_WIDTH;
  int32_t window_height = WINDOW_HEIGHT;

  bool closed = false;
};

} // namespace Chip8
