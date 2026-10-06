#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <cmath>

const int CHUNK_SIZE = 2048;

struct ChunkCoord {
    int x, y;
    bool operator==(const ChunkCoord& other) const {
        return x == other.x && y == other.y;
    }
};

struct ChunkHash {
    size_t operator()(const ChunkCoord& c) const {
        return std::hash<int>()(c.x) ^ (std::hash<int>()(c.y) + 0x9e3779b9 + (std::hash<int>()(c.x) << 6) + (std::hash<int>()(c.x) >> 2));
    }
};

struct WorldAsset {
    int id;
    float x, y;
    std::string type;
};

struct Chunk {
    ChunkCoord coord;
    std::vector<WorldAsset> assets;
};

struct SpatialPartition {
    std::unordered_map<ChunkCoord, Chunk, ChunkHash> spatialGrid;

    void processAssets(const std::vector<WorldAsset>& rawAssets) {
        spatialGrid.clear();

        for (const auto& asset : rawAssets) {
            int chunkX = static_cast<int>(std::floor(asset.x / CHUNK_SIZE));
            int chunkY = static_cast<int>(std::floor(asset.y / CHUNK_SIZE));
            ChunkCoord coord = { chunkX, chunkY };

            spatialGrid[coord].coord = coord;
            spatialGrid[coord].assets.push_back(asset);
        }
    }

    void displayGrid() {
        std::cout << "\n=== SPATIAL GRID HASHMAP CONTENTS ===" << std::endl;
        std::cout << "Total Active Chunks: " << spatialGrid.size() << "\n" << std::endl;

        for (const auto& [coord, chunk] : spatialGrid) {
            std::cout << "[Chunk X: " << coord.x << ", Y: " << coord.y << "] contains " 
                      << chunk.assets.size() << " asset(s):" << std::endl;

            for (const auto& asset : chunk.assets) {
                std::cout << "  -> Asset ID: " << asset.id 
                          << " | Type: " << asset.type 
                          << " | Position: (" << asset.x << ", " << asset.y << ")" << std::endl;
            }
            std::cout << "-----------------------------------" << std::endl;
        }
    }
};

int main() {
    std::vector<WorldAsset> mockAssets = {
        {1, 150.0f, 200.0f, "tree"},
        {2, 2200.0f, 500.0f, "rock"},
        {3, 4100.0f, 2150.0f, "building"},
        {4, 2100.0f, 2150.0f, "tree"}
    };

    SpatialPartition spatial;
    spatial.processAssets(mockAssets);
    spatial.displayGrid();

    return 0;
}