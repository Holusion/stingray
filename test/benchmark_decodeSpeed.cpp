#include "../config.h"
#include  "constants.h"
#include  "debug.h"


#include "video.hpp"

#include  "exceptions/global_exception.hpp"
#include  "exceptions/av_exception.hpp"
#include  <thread>
#include  <chrono>

/*
 * USAGE : benchmark /path/to/video.mp4
 * Will give time taken to decode a chunk of frames.
 * It tests only reverse decode as there is no way forward decode is more problematic...
 */
void cycle(decoder::VideoDecoder* decoder,entities::Video* video ){
  int i,c;
  std::chrono::high_resolution_clock::time_point p(std::chrono::high_resolution_clock::now());

  for (c=0;c<10;c++){
    decoder->decodeAndWrite(*video->buffer);
    for(i=0;i<DECODE_SIZE;i++){
      //We don't check buffer has enough elements : it should...
      video->buffer->forward();
    }
  }
  using dura = std::chrono::duration<double>;
        auto d = std::chrono::high_resolution_clock::now() - p;
        std::cout << "Decode cycle: "
            << std::chrono::duration_cast<dura>(d/10).count()
            <<" s"
            << std::endl;
}

int  main (int argc, char** argv) {

  entities::Video        video(argv[1],1920,1080);
  decoder::VideoDecoder decoder(video.context);

  if (argc < 2){
    return EXIT_FAILURE;
  }

  video.buffer->swap();
  try {
    while(true)
      cycle(&decoder,&video);
  }catch (AVException& e){
    std::cerr <<"AV Exception : "<< e.what() <<std::endl;
    return EXIT_FAILURE;
  }catch (GlobalException& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
