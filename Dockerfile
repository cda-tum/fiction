FROM ubuntu:latest

# Optional argument to run the "make" command in parallel with the specified NUMBER_OF_JOBS
ARG NUMBER_OF_JOBS=4

# Configure apt and install packages
RUN apt-get -y update && \
    apt-get -y upgrade && \
    apt-get -y install make cmake gcc g++ mold git python3 python3-dev python3-pip python3-venv libreadline-dev xdg-utils

# Set up a working directory
WORKDIR /app

# Create a Python virtual environment and activate it
RUN python3 -m venv venv

# Use the virtual environment's pip to install z3-solver
RUN ./venv/bin/pip install z3-solver==4.10.0

# Set the path to include the virtual environment
ENV PATH="/app/venv/bin:$PATH"

# Clone fiction's repository including submodules
# RUN git clone --recursive https://github.com/cda-tum/fiction.git
COPY . fiction/

# Build fiction
RUN cmake -S fiction -B fiction/build \
    -DCMAKE_BUILD_TYPE=Release \
    -DFICTION_ENABLE_UNITY_BUILD=ON \
    -DFICTION_ENABLE_PCH=ON \
    -DFICTION_CLI=ON \
    -DFICTION_TEST=OFF \
    -DFICTION_EXPERIMENTS=OFF \
    -DFICTION_Z3=ON \
    -DFICTION_ENABLE_MUGEN=OFF \
    -DFICTION_PROGRESS_BARS=ON \
    -DFICTION_WARNINGS_AS_ERRORS=OFF \
    -DMOCKTURTLE_EXAMPLES=OFF \
    -DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=mold \
    && cmake --build fiction/build --config Release -j${NUMBER_OF_JOBS}


# Automatically start fiction when started in interactive mode
CMD ["./fiction/build/cli/fiction"]
