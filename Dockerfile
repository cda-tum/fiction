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
        git g++ cmake boost-dev python readline-dev

# Clone the repository with the submodules
RUN git clone --recursive https://github.com/marcelwa/fiction.git

# Create fiction directory in root
WORKDIR /fiction

# Build fiction
RUN mkdir build \
    && cd build \
    && cmake .. \
    && make -j${NUMBER_OF_JOBS}

# Automatically start fiction when started in interactive mode
CMD ["./build/fiction"]