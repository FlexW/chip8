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

add_executable(chip8 ${SOURCE_LIST} ${HEADER_LIST})

target_link_libraries(
  chip8
  PRIVATE
  chip8_lib
  )
