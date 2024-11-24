if (APPLE)
    set(BASEPOINT @loader_path)
else ()
    set(BASEPOINT $ORIGIN)
endif ()
list(APPEND CMAKE_INSTALL_RPATH ${BASEPOINT} ${BASEPOINT}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

pybind11_add_module(pyfiction
        # Prefer thin LTO if available
        THIN_LTO pyfiction.cpp)
target_link_libraries(pyfiction PRIVATE libfiction)
target_include_directories(pyfiction
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_property(TARGET pyfiction PROPERTY POSITION_INDEPENDENT_CODE ON)

if (MSVC)
    target_compile_options(pyfiction PRIVATE /utf-8)
    add_definitions(-D_WIN32_WINNT=0x0A00)
    add_definitions(-DUNICODE -D_UNICODE)
endif ()

# Install directive for scikit-build-core
install(TARGETS pyfiction
        DESTINATION .
        COMPONENT fiction_Python)
