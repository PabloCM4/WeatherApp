# Use ubuntu 24.04 (same I used locally to build the service)
FROM ubuntu:24.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    git \
    libsasl2-dev \
    libmongocxx-dev \
    libbsoncxx-dev
    && rm -rf /var/lib/apt/lists/*

# Copy the Crow and FavouritesService directories into the container
COPY Crow ./Crow
COPY FavouritesService ./FavouritesService

# Set the working directory to the FavouritesService directory to run the commnands
WORKDIR /app/FavouritesService

# Generate build files
RUN cmake -B build
# Build the application (executable)
RUN cmake --build build

# Expose the port the service will run on
EXPOSE 8081
# Run the executable when the container starts
CMD ["./build/favourites_service"]