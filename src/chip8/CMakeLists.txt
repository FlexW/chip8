file(
  GLOB_RECURSE
  HEADER_LIST
  CONFIGURE_DEPENDS
  "*.hpp"
  )

file(
  GLOB_RECURSE
  SOURCE_LIST
  CONFIGURE_DEPENDS
  "*.cpp"
  )

add_library(chip8_lib ${SOURCE_LIST} ${HEADER_LIST})

target_link_libraries(chip8_lib PUBLIC glfw glad)

target_include_directories(chip8_lib PUBLIC .)

target_compile_features(chip8_lib PUBLIC cxx_std_17)

target_compile_options(
  chip8_lib
  PRIVATE
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror>
  )
