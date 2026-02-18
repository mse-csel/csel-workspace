# https://github.com/devcontainers/images/tree/main/src/cpp
ARG VARIANT=ubuntu-24.04
FROM mcr.microsoft.com/devcontainers/base:${VARIANT}

EXPOSE 69/udp

RUN apt-get update && \
    export DEBIAN_FRONTEND=noninteractive && \
    apt-get -y install --no-install-recommends \
    dnsmasq \
    tcpdump
