# Stingray

Stingray is a video reader with fluid customizable play speed. Designed for user-controlled display experience.


Read doc and install instructions at https://holusion.github.io/stingray/



# Tests and benchmark

gtest is vendored in `vendor/gtest`. Rebuild before testing using the recommended [cmake process](https://github.com/google/googletest/blob/master/googletest/README.md).

test using `make check`

A Benchmark tool is also produced on `make`. It can be used like :

    benchmark target_video.mp4

With different encodings or to test code performance improvement.
