#include <bits/stdint-uintn.h>
#include <cstdint>
#include <stdexcept>

#include "cpu.hpp"

namespace Chip8
{

Cpu::Cpu(std::shared_ptr<Renderer> renderer, std::unique_ptr<Keyboard> keyboard)
    : random_engine(random_device()),
      uniform_dist(0, 255),
      renderer(renderer),
      keyboard(std::move(keyboard))

{
}

void Cpu::init()
{
  load_sprites();
  renderer->create_window();
}

void Cpu::load_program(const std::vector<byte_t> &program)
{
  for (size_t i = 0; i < program.size(); ++i)
  {
    if (program_start + i >= memory.size())
    {
      throw std::runtime_error("Program is to long");
    }

    memory[program_start + i] = program[i];
  }
}

void Cpu::cycle()
{
  if (paused)
  {
    return;
  }

  const dbyte_t opcode = get_next_instruction();
  execute_instruction(opcode);
  increase_program_counter();
  update_timers();
}

void Cpu::load_sprites()
{
  // Array of hex values for each sprite. Each sprite is 5 bytes.
  // The technical reference provides us with each one of these values.
  const std::array<byte_t, 80> sprites = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  // According to the technical reference, sprites are stored in the
  // interpreter section of memory starting at hex 0x000
  for (uint32_t i = 0; i < sprites.size(); i++)
  {
    memory[i] = sprites[i];
  }
}

dbyte_t Cpu::get_next_instruction()
{
  const dbyte_t opcode = memory[pc_register] << 8 | memory[pc_register + 1];
  return opcode;
}

void Cpu::increase_program_counter() { pc_register += 2; }

void Cpu::execute_instruction(const dbyte_t opcode)
{
  const dbyte_t addr = opcode & 0xFFF;
  const byte_t  x    = (opcode & 0x0F00) >> 8;
  const byte_t  y    = (opcode & 0x00F0) >> 4;

  switch (opcode & 0xF000)
  {
  case 0x0000:
    switch (opcode)
    {
    case 0x00E0:
      renderer->clear_display();
      break;

    case 0x00EE:
      pc_register = stack.top();
      stack.pop();
      break;
    }
    break;

  case 0x1000:
    pc_register = addr;
    break;

  case 0x2000:
    stack.push(pc_register);
    pc_register = addr;
    break;

  case 0x3000:
    if (v_registers[x] == (opcode & 0xFF))
    {
      increase_program_counter();
    }
    break;

  case 0x4000:
    if (v_registers[x] != (opcode & 0xFF))
    {
      increase_program_counter();
    }
    break;

  case 0x5000:
    if (v_registers[x] == v_registers[y])
    {
      increase_program_counter();
    }
    break;

  case 0x6000:
    v_registers[x] = (opcode & 0xFF);
    break;

  case 0x7000:
    v_registers[x] += (opcode & 0xFF);
    break;

  case 0x8000:
    switch (opcode & 0xF)
    {
    case 0x0:
      v_registers[x] = v_registers[y];
      break;

    case 0x1:
      v_registers[x] |= v_registers[y];
      break;

    case 0x2:
      v_registers[x] &= v_registers[y];
      break;

    case 0x3:
      v_registers[x] ^= v_registers[y];
      break;

    case 0x4:
    {
      dbyte_t sum = v_registers[x] + v_registers[y];

      v_registers[0xF] = 0;

      if (sum > 0xFF)
      {
        v_registers[0xF] = 1;
      }

      v_registers[x] = sum;
    }
    break;

    case 0x5:
    {
      dbyte_t sum = v_registers[x] - v_registers[y];

      v_registers[0xF] = 0;

      if (v_registers[y] > v_registers[x])
      {
        v_registers[0xF] = 1;
      }

      v_registers[x] = sum;
    }
    break;

    case 0x6:
      v_registers[0xF] = (v_registers[x] & 0x1);

      v_registers[x] >>= 1;
      break;

    case 0x7:
      v_registers[0xF] = 0;

      if (v_registers[y] > v_registers[x])
      {
        v_registers[0xF] = 1;
      }

      v_registers[x] = v_registers[y] - v_registers[x];
      break;

    case 0xE:
      v_registers[0xF] = (v_registers[x] & 0x80);
      v_registers[x] <<= 1;
      break;
    }

    break;

  case 0x9000:
    if (v_registers[x] != v_registers[y])
    {
      increase_program_counter();
    }
    break;

  case 0xA000:
    i_register = addr;
    break;

  case 0xB000:
    pc_register = (opcode & 0xFFF) + v_registers[0];
    break;

  case 0xC000:
  {
    const byte_t random_num = uniform_dist(random_engine);
    v_registers[x]          = random_num & (opcode & 0xFF);
  }
  break;

  case 0xD000:
  {
    byte_t width  = 8;
    byte_t height = opcode & 0xF;

    v_registers[0xF] = 0;

    for (uint32_t row = 0; row < height; ++row)
    {
      byte_t sprite = memory[i_register + row];

      for (uint32_t column = 0; column < width; ++column)
      { // If the bit (sprite) is not 0, render/erase the pixel
        if ((sprite & 0x80) > 0)
        {
          // If set_pixel returns true, which means a pixel was erased, set VF
          // to 1
          if (renderer->set_pixel(v_registers[x] + column,
                                  v_registers[y] + row))
          {
            v_registers[0xF] = 1;
          }
        }

        // Shift the sprite left 1. This will move the next next col/bit of the
        // sprite into the first position. Ex. 10010000 << 1 will become 0010000
        sprite <<= 1;
      }
    }
  }
  break;

  case 0xE000:
    switch (opcode & 0xFF)
    {
    case 0x9E:
      if (keyboard->is_key_pressed(v_registers[x]))
      {
        increase_program_counter();
      }
      break;

    case 0xA1:
      if (!keyboard->is_key_pressed(v_registers[x]))
      {
        increase_program_counter();
      }
      break;
    }

    break;

  case 0xF000:
    switch (opcode & 0xFF)
    {
    case 0x07:
      v_registers[x] = timer_delay_register;
      break;

    case 0x0A:
      paused = true;

      // TODO: Set callback to return to execution on keypress
      break;

    case 0x15:
      timer_delay_register = v_registers[x];
      break;

    case 0x18:
      sound_delay_register = v_registers[x];
      break;

    case 0x1E:
      i_register += v_registers[x];
      break;

    case 0x29:
      i_register = v_registers[x] * 5;
      break;

    case 0x33:
      // Get the hundreds digit and place it in I.
      memory[i_register] = v_registers[x] / 100;

      // Get tens digit and place it in I+1. Gets a value between 0 and 99,
      // then divides by 10 to give us a value between 0 and 9.
      memory[i_register + 1] = (v_registers[x] % 100) / 10;

      // Get the value of the ones (last) digit and place it in I+2.
      memory[i_register + 2] = v_registers[x] % 10;
      break;

    case 0x55:
      for (uint32_t i = 0; i <= x; ++i)
      {
        memory[i_register + i] = v_registers[i];
      }
      break;

    case 0x65:
      for (uint32_t i = 0; i <= x; ++i)
      {
        v_registers[i] = memory[i_register + i];
      }
      break;
    }

    break;

  default:
    throw std::runtime_error("Unknown opcode " + std::to_string(opcode));
  }
}

void Cpu::update_timers()
{
  if (timer_delay_register > 0)
  {
    --timer_delay_register;
  }

  if (sound_delay_register > 0)
  {
    --sound_delay_register;
  }
}

} // namespace Chip8
