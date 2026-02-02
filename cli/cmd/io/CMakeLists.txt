target_sources(
  fiction
  PRIVATE src/blif.cpp
          src/fgl.cpp
          src/fqca.cpp
          src/qca.cpp
          src/qcc.cpp
          src/qll.cpp
          src/read.cpp
          src/sqd.cpp
          src/tt.cpp
          src/verilog.cpp)

target_sources(
  fiction
  PRIVATE FILE_SET
          HEADERS
          BASE_DIRS
          ${PROJECT_SOURCE_DIR}/cli
          FILES
          cmd_io.hpp
          include/blif.hpp
          include/fgl.hpp
          include/fqca.hpp
          include/qca.hpp
          include/qcc.hpp
          include/qll.hpp
          include/read.hpp
          include/sqd.hpp
          include/tt.hpp
          include/verilog.hpp)
