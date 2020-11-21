#pragma once

#include <cstdint>

namespace Chip8
{

class Renderer
{
public:
  virtual ~Renderer() = default;

  virtual void create_window() = 0;

  virtual void clear_display() = 0;

  virtual bool set_pixel(uint32_t x, uint32_t y) = 0;

  virtual void render() = 0;

  virtual void terminate() = 0;
};

} // namespace Chip8
