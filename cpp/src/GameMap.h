#pragma once
#include <vector>
#include <cstdint>
#include <functional>
#include <string>

enum class TerrainType {
    Plains,
    Highland,
    Mountain,
    Lake,
    Ocean
};

class GameMap {
public:
    using TileRef = uint32_t;

    GameMap(int width, int height, std::vector<uint8_t> terrain, int numLand);
    static GameMap loadFromBin(const std::string& filename);

    int width() const { return width_; }
    int height() const { return height_; }
    int numLandTiles() const { return numLandTiles_; }
    int numTilesWithFallout() const { return numTilesWithFallout_; }

    bool isValidCoord(int x, int y) const;
    TileRef ref(int x, int y) const;
    int x(TileRef r) const { return refToX_[r]; }
    int y(TileRef r) const { return refToY_[r]; }

    bool isLand(TileRef r) const;
    bool isOcean(TileRef r) const;
    bool isShoreline(TileRef r) const;
    uint8_t magnitude(TileRef r) const;

    int ownerID(TileRef r) const;
    bool hasOwner(TileRef r) const { return ownerID(r) != 0; }
    void setOwnerID(TileRef r, int playerId);

    bool hasFallout(TileRef r) const;
    void setFallout(TileRef r, bool value);

    bool isWater(TileRef r) const { return !isLand(r); }
    bool isLake(TileRef r) const { return !isLand(r) && !isOcean(r); }
    bool isShore(TileRef r) const { return isLand(r) && isShoreline(r); }
    int cost(TileRef r) const { return magnitude(r) < 10 ? 2 : 1; }
    TerrainType terrainType(TileRef r) const;

    bool isOnEdgeOfMap(TileRef r) const;
    std::vector<TileRef> neighbors(TileRef r) const;

    int manhattanDist(TileRef a, TileRef b) const;
    int euclideanDistSquared(TileRef a, TileRef b) const;

    std::vector<TileRef> bfs(TileRef start,
         const std::function<bool(const GameMap&, TileRef)>& filter) const;

private:
    int width_;
    int height_;
    std::vector<uint8_t> terrain_;
    std::vector<uint16_t> state_;
    std::vector<int> refToX_;
    std::vector<int> refToY_;
    std::vector<int> yToRef_;
    int numLandTiles_;
    int numTilesWithFallout_;

    static constexpr uint8_t IS_LAND_BIT = 7;
    static constexpr uint8_t SHORELINE_BIT = 6;
    static constexpr uint8_t OCEAN_BIT = 5;
    static constexpr uint8_t MAGNITUDE_MASK = 0x1f;

    static constexpr uint16_t PLAYER_ID_MASK = 0xfff;
    static constexpr uint16_t FALLOUT_BIT = 13;
    static constexpr uint16_t DEFENSE_BONUS_BIT = 14;
};

