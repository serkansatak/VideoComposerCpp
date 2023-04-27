// Includes
#include "opencv2/opencv.hpp"
#include "Utils.hpp"
#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <atomic>

// Define Module
#ifndef VIDEO_COMPOSER_H
#define VIDEO_COMPOSER_H


StrVector DEFAULT_IMAGE_EXTENSIONS = {".JPEG", ".JPG", ".PNG"};

struct FrameData {
    int index;
    cv::Mat frame;
};

class VideoComposer {

    public:
        /*
            Video Composer Constructer. 

            PARAMETERS:

            out : filepath for output mp4 file.
            fps : fps of the video that is going to be composed.
        */
        VideoComposer(const std::string& out, int& fps) 
        : out(out), fps(fps)
        {    
            this->codec = cv::VideoWriter::fourcc('m', 'p', '4', 'v');            
        };


        /*
            Composes video from images.
            Finds all files with certain extensions in image directory,
            sorts them and composes video.

            PARAMETERS:

            imgDir : filepath for the directory which images located.
            extension: a vector of extensions.

            RETURNS:

            bool : depending on success.

        */
        bool ComposeVideoFromImages(const std::string& imgDir, const StrVector extensions = DEFAULT_IMAGE_EXTENSIONS) 
        {
            this->imgDir = imgDir;
            FileUtils::findFilesWithExtensions(this->imgDir, extensions, this->fileList);
            this->writer = new cv::VideoWriter();
            bool result;

            #ifdef THREADED_PROCESS
                result = this->ComposeVideoFromImagesMulti();
            #else
                result = this->ComposeVideoFromImagesSingle();
            #endif

            this->writer->release();
            delete this->writer;

            #ifdef TEST_VIDEO
                if (result) 
                {
                    cv::VideoCapture cap = cv::VideoCapture(this->out);
                    while (cap.isOpened()) 
                    {
                        if (int(cap.get(cv::CAP_PROP_FRAME_COUNT)) != int(this->fileList.size())) 
                        {
                            result = 0;  
                        } 
                        break;
                    }
                }
            #endif                      
            return result;
        };

        virtual void ProcessImage(cv::Mat img){}


    protected:
        std::string imgDir;
        std::string out;
        int fps;
        cv::VideoWriter* writer;
        int codec;
        PathVector fileList;

        // Same as in OpenCV examples
        bool ComposeVideoFromImagesSingle()
        {
            if (this->fileList.size() > 0) {
                for (auto& imPath: this->fileList) 
                {
                    // Read image
                    cv::Mat img = cv::imread(std::string(imPath), cv::IMREAD_COLOR);
                    if (!this->writer->isOpened()) {
                        this->writer->open(this->out, this->codec, this->fps, img.size(), true);
                    };
                    // Process image
                    this->ProcessImage(img);
                    // Write frame
                    this->writer->write(img);
                }
                return 1;
            }else {return 0;};      

        };


        bool ComposeVideoFromImagesMulti() 
        {
            // Define priority queue instead of normal queue to be sure queue is in order
            std::priority_queue<FrameData, std::vector<FrameData>, std::function<bool(const FrameData&, const FrameData&)>> frames_{
                [](const FrameData& f1, const FrameData& f2) { return f1.index > f2.index; } };
            std::mutex queueMutex;
            std::atomic<int> nextFrameIndex(0);
            // Define a worker thread function to process frames
            auto workerThreadFunc = [&]() {
                while (true) {
                    // Get the next frame index to process
                    int frameIndex = nextFrameIndex.fetch_add(1, std::memory_order_relaxed);
                    // Check if we've processed all frames
                    if (frameIndex >= this->fileList.size()) {
                        break;
                    }
                    // Load the image from disk
                    cv::Mat image = cv::imread(this->fileList[frameIndex], cv::IMREAD_COLOR);
                    if (!this->writer->isOpened()) {
                        this->writer->open(this->out, this->codec, this->fps, image.size(), true);
                    };
                    // Check if we were able to load the image
                    if (image.empty()) {
                        std::cerr << "Failed to load image: " << this->fileList[frameIndex] << std::endl;
                        continue;
                    }
                    this->ProcessImage(image);
                    // Add the frame data to the queue
                    std::lock_guard<std::mutex> lock(queueMutex);
                    frames_.push({frameIndex, image});
                }
            };
            // Create worker threads to process the frames
            std::vector<std::thread> workerThreads;
            int numThreads = std::thread::hardware_concurrency();
            for (int i = 0; i < numThreads; ++i) {
                workerThreads.emplace_back(workerThreadFunc);
            }
            // Wait for the worker threads to finish
            for (auto& thread : workerThreads) {
                thread.join();
            }

            // Process the frames in the correct order
            int numFrames = this->fileList.size();
            for (int i = 0; i < numFrames; ++i) {
                // Get the next frame from the queue
                std::unique_lock<std::mutex> lock(queueMutex);
                while (frames_.empty()) {
                    lock.unlock();
                    std::this_thread::sleep_for(std::chrono::microseconds(50));
                    lock.lock();
                }
                auto frameData = frames_.top();
                frames_.pop();
                // Check that we have the correct frame index
                if (frameData.index != i) {
                    std::cerr << "Out-of-order frame: expected " << i << ", got " << frameData.index << std::endl;
                    continue;
                }
                // Write the frame to the output video
                this->writer->write(frameData.frame);
            }
            return true;
        
        };
};

#endif
