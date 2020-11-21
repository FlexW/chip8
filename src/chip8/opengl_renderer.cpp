#include <GL/gl.h>
#include <cassert>
#include <stdexcept>
#include <string>

#include "opengl_renderer.hpp"

#define GLSL_SHADER_CODE(code) "#version 460 core\n" #code

namespace Chip8
{

OpenGlRenderer::OpenGlRenderer(std::shared_ptr<GlfwWindow> glfw_window)
    : glfw_window(glfw_window)
{
}

void OpenGlRenderer::create_window()
{
  glfw_window->create_window();

  // Setup opengl
  // glEnable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  load_shaders();
  load_quad();
  create_pixel_data_tex();
}

void OpenGlRenderer::clear_display() {}

bool OpenGlRenderer::set_pixel(uint32_t x, uint32_t y)
{
  assert(x <= 63);
  assert(y <= 31);

  bool set = false;

  if (pixel_data[x][y] == 0)
  {
    set              = true;
    pixel_data[x][y] = 1;
  }
  else
  {
    set              = false;
    pixel_data[x][y] = 0;
  }

  return set;
}

void OpenGlRenderer::render()
{
  glClear(GL_COLOR_BUFFER_BIT /* | GL_DEPTH_BUFFER_BIT */);
  glViewport(0, 0, glfw_window->get_width(), glfw_window->get_height());

  glUseProgram(shader_id);

  const float window_width  = static_cast<float>(glfw_window->get_width());
  const float window_height = static_cast<float>(glfw_window->get_height());

  const float step_width  = (window_width / 63.0) / window_width;
  const float step_height = (window_height / 31.0) / window_height;

  glUniform1f(glGetUniformLocation(shader_id, "step_width"), step_width);
  glUniform1f(glGetUniformLocation(shader_id, "step_height"), step_height);

  glBindVertexArray(quad_vao_id);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void OpenGlRenderer::load_shaders()
{
  // clang-format off
  const char *vertex_shader_code = GLSL_SHADER_CODE(
    layout(location = 0) in vec3 in_position;
    layout(location = 1) in vec2 in_tex_coord;

    out vec2 frag_tex_coord;

    void main()
    {
      frag_tex_coord = in_tex_coord;
      gl_Position    = vec4(in_position, 1.0);
    }
    );

  const char *fragment_shader_code = GLSL_SHADER_CODE(
      in vec2 frag_tex_coord;
      layout(location = 0) out vec4 out_color;

      void main()
      {
        out_color = vec4(frag_tex_coord, 0.0, 1.0);
      }
    );

  // clang-format on

  const uint32_t vs_id = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs_id, 1, &vertex_shader_code, nullptr);
  glCompileShader(vs_id);
  check_for_shader_compile_errors(vs_id, "Vertex");

  const uint32_t fs_id = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs_id, 1, &fragment_shader_code, nullptr);
  glCompileShader(fs_id);
  check_for_shader_compile_errors(fs_id, "Fragment");

  shader_id = glCreateProgram();
  glAttachShader(shader_id, vs_id);
  glAttachShader(shader_id, fs_id);

  glLinkProgram(shader_id);
  check_for_shader_program_compile_errors(shader_id);

  glDeleteShader(vs_id);
  glDeleteShader(fs_id);
}

void OpenGlRenderer::check_for_shader_compile_errors(uint32_t    id,
                                                     const char *name)
{
  GLint     success;
  const int info_log_size = 1024;
  GLchar    info_log[info_log_size];

  glGetShaderiv(id, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(id, info_log_size, nullptr, info_log);

    std::string message = std::string(name) + " Shader Error:\n" + info_log;
    throw std::runtime_error(message);
  }
}

void OpenGlRenderer::check_for_shader_program_compile_errors(uint32_t id)
{
  GLint     success;
  const int info_log_size = 1024;
  GLchar    info_log[info_log_size];

  glGetProgramiv(id, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(id, info_log_size, nullptr, info_log);

    throw std::runtime_error(std::string("Shader Program Error:\n") + info_log);
  }
}

void OpenGlRenderer::load_quad()
{
  float quad_data[] = {
      -1.0f, 1.0f,  0.0f,

      0.0f,  0.0f,

      -1.0f, -1.0f, 0.0f,

      0.0f,  1.0f,

      1.0f,  1.0f,  0.0f,

      1.0f,  0.0f,

      1.0f,  -1.0f, 0.0f,

      1.0f,  1.0f,
  };

  // Generate Vertex Buffer
  glGenBuffers(1, &quad_vbo_id);
  glBindBuffer(GL_ARRAY_BUFFER, quad_vbo_id);

  glBufferData(GL_ARRAY_BUFFER,
               4 * (3 + 2) * sizeof(float),
               quad_data,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate Vertex Array
  glGenVertexArrays(1, &quad_vao_id);
  glBindVertexArray(quad_vao_id);

  glBindBuffer(GL_ARRAY_BUFFER, quad_vbo_id);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3 + 2) * sizeof(float), 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        (3 + 2) * sizeof(float),
                        reinterpret_cast<void *>(3 * sizeof(float)));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void OpenGlRenderer::terminate()
{
  glDeleteProgram(shader_id);
  glDeleteVertexArrays(1, &quad_vao_id);
  glDeleteBuffers(1, &quad_vbo_id);
}

void OpenGlRenderer::create_pixel_data_tex()
{
  glGenTextures(1, &pixel_data_tex_id);
  glBindTexture(GL_TEXTURE_2D, pixel_data_tex_id);
}

} // namespace Chip8
