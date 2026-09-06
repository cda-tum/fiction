FROM ubuntu:26.04

# Optional argument to run the build in parallel with the specified NUMBER_OF_JOBS
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


# Configure apt and install the toolchain the wheel build needs
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    cmake ninja-build gcc g++ mold git python3 python3-dev python3-pip python3-venv libtbb-dev && \
    rm -rf /var/lib/apt/lists/*

# Set up a non-root user for security and create a working directory
RUN useradd -m appuser && mkdir -p /app && chown -R appuser:appuser /app
WORKDIR /app

# Switch to non-root user
USER appuser

# Create a Python virtual environment; the Z3 wheel provides the solver the build links against
RUN python3 -m venv venv && \
    . venv/bin/activate && \
    pip install --upgrade --no-cache-dir pip && \
    pip install --no-cache-dir z3-solver==4.14.1

# Add the virtual environment to the PATH
ENV PATH="/app/venv/bin:$PATH"

# Copy the local fiction repository to the container (preferred for development and CI); `.git` comes along
# because the wheel takes its version from the tags
COPY --chown=appuser:appuser . fiction/

# Build and install the `mnt.pyfiction` wheel, which carries the `fiction` command-line interface. pip builds
# in an isolated environment without the Z3 wheel, so `Z3_ROOT` points the build at the one installed above
ENV CMAKE_BUILD_PARALLEL_LEVEL=${NUMBER_OF_JOBS}
RUN Z3_ROOT="$(python3 -c 'import os, z3; print(os.path.dirname(z3.__file__))')" \
    pip install --no-cache-dir ./fiction


WORKDIR /app/fiction
# Automatically start fiction when started in interactive mode
CMD ["fiction"]
