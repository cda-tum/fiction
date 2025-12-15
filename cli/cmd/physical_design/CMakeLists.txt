target_sources(fiction PRIVATE
    src/exact.cpp
    src/gold.cpp
    src/hex.cpp
    src/onepass.cpp
    src/optimize.cpp
    src/ortho.cpp
)

target_sources(fiction
    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS ${PROJECT_SOURCE_DIR}/cli
    FILES
    cmd_physical_design.hpp
    include/exact.hpp
    include/gold.hpp
    include/hex.hpp
    include/onepass.hpp
    include/optimize.hpp
    include/ortho.hpp
)
