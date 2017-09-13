FROM debian:stable
RUN apt-get update 
RUN apt-get install -y libavcodec-dev libavformat-dev libavutil-dev 
RUN apt-get install -y libsdl2-dev
RUN apt-get install -y build-essential autoreconf automake

RUN mkdir -p /src
WORKDIR /src
COPY . /src/
RUN ./autogen.sh
RUN ./configure --prefix=/usr
RUN make
RUN make check
