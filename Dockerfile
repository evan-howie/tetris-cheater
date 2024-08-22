# Use an official GCC image as a base
FROM gcc:latest

# Install dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    mingw-w64 \
    wget \
    unzip \
    && apt-get clean

# Set up environment variables for cross-compilation
ENV CC=x86_64-w64-mingw32-gcc
ENV CXX=x86_64-w64-mingw32-g++
ENV WINDRES=x86_64-w64-mingw32-windres

# Set the working directory inside the container
WORKDIR /usr/src/app

# Download and extract SFML for Windows
RUN wget https://www.sfml-dev.org/files/SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit.zip \
    && unzip SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit.zip \
    && ls -l \
    && rm SFML-2.5.1-windows-gcc-7.3.0-mingw-64-bit.zip

# List the contents of the current directory to verify the folder name
RUN ls -l

# Move the SFML directory to the desired location
RUN mv SFML-2.5.1 /usr/local/sfml

# Set up CMake to find SFML
ENV SFML_ROOT=/usr/local/sfml
ENV CMAKE_PREFIX_PATH=$SFML_ROOT

# Copy the entire project into the container
COPY . .

# Create a build directory
RUN mkdir build

# Run CMake to configure the project for cross-compilation
RUN cd build && cmake -DCMAKE_TOOLCHAIN_FILE=../Toolchain-mingw64.cmake -DSFML_DIR=$SFML_ROOT/lib/cmake/SFML ..

# Build the project
RUN cd build && make

RUN x86_64-w64-mingw32-strip build/tetris.exe

RUN mkdir tetrisplusplus && \
    mkdir tetrisplusplus/fonts && \
    mkdir tetrisplusplus/bin && \
    cp build/tetris.exe ./tetrisplusplus/bin && \
    cp /usr/local/sfml/bin/* ./tetrisplusplus/bin && \
    cp fonts/* ./tetrisplusplus/fonts
