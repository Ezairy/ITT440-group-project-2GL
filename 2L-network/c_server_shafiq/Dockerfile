FROM ubuntu:22.04

# Mengelakkan prompt interaktif semasa pemasangan pakej
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    gcc \
    make \
    libmysqlclient-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY Makefile server.c ./

RUN make

EXPOSE 5002

CMD ["./server"]