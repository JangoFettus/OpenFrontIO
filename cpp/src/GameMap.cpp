#include "GameMap.h"
#include <fstream>
#include <queue>
#include <stdexcept>

GameMap::GameMap(int width, int height, std::vector<uint8_t> terrain, int numLand)
    : width_(width), height_(height), terrain_(std::move(terrain)), state_(width * height),
      refToX_(width * height), refToY_(width * height), yToRef_(height),
      numLandTiles_(numLand), numTilesWithFallout_(0) {
  int ref = 0;
  for (int y = 0; y < height_; ++y) {
    yToRef_[y] = ref;
    for (int x = 0; x < width_; ++x) {
      refToX_[ref] = x;
      refToY_[ref] = y;
      ++ref;
    }
  }
}

GameMap GameMap::loadFromBin(const std::string& filename) {
  std::ifstream file(filename, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open map file: " + filename);
  }
  uint16_t wLE, hLE;
  file.read(reinterpret_cast<char*>(&wLE), 2);
  file.read(reinterpret_cast<char*>(&hLE), 2);
  int width = wLE;
  int height = hLE;
  std::vector<uint8_t> terrain(width * height);
  file.read(reinterpret_cast<char*>(terrain.data()), terrain.size());
  if (!file) {
    throw std::runtime_error("Failed to read map data from: " + filename);
  }
  int numLand = 0;
  for (uint8_t b : terrain) {
    if (b & (1 << IS_LAND_BIT)) ++numLand;
  }
  return GameMap(width, height, std::move(terrain), numLand);
}

bool GameMap::isValidCoord(int x, int y) const {
  return x >= 0 && x < width_ && y >= 0 && y < height_;
}

GameMap::TileRef GameMap::ref(int x, int y) const {
  if (!isValidCoord(x, y)) throw std::out_of_range("invalid coord");
  return static_cast<TileRef>(yToRef_[y] + x);
}

bool GameMap::isLand(TileRef r) const {
  return terrain_[r] & (1 << IS_LAND_BIT);
}

bool GameMap::isOcean(TileRef r) const {
  return terrain_[r] & (1 << OCEAN_BIT);
}

bool GameMap::isShoreline(TileRef r) const {
  return terrain_[r] & (1 << SHORELINE_BIT);
}

uint8_t GameMap::magnitude(TileRef r) const {
  return terrain_[r] & MAGNITUDE_MASK;
}

int GameMap::ownerID(TileRef r) const {
  return state_[r] & PLAYER_ID_MASK;
}

void GameMap::setOwnerID(TileRef r, int playerId) {
  if (playerId > static_cast<int>(PLAYER_ID_MASK)) {
    throw std::out_of_range("playerId too large");
  }
  state_[r] = (state_[r] & ~PLAYER_ID_MASK) | playerId;
}

bool GameMap::hasFallout(TileRef r) const {
  return state_[r] & (1 << FALLOUT_BIT);
}

void GameMap::setFallout(TileRef r, bool value) {
  bool existing = hasFallout(r);
  if (value) {
    if (!existing) {
      state_[r] |= 1 << FALLOUT_BIT;
      ++numTilesWithFallout_;
    }
  } else {
    if (existing) {
      state_[r] &= ~(1 << FALLOUT_BIT);
      --numTilesWithFallout_;
    }
  }
}

TerrainType GameMap::terrainType(TileRef r) const {
  if (isLand(r)) {
    auto mag = magnitude(r);
    if (mag < 10) return TerrainType::Plains;
    if (mag < 20) return TerrainType::Highland;
    return TerrainType::Mountain;
  }
  return isOcean(r) ? TerrainType::Ocean : TerrainType::Lake;
}

bool GameMap::isOnEdgeOfMap(TileRef r) const {
  int xVal = x(r);
  int yVal = y(r);
  return xVal == 0 || xVal == width_ - 1 || yVal == 0 || yVal == height_ - 1;
}

std::vector<GameMap::TileRef> GameMap::neighbors(TileRef r) const {
  std::vector<TileRef> result;
  int w = width_;
  int xVal = refToX_[r];
  if (r >= w) result.push_back(r - w);
  if (r < (height_ - 1) * w) result.push_back(r + w);
  if (xVal != 0) result.push_back(r - 1);
  if (xVal != w - 1) result.push_back(r + 1);
  return result;
}

int GameMap::manhattanDist(TileRef a, TileRef b) const {
  return std::abs(x(a) - x(b)) + std::abs(y(a) - y(b));
}

int GameMap::euclideanDistSquared(TileRef a, TileRef b) const {
  int dx = x(a) - x(b);
  int dy = y(a) - y(b);
  return dx * dx + dy * dy;
}

std::vector<GameMap::TileRef> GameMap::bfs(
    TileRef start,
    const std::function<bool(const GameMap&, TileRef)>& filter) const {
  std::vector<TileRef> result;
  std::vector<uint8_t> seen(width_ * height_, 0);
  std::queue<TileRef> q;
  if (filter(*this, start)) {
    seen[start] = 1;
    q.push(start);
    result.push_back(start);
  }
  while (!q.empty()) {
    TileRef cur = q.front();
    q.pop();
    for (TileRef n : neighbors(cur)) {
      if (!seen[n] && filter(*this, n)) {
        seen[n] = 1;
        q.push(n);
        result.push_back(n);
      }
    }
  }
  return result;
}

