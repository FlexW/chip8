#pragma once

#include <array>
#include <memory>

#include "glfw_window.hpp"
#include "renderer.hpp"

namespace Chip8
{

class OpenGlRenderer : public Renderer
{
public:
  OpenGlRenderer(std::shared_ptr<GlfwWindow> glfw_window);

  void create_window() override;

  void clear_display() override;

  bool set_pixel(uint32_t x, uint32_t y) override;

  void render() override;

  void terminate() override;

private:
  std::array<std::array<unsigned char, 32>, 64> pixel_data{};

  std::shared_ptr<GlfwWindow> glfw_window{};

  uint32_t shader_id{};
  uint32_t quad_vao_id{};
  uint32_t quad_vbo_id{};

  uint32_t pixel_data_tex_id{};

  void load_shaders();

  void check_for_shader_compile_errors(uint32_t id, const char *name);

  void check_for_shader_program_compile_errors(uint32_t id);

  void load_quad();

  void create_pixel_data_tex();
};

} // namespace Chip8
