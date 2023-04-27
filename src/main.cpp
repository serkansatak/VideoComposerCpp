#include "VideoComposer.hpp"
#include "argparse.hpp"
#include <chrono>

namespace cr = std::chrono;

int main(int argc, char *argv[]) 
{
    argparse::ArgumentParser program("VideoComposer");


    #ifdef MULTIPLE_DIRS
        program.add_argument("--baseDir");
        program.add_argument("--outDir");

    #else
        program.add_argument("--imgDir")
        .help("path for image directory");
        program.add_argument("--out")
        .help("path for output mp4 file");
    #endif

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

    int fps = program.get<int>("--fps");
    #ifdef MULTIPLE_DIRS
        std::string baseDir = program.get<std::string>("--baseDir");
        std::string outDir = program.get<std::string>("--outDir");

        std::mutex queueMutex;
        PathVector subDirs; 
        FileUtils::findSubDirsWithPattern(baseDir, "Video_", subDirs);
        fs::create_directory(outDir);

        std::atomic<int> nextDirIndex = 0;
        auto workerThreadFunc = [&]() {
            while (true) {
                // Get the next frame index to process
                int dirIndex = nextDirIndex.fetch_add(1, std::memory_order_relaxed);
                // Check if we've processed all frames
                if (dirIndex >= subDirs.size()) {
                    break;
                }

                fs::path out = outDir;
                fs::path imgDir = subDirs[dirIndex];
                out.append(subDirs[dirIndex].filename().string() + ".mp4");
                imgDir.concat("/imgs/");

                std::lock_guard<std::mutex> lock(queueMutex);
                VideoComposer composer = VideoComposer(out, fps);
                bool success = composer.ComposeVideoFromImages(imgDir);

                if (success) { std::cout << "Success -- imgDir: " << imgDir << " -- out: " << out << std::endl;}
            }
        };


        std::vector<std::thread> workerThreads;
        int numThreads = std::thread::hardware_concurrency();
        for (int i = 0; i < numThreads; ++i) {
            workerThreads.emplace_back(workerThreadFunc);
        }
        // Wait for the worker threads to finish
        for (auto& thread : workerThreads) {
            thread.join();
        }
    #else
        auto imgDir = program.get<std::string>("--imgDir");
        auto out = program.get<std::string>("--out");

        VideoComposer composer = VideoComposer(out, fps);
        auto startingTime = cr::high_resolution_clock::now();
        bool success = composer.ComposeVideoFromImages(imgDir);
        auto endTime = cr::high_resolution_clock::now();
        std::cout << "Execution time: " << (double)cr::duration_cast<cr::milliseconds>\
            (endTime - startingTime).count() / 1000 << " seconds" << std::endl;
        std::cout << "Success : " << success << std::endl;
    #endif

    return 0;
}