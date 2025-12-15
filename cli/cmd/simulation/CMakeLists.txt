target_sources(fiction PRIVATE
    src/clustercomplete.cpp
    src/opdom.cpp
    src/quickexact.cpp
    src/quicksim.cpp
    src/temp.cpp
)

target_sources(fiction
    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS ${PROJECT_SOURCE_DIR}/cli
    FILES
    cmd_simulation.hpp
    include/clustercomplete.hpp
    include/opdom.hpp
    include/quickexact.hpp
    include/quicksim.hpp
    include/temp.hpp
)
