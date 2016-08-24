# Event plugin API

## Interface

### getAxis

    int getAxis();

get called each frame. return a value between -127 & 127

### getQuit

    int getQuit();

get called each frame. `1` if the input requests the program to exit  `0` otherwise

### update

    void update();

get called before `getAxis()` and `getQuit()`. Allow the module to update itself synchronized with the display.


## Internals

Works with `ltdl`. Useful resources are hard to find :

- Minimal example in [autobook](https://sourceware.org/autobook/autobook/autobook_98.html)
- Automake / Autoconf setup in [libtool manual](https://www.gnu.org/software/libtool/manual/html_node/Distributing-libltdl.html#Distributing-libltdl)
- dlopen in c++ [HOWTO](http://www.tldp.org/HOWTO/pdf/C++-dlopen.pdf) pages 6 and more.
