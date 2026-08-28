FROM ubuntu:24.04 AS builder

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libcurl4-openssl-dev \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY CMakeLists.txt .
COPY src/ ./src/

RUN mkdir build && cd build && cmake .. && make

FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    libcurl4-openssl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/build/search_server .

EXPOSE 8080

CMD ["./search_server"]