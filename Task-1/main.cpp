#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <unistd.h>

namespace fs = std::filesystem;
static std::string searchString = "require";
static fs::path outputFile = "task1-result.txt";

class TFiles {
public:
    fs::path fullname;
    std::vector<fs::path>
        requires;

    TFiles(const fs::path &p_fullname) : fullname{fs::relative(p_fullname)} {
        std::string searchString = "require";

        std::ifstream file(fullname);
        if (!file.is_open()) return;

        std::string line;
        while (std::getline(file, line)) {
            size_t pos = 0;
            while (1) {
                pos = line.find(searchString, pos);
                if (pos == std::string::npos) break;
                pos += searchString.length();
                size_t openQuotePos = line.find("'", pos);
                if (openQuotePos == std::string::npos) continue;
                size_t closeQuotePos = line.find("'", openQuotePos + 1);
                if (closeQuotePos == std::string::npos) continue;
                requires.push_back(fs::relative(line.substr(openQuotePos + 1, closeQuotePos - openQuotePos - 1)));
            }
        }
        file.close();
    };

    friend bool operator==(const TFiles &lhs, const TFiles &rhs) {
        return lhs.fullname == rhs.fullname;
    }
};

struct Visited {
    const fs::path fullname;
    bool done;

    Visited(const fs::path &p_fullname) : fullname{p_fullname}, done{false} {
        // std::cout << fullname << "   Done = " << done << std::endl;
    }
};

std::vector<const TFiles *> fileList;

void getDirectoryItems(const fs::path &p) {
    for (const auto &dir_entry : fs::directory_iterator(p)) {
        const fs::path path = dir_entry.path();
        if (dir_entry.is_directory())
            getDirectoryItems(path);
        else if (fs::path(path).extension() == ".txt")
            fileList.push_back(new TFiles(path));
    }
    return;
}

std::vector<fs::path> stack;
std::vector<Visited> visited;

bool DFS(const TFiles &file) {
    for (const auto &require : file.requires) {
        auto it = std::find_if(fileList.begin(), fileList.end(), [&](const TFiles *&f) {
            return f->fullname == require;
        });
        if (it == fileList.end()) continue;

        auto foundVisited = std::find_if(visited.begin(), visited.end(), [&](const Visited &v) {
            return v.fullname == (**it).fullname;
        });
        if (foundVisited == visited.end()) {
            visited.push_back(Visited((**it).fullname));
            if (!DFS(**it)) return false;
        } else if (!foundVisited->done) {
            visited.push_back(Visited(foundVisited->fullname));
            return false;
        }
    }

    visited.back().done = true;
    stack.push_back(file.fullname);
    return true;
}

std::vector<fs::path> topologicalSort() {
    for (const auto &file : fileList) {
        if (std::find_if(visited.begin(), visited.end(), [&](const Visited &v) {
                return v.fullname == file->fullname;
            }) == visited.end()) {
            visited.push_back(Visited(file->fullname));
            if (!DFS(*file)) throw 99;
        }
    }

    return stack;
}

void concatenateFiles(std::vector<fs::path> &files) {
    std::ofstream output(outputFile, std::ios::binary | std::ios::app);

    for (auto &file : files) {
        std::ifstream input(file, std::ios::binary);
        output << input.rdbuf();
        input.close();
    }

    output.close();
}

int main(int argc, char** argv) {
    fs::path parseDir = ".";

    int ret;
    while((ret = getopt(argc, argv, "d:o:")) != -1) {
        switch (ret) {
        case 'd':
            parseDir = optarg;
            break;
        case 'o':
            outputFile = optarg;
            break;
        }
    }

    if (fs::exists(outputFile)) fs::remove(outputFile);

    getDirectoryItems(parseDir);

    std::cout << "The generated list of input data:" << std::endl;
    int i = 0;
    for (auto &item : fileList) {
        std::cout << "filelist[" << i++ << "]:" << std::endl;
        std::cout << "            fullname: " << item->fullname << "]:" << std::endl;
        std::cout << "            requires: " << std::endl;
        for (auto require : item->requires)
            std::cout << "                      " << require << std::endl;
    }
    try {
        std::vector<fs::path> result = topologicalSort();
        std::cout << "Sorted list of files:" << std::endl;
        for (auto &item : result) {
            std::cout << item << std::endl;
        }
        concatenateFiles(result);

    } catch (int iErr) {
        std::cerr << "Error: Circular dependency detected!" << std::endl;
        if (!visited.empty()) {
            std::cerr << "Dependency chain: ";
            int i;
            for (i = visited.size() - 1; i >= 0; i--)
                if (visited[i].done) break;
            for (i++; i < visited.size(); i++)
                std::cerr << visited[i].fullname << "->";
            std::cerr << std::endl;
        }
    }

    for (auto &item : fileList)
        delete item;

    return 0;
}
