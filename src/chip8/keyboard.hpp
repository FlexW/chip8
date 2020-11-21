#pragma once

namespace Chip8
{

class Keyboard
{
public:
  virtual ~Keyboard() = default;

  virtual bool is_key_pressed(const unsigned char value) = 0;
};

} // namespace Chip8
