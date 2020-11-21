#include <cstdlib>
#include <iostream>

#include "simulator.hpp"

void print_usage(const char *program_name)
{
  std::cerr << "Usage: " << program_name << " [PROGRAM_FILEPATH]" << std::endl;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    print_usage(argv[0]);
    std::exit(EXIT_FAILURE);
  }

  Chip8::Simulator simulator;

  const auto program_filepath = argv[1];
  simulator.load_program(program_filepath);

  simulator.execute();

  simulator.terminate();

  return 0;
}
