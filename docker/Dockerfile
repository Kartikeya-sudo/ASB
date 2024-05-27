# Base image for the container
FROM ubuntu:18.04

# Updates the container and install curl
RUN apt update && apt-get install -y curl gcc make file
# Gets the required library libressl from the source and builds it
RUN curl -Lo libressl.tar.gz https://ftp.openbsd.org/pub/OpenBSD/LibreSSL/libressl-3.9.2.tar.gz &&\ 
    tar -xzvf libressl.tar.gz &&\ 
    cd libressl-3.9.2 &&\ 
    ./configure --disable-dependency-tracking && make && make install
# Adds the library to the path for compilation
ENV LD_LIBRARY_PATH $LD_LIBRARY_PATH:/usr/local/lib/
RUN cd && apt-get install -y git && git clone https://github.com/Kartikeya-sudo/ASB && cd ASB && make
