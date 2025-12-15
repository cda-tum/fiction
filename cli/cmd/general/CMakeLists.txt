target_sources(fiction PRIVATE
    src/clear.cpp
    src/version.cpp
)

target_sources(fiction
    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS ${PROJECT_SOURCE_DIR}/cli
    FILES
    cmd_general.hpp
    include/clear.hpp
    include/version.hpp
)
