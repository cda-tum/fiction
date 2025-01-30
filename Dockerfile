FROM ubuntu:24.04

# Optional argument to run the "make" command in parallel with the specified NUMBER_OF_JOBS
ARG NUMBER_OF_JOBS=4

# Unified metadata labels for DockerHub and the Open Container Initiative (OCI)
LABEL maintainer="Marcel Walter <marcel.walter@tum.de>" \
      org.opencontainers.image.title="fiction" \
      org.opencontainers.image.description="Docker image for fiction, an open-source design automation framework for Field-coupled Nanotechnologies." \
      org.opencontainers.image.authors="Marcel Walter <marcel.walter@tum.de>, Jan Drewniok <jan.drewniok@tum.de>, Simon Hofmann <simon.t.hofmann@tum.de>, Benjamin Hien <benjamin.hien@tum.de>, Willem Lambooy <willem.lambooy@tum.de>" \
      org.opencontainers.image.url="https://www.cda.cit.tum.de/research/nanotech/" \
      org.opencontainers.image.source="https://github.com/cda-tum/fiction" \
      org.opencontainers.image.documentation="https://fiction.readthedocs.io/" \
      org.opencontainers.image.licenses="MIT" \
      org.opencontainers.image.vendor="Chair for Design Automation, Technical University of Munich (TUM)"


# Configure apt and install required packages
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    make cmake gcc g++ ccache mold git python3 python3-dev python3-pip python3-venv libreadline-dev xdg-utils libtbb-dev && \
    rm -rf /var/lib/apt/lists/*

# Set up a non-root user for security and create a working directory
RUN useradd -m appuser && mkdir -p /app && chown -R appuser:appuser /app
WORKDIR /app

# Switch to non-root user
USER appuser

# Create a Python virtual environment and install necessary Python packages
RUN python3 -m venv venv && \
    . venv/bin/activate && \
    pip install --upgrade --no-cache-dir pip setuptools && \
    pip install --no-cache-dir z3-solver==4.13.0

# Add the virtual environment to the PATH
ENV PATH="/app/venv/bin:$PATH"

# Clone fiction's repository including submodules (in case a local copy is not available)
# RUN git clone --recursive https://github.com/cda-tum/fiction.git

# Copy the local fiction repository to the container (preferred for development and CI)
COPY --chown=appuser:appuser . fiction/

# Build fiction
RUN . venv/bin/activate \
    && cmake -S fiction -B fiction/build \
      -DCMAKE_BUILD_TYPE=Release \
      -DFICTION_ENABLE_UNITY_BUILD=ON \
      -DFICTION_ENABLE_PCH=ON \
      -DFICTION_CLI=ON \
      -DFICTION_TEST=OFF \
      -DFICTION_EXPERIMENTS=OFF \
      -DFICTION_Z3=ON \
      -DFICTION_ALGLIB=ON \
      -DFICTION_ENABLE_MUGEN=OFF \
      -DFICTION_PROGRESS_BARS=ON \
      -DFICTION_WARNINGS_AS_ERRORS=OFF \
      -DMOCKTURTLE_EXAMPLES=OFF \
      -DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=mold \
    && cmake --build fiction/build --config Release -j${NUMBER_OF_JOBS}


WORKDIR /app/fiction
# Automatically start fiction when started in interactive mode
CMD ["build/cli/fiction"]
