#include "VideoComposer.hpp"
#include "argparse.hpp"
#include <chrono>

namespace cr = std::chrono;

int main(int argc, char *argv[]) 
{
    argparse::ArgumentParser program("VideoComposer");

    program.add_argument("--imgDir")
    .help("path for image directory");

    program.add_argument("--out")
    .help("path for output mp4 file");

    program.add_argument("--fps")
    .help("FPS of output video")
    .default_value(25);

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    auto imgDir = program.get<std::string>("--imgDir");
    auto out = program.get<std::string>("--out");
    int fps = program.get<int>("--fps");

    VideoComposer composer = VideoComposer(out, fps);

    auto startingTime = cr::high_resolution_clock::now();
    bool success = composer.ComposeVideoFromImages(imgDir);
    auto endTime = cr::high_resolution_clock::now();
    std::cout << "Execution time: " << (double)cr::duration_cast<cr::milliseconds>\
        (endTime - startingTime).count() / 1000 << " seconds" << std::endl;
    std::cout << "Success : " << success << std::endl;

    return 0;
}