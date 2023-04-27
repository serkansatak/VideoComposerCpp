#include <iostream>
#include <filesystem>
#include <string>
#include <algorithm>

namespace fs = std::filesystem;

using PathVector = std::vector<fs::path>;
using StrVector = std::vector<std::string>;

class FileUtils {
public:
    // Find all files in a directory with a given extension list, case-insensitively, results will be sorted.
    static void findFilesWithExtensions(const std::string& directory_path, const StrVector& extensions, PathVector& results) {
        for (const auto& entry : fs::directory_iterator(directory_path)) {
            if (fs::is_regular_file(entry.path())) {
                // Check if the file has a matching extension
                const std::string extension = entry.path().extension().string();
                if (std::find_if(extensions.begin(), extensions.end(),
                    [&](const std::string& ext) { return ends_with(extension, ext); }) != extensions.end()) {
                    results.push_back(entry.path());
                }
            }
        }
        std::sort(results.begin(), results.end());
    }

    static void findSubDirsWithPattern(const std::string& base_directory, const std::string& pattern, PathVector& results) {
        for (const auto& entry : fs::directory_iterator(base_directory)) {
            if (fs::is_directory(entry.path())) {
                // Check if the directories match the pattern
                if (entry.path().string().find(pattern) != std::string::npos) {
                    results.push_back(entry.path());
                }
            }
        }
        std::sort(results.begin(), results.end());
    }

private:
    // Helper function to check if a string ends with a given suffix, case-insensitively
    static bool ends_with(const std::string& str, const std::string& suffix) {
        if (str.length() < suffix.length()) return false;
        return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin(),
            [](char a, char b) { return std::tolower(a) == std::tolower(b); });
    }
};
