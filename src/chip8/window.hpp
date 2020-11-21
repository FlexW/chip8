#pragma once

namespace Chip8
{

class Window
{
public:
  virtual ~Window() = default;

  virtual bool is_closed() = 0;

  virtual void flush() = 0;

  virtual void terminate() = 0;
};

} // namespace Chip8
