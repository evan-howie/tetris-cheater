#!/bin/bash

if [ -d "build" ]; then
  echo "build folder already exists. Removing..."
  rm -rf build
  mkdir build
fi

# Build the Docker image
echo "Building Docker image..."
docker build -t tetris-build-env .

# Check if the image was built successfully
if [ $? -ne 0 ]; then
  echo "Docker image build failed."
  exit 1
fi

# Run the Docker container
echo "Running the Docker container..."
docker create --name dummy tetris-build-env
docker cp dummy:/usr/src/app/tetrisplusplus/ build
docker rm -f dummy

# Check if the container ran successfully
if [ $? -ne 0 ]; then
  echo "Docker container run failed."
  exit 1
fi

# Optionally remove the Docker image to save space
echo "Removing Docker image..."
docker rmi tetris-build-env

# Check if the image was removed successfully
if [ $? -ne 0 ]; then
  echo "Failed to remove Docker image."
  exit 1
fi

echo "Build and cleanup completed successfully."

