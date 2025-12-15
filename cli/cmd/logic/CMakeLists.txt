target_sources(fiction PRIVATE
    src/abc.cpp
    src/akers.cpp
    src/balance.cpp
    src/fanouts.cpp
    src/gates.cpp
    src/map.cpp
    src/miginvopt.cpp
    src/miginvprop.cpp
    src/random.cpp
    src/simulate.cpp
)

target_sources(fiction
    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS ${PROJECT_SOURCE_DIR}/cli
    FILES
    cmd_logic.hpp
    include/abc.hpp
    include/akers.hpp
    include/balance.hpp
    include/fanouts.hpp
    include/gates.hpp
    include/map.hpp
    include/miginvopt.hpp
    include/miginvprop.hpp
    include/random.hpp
    include/simulate.hpp
)
