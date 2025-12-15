target_sources(fiction PRIVATE
    src/check.cpp
    src/equiv.cpp
)

target_sources(fiction
    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS ${PROJECT_SOURCE_DIR}/cli
    FILES
    cmd_verification.hpp
    include/check.hpp
    include/equiv.hpp
)
