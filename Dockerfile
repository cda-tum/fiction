FROM alpine:latest

# Optional argument to run the "make" command in parallel with the specified NUMBER_OF_JOBS
ARG NUMBER_OF_JOBS=1

# Configure apt and install packages
RUN apk add --no-cache \
    # Install cmake with its dependencies
    build-base gcc abuild binutils binutils-doc gcc-doc \
    cmake cmake-doc extra-cmake-modules extra-cmake-modules-doc \
    #
    # Install packages needed to build fiction
    git g++ cmake python3 python3-dev readline-dev zlib-dev xdg-utils

# Setup Z3
RUN git clone --depth 1 --branch z3-4.10.0 https://github.com/Z3Prover/z3.git

# Build Z3
RUN cmake -S z3 -B z3/build -DCMAKE_INSTALL_PREFIX=z3lib -DZ3_BUILD_LIBZ3_SHARED=OFF -DZ3_BUILD_TEST_EXECUTABLES=OFF -DZ3_ENABLE_EXAMPLE_TARGETS=OFF \
    && cmake --build z3/build --config Release -j${NUMBER_OF_JOBS} --target install

# Clone fiction's repository including submodules
RUN git clone --recursive https://github.com/marcelwa/fiction.git

# Build fiction
RUN cmake -S fiction -B fiction/build -DCMAKE_BUILD_TYPE=Release -DFICTION_CLI=ON -DFICTION_TEST=OFF -DFICTION_EXPERIMENTS=OFF -DFICTION_Z3=ON -DFICTION_Z3_SEARCH_PATHS=z3lib -DFICTION_ENABLE_MUGEN=OFF -DFICTION_PROGRESS_BARS=ON -DMOCKTURTLE_EXAMPLES=OFF -DWARNINGS_AS_ERRORS=OFF \
    && cmake --build fiction/build --config Release -j${NUMBER_OF_JOBS}

# Automatically start fiction when started in interactive mode
CMD ["./fiction/build/cli/fiction"]
