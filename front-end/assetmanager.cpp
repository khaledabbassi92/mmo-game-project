#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <vector>
#include "json.hpp"
#include "core.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

using json = nlohmann::json;

// Helper to upload a PNG file to VRAM
static unsigned int LoadTextureToVRAM(const std::string& filePath) {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true); 

    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "[AssetManager Error] Failed to load texture at: " << filePath << "\n";
        return 0;
    }

    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    std::cout << "[AssetManager] Loaded Texture '" << filePath << "' -> GPU Handle: " << textureID << "\n";
    return textureID;
}

// Loads OpenGL texture handles into outAssets.textureID
void LoadWorldTextures(WorldStaticAssets& assets) {
    for (std::size_t i = 0; i < assets.count; ++i) {
        if (!assets.texturePath[i].empty()) {
            assets.textureID[i] = LoadTextureToVRAM(assets.texturePath[i]);
        } else {
            assets.textureID[i] = 0;
        }
    }
}

bool loadWorldStaticAssets(const std::string& filePath, WorldStaticAssets& outAssets) {
    std::ifstream file(filePath);
    std::string loadedPath = filePath;

    if (!file.is_open()) {
        std::vector<std::string> searchPaths = {
            filePath,
            "front-end/" + filePath,
            "../front-end/" + filePath,
            "../" + filePath
        };

        for (const auto& path : searchPaths) {
            file.open(path);
            if (file.is_open()) {
                loadedPath = path;
                break;
            }
            file.clear();
        }
    }

    if (!file.is_open()) {
        std::cerr << "Error: Could not open JSON file at " << filePath << "\n";
        return false;
    }

    std::cout << "[AssetManager] Loaded JSON file from: " << loadedPath << std::endl;

    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        std::cerr << "JSON Parse Error: " << e.what() << "\n";
        return false;
    }

    outAssets.count = 0;

    if (!j.is_array()) {
        std::cerr << "Error: Expected top-level JSON array [].\n";
        return false;
    }

    for (const auto& item : j) {
        if (outAssets.count >= MAX_ASSETS) {
            std::cerr << "Warning: Exceeded MAX_ASSETS capacity of " << MAX_ASSETS << "\n";
            break;
        }

        std::size_t idx = outAssets.count;

        // Position
        if (item.contains("position") && item["position"].is_object()) {
            outAssets.position[idx].x = item["position"].value("x", 0.0f);
            outAssets.position[idx].y = item["position"].value("y", 0.0f);
        } else {
            outAssets.position[idx].x = 0.0f;
            outAssets.position[idx].y = 0.0f;
        }

        // Metadata
        outAssets.scale[idx]  = item.value("scale", 1.0f);
        outAssets.meshId[idx] = item.value("meshId", 0);
        outAssets.type[idx]   = item.value("type", 0);

        // Texture Path
        if (item.contains("texturePath")) {
            outAssets.texturePath[idx] = item.value("texturePath", "");
        } else {
            outAssets.texturePath[idx] = item.value("textureId", "");
        }

        // Flags
        if (item.contains("flags") && item["flags"].is_array()) {
            auto instanceFlags = item["flags"].get<std::vector<int>>();
            std::size_t flagsToCopy = std::min(instanceFlags.size(), static_cast<std::size_t>(MAX_FLAGS_PER_ASSET));

            outAssets.flagCount[idx] = flagsToCopy;
            for (std::size_t f = 0; f < flagsToCopy; ++f) {
                outAssets.flags[idx][f] = instanceFlags[f];
            }
        } else {
            outAssets.flagCount[idx] = 0;
        }

        outAssets.count++;
    }

    return true;
}

void printWorldStaticAssets(const WorldStaticAssets& assets) {
    std::cout << "\n========== WORLD STATIC ASSETS DEBUG ==========\n";
    std::cout << "Active Assets Loaded : " << assets.count << " / " << MAX_ASSETS << "\n";
    std::cout << "Total Memory Footprint: ~" << (sizeof(WorldStaticAssets) / 1024) << " KB\n";
    std::cout << "===============================================\n\n";

    if (assets.count == 0) {
        std::cout << "No assets currently stored in memory.\n";
        return;
    }

    for (std::size_t i = 0; i < assets.count; ++i) {
        std::cout << "Asset [" << std::setw(3) << i << "] | "
                  << "Type: " << std::setw(3) << assets.type[i] << " | "
                  << "Mesh ID: " << std::setw(4) << assets.meshId[i] << " | "
                  << "Texture: " << assets.texturePath[i] << " | "
                  << "Pos: (" << std::setw(6) << std::fixed << std::setprecision(2) << assets.position[i].x << ", " 
                             << std::setw(6) << assets.position[i].y << ") | "
                  << "Scale: " << std::setw(4) << assets.scale[i] << " | "
                  << "Flags (" << assets.flagCount[i] << "): [ ";

        for (std::size_t f = 0; f < assets.flagCount[i]; ++f) {
            std::cout << assets.flags[i][f] << (f + 1 < assets.flagCount[i] ? ", " : "");
        }
        std::cout << " ]\n";
    }

    std::cout << "===============================================\n";
}