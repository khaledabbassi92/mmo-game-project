#include <iostream>
#include <fstream>
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

static unsigned int LoadTextureToVRAM(const std::string& filePath) 
{
    std::vector<std::string> pathsToTry = {
        filePath,
        "assets/world/" + filePath,
        "../" + filePath,
        "../assets/world/" + filePath,
        "front-end/" + filePath,
        "../front-end/" + filePath
    };

    int width = 0, height = 0, channels = 0;
    stbi_set_flip_vertically_on_load(false); 
    unsigned char* data = nullptr;
    std::string resolvedPath = "";

    for (const auto& path : pathsToTry) {
        data = stbi_load(path.c_str(), &width, &height, &channels, 4);
        if (data) {
            resolvedPath = path;
            break;
        }
    }

    if (!data) {
        std::cerr << "[AssetManager Error] Could not find texture: " << filePath << std::endl;
        return 0;
    }

    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    std::cout << "[AssetManager] Uploaded '" << resolvedPath << "' (" << width << "x" << height << ") -> GPU ID: " << textureID << "\n";
    return textureID;
}

void LoadWorldTextures(WorldStaticAssets& assets) 
{
    for (std::size_t i = 0; i < assets.count; ++i) 
    {
        if (!assets.texturePath[i].empty()) {
            assets.textureID[i] = LoadTextureToVRAM(assets.texturePath[i]);
        } else {
            assets.textureID[i] = 0;
        }
    }
}

bool loadWorldStaticAssets(const std::string& filePath, WorldStaticAssets& outAssets) 
{
    std::ifstream file(filePath);
    std::string loadedPath = filePath;

    if (!file.is_open()) {
        std::vector<std::string> searchPaths = {
            filePath,
            "../" + filePath,
            "front-end/" + filePath,
            "../front-end/" + filePath,
            "assets/world/" + filePath,
            "../assets/world/" + filePath
        };
        for (const auto& p : searchPaths) {
            file.open(p);
            if (file.is_open()) {
                loadedPath = p;
                break;
            }
            file.clear();
        }
    }

    if (!file.is_open()) {
        std::cerr << "[AssetManager Error] Could not open JSON file: " << filePath << std::endl;
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
    if (!j.is_array()) return false;

    for (const auto& item : j) 
    {
        if (outAssets.count >= MAX_ASSETS) break;
        std::size_t idx = outAssets.count;

        if (item.contains("position") && item["position"].is_object()) {
            outAssets.position[idx].x = item["position"].value("x", 0.0f);
            outAssets.position[idx].y = item["position"].value("y", 0.0f);
        } else {
            outAssets.position[idx].x = 0.0f;
            outAssets.position[idx].y = 0.0f;
        }

        outAssets.scale[idx]  = item.value("scale", 1.0f);
        outAssets.meshId[idx] = item.value("meshId", 0);
        outAssets.type[idx]   = item.value("type", 1);

        if (item.contains("textureId")) {
            outAssets.texturePath[idx] = item.value("textureId", "");
        } else {
            outAssets.texturePath[idx] = item.value("texturePath", "");
        }

        outAssets.count++;
    }

    return true;
}

void printWorldStaticAssets(const WorldStaticAssets& assets) 
{
    std::cout << "[AssetManager] Total Static Assets Parsed: " << assets.count << "\n";
}