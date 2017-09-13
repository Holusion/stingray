FROM debian:stable
RUN apt-get update 
RUN apt-get install -y --force-yes libavcodec-dev libavformat-dev libavutil-dev 
RUN apt-get install -y --force-yes libsdl2-dev
RUN apt-get install build-essential

RUN mkdir -p /src
WORKDIR /src
COPY . /src/
RUN ./configure --prefix=/usr
RUN make
RUN make check
