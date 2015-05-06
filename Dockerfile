FROM resin/rpi-raspbian:latest

# Install GCC and stuff.
RUN apt-get update && apt-get install -y build-essential && apt-get clean && rm -rf /var/lib/apt/lists/*

COPY . /app

# Compile app
RUN gcc -o /app/blink /app/blink.c

# Start blink app
CMD ["sudo /app/blink"]
