# Use ubuntu 24.04 (same I used locally to build the service)
FROM ubuntu:24.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    curl \
    libcurl4-openssl-dev \
    libasio-dev \
    libssl-dev \
    zip \
    unzip \
    git \
    && rm -rf /var/lib/apt/lists/* # Removes the index of the packages to reduce the image size

# Set the working directory
WORKDIR /app

# Copy the Crow, vcpkg and FavouritesService directories into the container
COPY Crow ./Crow
COPY vcpkg ./vcpkg
COPY FavouritesService ./FavouritesService

# Bootstrap vcpkg
RUN /app/vcpkg/bootstrap-vcpkg.sh

# Install MongoDB driver
RUN /app/vcpkg/vcpkg install mongo-cxx-driver

# Set the working directory to the FavouritesService directory to run the commnands
WORKDIR /app/FavouritesService

# Generate build files
RUN cmake -B build -DCMAKE_TOOLCHAIN_FILE=/app/vcpkg/scripts/buildsystems/vcpkg.cmake

# Build the application (executable)
RUN cmake --build build

# Expose the port the service will run on
EXPOSE 8081

# Run the executable when the container starts
CMD ["./build/favourites_service"]