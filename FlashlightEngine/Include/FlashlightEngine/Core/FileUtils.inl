#pragma once

inline std::vector<char> ReadBinary(const std::filesystem::path &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        std::cerr << std::format("Failed to open file : {}", filename);
        throw std::runtime_error("");
    }

    size_t fileSize = file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}
