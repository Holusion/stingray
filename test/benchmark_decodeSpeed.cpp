#include <unistd.h>
#include  <thread>
#include  <chrono>

#include "../config.h"
#include  "debug.h"
#include  "video_decoder.hpp"
#include  "video_frame.hpp"
#include  "exceptions/global_exception.hpp"
#include  "exceptions/av_exception.hpp"

#define CYCLE_LENGTH 1000

/*
 * USAGE : benchmark /path/to/video.mp4
 * Will give time taken to decode a chunk of frames.
 * It tests only reverse decode as there is no way forward decode is more problematic...
 */
void cycle(entities::Video* video ){
  int i,c;
  std::chrono::high_resolution_clock::time_point p(std::chrono::high_resolution_clock::now());

  for (c=0;c<CYCLE_LENGTH;c++){
    while(video->buffer.size() == 0)
      usleep(1);
    video->buffer.forward();
  }
  using dura = std::chrono::duration<double>;
        auto d = std::chrono::high_resolution_clock::now() - p;
        std::cout << "Decode cycle ("<<CYCLE_LENGTH<<" frames): "
            << std::chrono::duration_cast<dura>(d).count()
            <<" s"
            << std::endl;
}

int  main (int argc, char** argv) {

  if (argc < 2){
    return EXIT_FAILURE;
  }
  entities::Video        video(argv[1],1920,1080);
  video.buffer.swap(); //Reverse-decode is usually slower and what we're interested in
  try {
    while(true)
      cycle(&video);
  }catch (AVException& e){
    std::cerr <<"AV Exception : "<< e.what() <<std::endl;
    return EXIT_FAILURE;
  }catch (GlobalException& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
