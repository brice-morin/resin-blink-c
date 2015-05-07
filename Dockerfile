FROM resin/rpi-raspbian:latest

# Install GCC and stuff.
RUN apt-get update && apt-get install -y build-essential && apt-get clean && rm -rf /var/lib/apt/lists/*

COPY . /app
WORKDIR /app

# Compile app
RUN gcc -o blink blink.c

# Start blink app
CMD ["./blink"]
