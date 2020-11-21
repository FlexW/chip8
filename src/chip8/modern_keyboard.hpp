#pragma once

#include "keyboard.hpp"

namespace Chip8
{

class ModernKeyboard : public Keyboard
{
public:
  bool is_key_pressed(const unsigned char value) override;
};

} // namespace Chip8
