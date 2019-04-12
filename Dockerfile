FROM debian:stable
RUN apt-get update
RUN apt-get install -y libsystemd-dev systemd

RUN apt-get -y install build-essential autoconf automake cmake libtool git \
checkinstall nasm yasm libass-dev libfreetype6-dev libsdl2-dev autotools-dev \
libva-dev libvdpau-dev libvorbis-dev libxcb1-dev libxcb-shm0-dev \
libxcb-xfixes0-dev pkg-config texinfo wget zlib1g-dev libchromaprint-dev \
frei0r-plugins-dev gnutls-dev ladspa-sdk libcaca-dev libcdio-paranoia-dev \
libcodec2-dev libfontconfig1-dev libfreetype6-dev libfribidi-dev libgme-dev \
libgsm1-dev libjack-dev libmodplug-dev libmp3lame-dev libopencore-amrnb-dev \
libopencore-amrwb-dev libopenjp2-7-dev libopenmpt-dev libopus-dev \
libpulse-dev librsvg2-dev librubberband-dev librtmp-dev libshine-dev \
libsmbclient-dev libsnappy-dev libsoxr-dev libspeex-dev libssh-dev \
libtheora-dev libtwolame-dev libv4l-dev libvo-amrwbenc-dev \
libvorbis-dev libvpx-dev libwavpack-dev libwebp-dev libx264-dev libx265-dev \
libxvidcore-dev libxml2-dev libzmq3-dev libzvbi-dev liblilv-dev \
libopenal-dev opencl-dev

RUN mkdir -p /src
WORKDIR /src



COPY . /src/
RUN cd ffmpeg && ./configure --enable-gpl --enable-version3 --enable-static \
--disable-shared --enable-small --enable-avisynth --enable-chromaprint \
--enable-frei0r --enable-gmp --enable-gnutls --enable-ladspa \
--enable-libass --enable-libcaca --enable-libcdio \
--enable-libcodec2 --enable-libfontconfig --enable-libfreetype \
--enable-libfribidi --enable-libgme --enable-libgsm --enable-libjack \
--enable-libmodplug --enable-libmp3lame --enable-libopencore-amrnb \
--enable-libopencore-amrwb --enable-libopencore-amrwb \
--enable-libopenjpeg --enable-libopenmpt --enable-libopus --enable-libpulse \
--enable-librsvg --enable-librubberband --enable-librtmp --enable-libshine \
--enable-libsmbclient --enable-libsnappy --enable-libsoxr --enable-libspeex \
--enable-libssh --enable-libtheora \
--enable-libtwolame --enable-libv4l2 --enable-libvo-amrwbenc \
--enable-libvorbis --enable-libvpx --enable-libwavpack --enable-libwebp \
--enable-libx264 --enable-libx265 --enable-libxvid --enable-libxml2 \
--enable-libzmq --enable-libzvbi --enable-lv2 \
--enable-openal --enable-opencl --enable-opengl --enable-libdrm && make -j4

RUN ./autogen.sh
RUN ./configure --prefix=/usr
RUN make -j4
RUN make check
