# Stingray

Stingray is a video reader with fluid customizable play speed. Designed for user-controlled display experience.


Read doc and install instructions at https://holusion.github.io/stingray/

Example configuration :

    CPPFLAGS=-DDEBUG ./configure --enable-seamless --enable-dbus --enable-capacity=100 --enable-crossfade --disable-modules

# Tests and benchmark

gtest is vendored in `vendor/gtest`. Rebuild before testing using the recommended [cmake process](https://github.com/google/googletest/blob/master/googletest/README.md).

test using `make check`

A Benchmark tool is also produced on `make`. It can be used like :

    benchmark target_video.mp4

With different encodings or to test code performance improvement.

### Debug

Compiling with debug symbols is done like :

    make clean
    ./configure CPPFLAGS=-DDEBUG
    make

# Hardware acceleration

## Preamble

Usefull doc :
- http://developer.download.nvidia.com/compute/redist/ffmpeg/1511-patch/FFMPEG-with-NVIDIA-Acceleration-on-Ubuntu_UG_v01.pdf
- https://trac.ffmpeg.org/wiki/HWAccelIntro
- https://gist.github.com/Brainiarc7/988473b79fd5c8f0db54b92ebb47387a

use `nvidia-smi` (package nvidia-smi) to get GPU usage "like top".

    nvidia-smi dmon -i 0

Debian package `nvidia-cuda-toolkit` will do for all CUDA install requirements.


Usefull packages :

    apt-get install yasm nvidia-cuda-toolkit

Essentially, only CUVID could provide the kind of function we need. However it's not nearly mature enough to provide a stable base.

### ffmpeg-static

too much trouble with system-version. We need to static-compile FFMPEG to be able to use latest hwaccel

Source : https://fritzone.wordpress.com/2010/05/11/link-with-static-ffmpeg/
Generic Compile instructions : https://trac.ffmpeg.org/wiki/CompilationGuide/Ubuntu
Could be made "more static" by using work on fully-static binary : https://github.com/zimbatm/ffmpeg-static/blob/master/build.sh

final configure : 
    ./configure --disable-ffplay --extra-libs="-lpthread -lm" --pkg-config-flags="--static" --enable-static --enable-gpl --enable-vaapi --enable-libx264