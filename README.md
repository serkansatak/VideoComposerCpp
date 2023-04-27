# Video Composer OpenCV/C++

I built this repo while composing video from datasets for my thesis.

## Table of Contents

- [About](#about)
- [Getting Started](#getting_started)
- [Usage](#usage)

## About <a name = "about"></a>

A helper class for composing video on OpenCV/C++. Code is header only so you don't need any installation except OpenCV.

VideoComposer takes images from a folder which has spesific file extensions, sorts them alphabetically and composes a video.

[Argparse](https://github.com/p-ranav/argparse) which I use for command line interface is also a helpful library and header only.

### CMake Flags

1. ```THREADED_PROCESS```

    Currently it is set to read frames threaded but you can also use it for ```VideoCompose::ProcessImage``` or concurently processing multiple folders. In threaded option queue is automatically controlled if it is in order.
2. ```TEST_VIDEO```

    Just tests if the output video has same frame count as your folder. Might increase time.


## Getting Started <a name = "getting_started"></a>

You can use it however you like. There are 2 prerequisites

### Prerequisites

 - C++17
 - OpenCV 4.x

### Usage Example

If you want to use it as it is, follow these steps.

```bash
git clone --recurse-submodules https://github.com/serkansatak/VideoComposerCpp
cd VideoComposerCpp

# make sure you have OpenCV installation and configure the Makefile Includes and Libs for your machine.

make all
./bin/VideoComposer --imgDir ${YOUR_IMAGE_DIR} --out ${YOUR_OUTPUT_PATH} --fps ${DESIRED_FPS}
```

