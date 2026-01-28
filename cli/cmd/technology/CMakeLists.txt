target_sources(fiction PRIVATE src/area.cpp src/cell.cpp src/energy.cpp)

target_sources(
  fiction
  PRIVATE FILE_SET
          HEADERS
          BASE_DIRS
          ${PROJECT_SOURCE_DIR}/cli
          FILES
          cmd_technology.hpp
          include/area.hpp
          include/cell.hpp
          include/energy.hpp)
