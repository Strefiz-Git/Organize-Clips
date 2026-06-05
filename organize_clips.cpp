#include <filesystem>
#include <regex>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

namespace fs = std::filesystem;

struct ClipDate {
    int year;
    int month;
    int day;
};

bool parseDate(const std::string& filename, ClipDate& date) {
    std::regex dateRegex(R"((\d{4})\.(\d{2})\.(\d{2}))");
    std::smatch match;
    if (std::regex_search(filename, match, dateRegex)) {
        date.year = std::stoi(match[1]);
        date.month = std::stoi(match[2]);
        date.day = std::stoi(match[3]);
        return true;
    }
    return false;
}

int main() {
    fs::path currentDir = fs::current_path();
    std::vector<fs::path> videoFiles;

    for (const auto& entry : fs::directory_iterator(currentDir)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (ext == ".mp4" || ext == ".avi" || ext == ".mov" || ext == ".mkv" || ext == ".webm" || ext == ".flv" || ext == ".wmv") {
                videoFiles.push_back(entry.path());
            }
        }
    }

    int moved = 0;

    for (const auto& filePath : videoFiles) {
        std::string filename = filePath.filename().string();
        ClipDate date;

        if (!parseDate(filename, date)) {
            continue;
        }

        char monthFolder[32];
        snprintf(monthFolder, sizeof(monthFolder), "%04d-%02d", date.year, date.month);

        char dayFolder[16];
        snprintf(dayFolder, sizeof(dayFolder), "%02d", date.day);

        fs::path monthPath = currentDir / monthFolder;
        fs::path dayPath = monthPath / dayFolder;

        if (!fs::exists(monthPath)) {
            fs::create_directory(monthPath);
        }

        if (!fs::exists(dayPath)) {
            fs::create_directory(dayPath);
        }

        fs::path destPath = dayPath / filename;

        if (fs::exists(destPath)) {
            continue;
        }

        fs::rename(filePath, destPath);
        moved++;
    }

    std::cout << "Done. Moved " << moved << " file(s)." << std::endl;

    return 0;
}
