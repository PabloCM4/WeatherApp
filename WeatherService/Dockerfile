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
    && rm -rf /var/lib/apt/lists/* # Removes the index of the packages to reduce the image size

# Set the working directory
WORKDIR /app

# Copy the Crow and WeatherService directories into the container
COPY Crow ./Crow
COPY WeatherService ./WeatherService

# Set the working directory to the WeatherService directory to run the commnands
WORKDIR /app/WeatherService

# Generate build files
RUN cmake -B build
# Build the application (executable)
RUN cmake --build build

# Expose the port the service will run on
EXPOSE 8080
# Run the executable when the container starts
CMD ["./build/weather_service"]