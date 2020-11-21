#include <cassert>
#include <iostream>
#include <stdexcept>

#include "glfw_window.hpp"

#define OPENGL_VERSION_MAJOR 4
#define OPENGL_VERSION_MINOR 6
#define OPENGL_DEBUG         true

#define WINDOW_TITLE "Chip8"

namespace Chip8
{

void window_error_callback(int error, const char *description)
{
  std::cerr << "Glfw Error [" << error << "]: " << description << std::endl;
}

void window_framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  auto w = static_cast<GlfwWindow *>(glfwGetWindowUserPointer(window));

  assert(w);

  w->on_window_framebuffer_size(width, height);
}

void window_close_callback(GLFWwindow *window)
{
  auto w = static_cast<GlfwWindow *>(glfwGetWindowUserPointer(window));

  assert(w);

  w->on_close();
}

void key_callback(GLFWwindow *window,
                  int         key,
                  int         scancode,
                  int         action,
                  int         mods)
{
  auto w = static_cast<GlfwWindow *>(glfwGetWindowUserPointer(window));

  assert(w);

  w->on_key(key, scancode, action, mods);
}

void GlfwWindow::create_window()
{
  if (!glfwInit())
  {
    throw std::runtime_error("Could not initialize glfw");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  if (OPENGL_DEBUG)
  {
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  }

  glfwSetErrorCallback(window_error_callback);

  glfw_window = glfwCreateWindow(WINDOW_WIDTH,
                                 WINDOW_HEIGHT,
                                 WINDOW_TITLE,
                                 nullptr,
                                 nullptr);
  if (!glfw_window)
  {
    throw std::runtime_error("Unable to create glfw window");
  }
  glfwSetWindowUserPointer(glfw_window, this);

  glfwMakeContextCurrent(glfw_window);

  glfwSetFramebufferSizeCallback(glfw_window, window_framebuffer_size_callback);
  glfwSetWindowCloseCallback(glfw_window, window_close_callback);
  glfwSetKeyCallback(glfw_window, key_callback);

  glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  if (!gladLoadGL())
  {
    throw std::runtime_error("Could not initialze glad");
  }
}

void GlfwWindow::on_key(int /*key*/,
                        int /*scancode*/,
                        int /*action*/,
                        int /*mods*/)
{
}

void GlfwWindow::on_window_framebuffer_size(int width, int height)
{
  window_width  = width;
  window_height = height;
}

void GlfwWindow::on_close() { closed = true; }

bool GlfwWindow::is_closed() { return closed; }

void GlfwWindow::flush()
{
  glfwSwapBuffers(glfw_window);
  glfwPollEvents();
}

void GlfwWindow::terminate() { glfwTerminate(); }

} // namespace Chip8
