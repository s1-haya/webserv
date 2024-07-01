FROM ubuntu:20.04

# 非対話的モードを設定
ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /webserv

RUN apt-get update && \
    apt-get install -y \
    build-essential --no-install-recommends

COPY srcs /webserv/srcs
COPY html /webserv/html
COPY Makefile /webserv/Makefile
COPY docker /webserv/docker/

RUN rm -rf /var/lib/apt/lists/*

CMD ["make", "run"]
