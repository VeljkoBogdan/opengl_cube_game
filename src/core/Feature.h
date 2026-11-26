#ifndef FEATURE_H
#define FEATURE_H

#include <algorithm>
#include <cstdint>
#include <memory>
#include <vector>

#include "glm/glm.hpp"
#include "chunk.h"

inline uint64_t mix64(uint64_t z) {
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

inline uint64_t hashCoords(uint64_t seed, int x, int z, uint32_t salt) {
    uint64_t h = mix64(seed + uint64_t(salt) * 0x9E3779B97F4A7C15ULL);
    h = mix64(h + uint64_t(uint32_t(x)) * 0xC2B2AE3D27D4EB4FULL);
    h = mix64(h + uint64_t(uint32_t(z)) * 0x165667B19E3779F9ULL);
    return h;
}

class Rng {
public:
    explicit Rng(uint64_t seed) : state(seed) {}

    uint64_t next() {
        state += 0x9E3779B97F4A7C15ULL;
        return mix64(state);
    }

    float nextFloat() { return float(next() >> 40) * (1.0f / 16777216.0f); }
    int nextInt(int lo, int hi) { return lo + int(next() % uint64_t(hi - lo + 1)); }
    bool chance(float p) { return nextFloat() < p; }

private:
    uint64_t state;
};

struct FeatureExtent {
    int radius;
    int below;
    int above;
};

struct FeatureWriter {
    Chunk& chunk;
    glm::ivec3 chunkMin;
    glm::ivec3 origin;

    bool toLocal(int dx, int dy, int dz, glm::ivec3& local) const {
        local = origin + glm::ivec3(dx, dy, dz) - chunkMin;
        return local.x >= 0 && local.x < CHUNK_WIDTH
            && local.y >= 0 && local.y < CHUNK_HEIGHT
            && local.z >= 0 && local.z < CHUNK_LENGTH;
    }

    void set(int dx, int dy, int dz, uint16_t block) {
        glm::ivec3 l;
        if (toLocal(dx, dy, dz, l)) chunk.blocks[l.x][l.y][l.z] = block;
    }

    void setIfAir(int dx, int dy, int dz, uint16_t block) {
        glm::ivec3 l;
        if (toLocal(dx, dy, dz, l) && chunk.blocks[l.x][l.y][l.z] == 0) chunk.blocks[l.x][l.y][l.z] = block;
    }
};

class Feature {
public:
    virtual ~Feature() = default;
    virtual FeatureExtent extent() const = 0;
    virtual void generate(FeatureWriter& writer, Rng& rng) const = 0;
};

class WeightedFeature : public Feature {
public:
    void add(std::shared_ptr<const Feature> feature, float weight) {
        entries.push_back({std::move(feature), weight});
        total += weight;
    }

    FeatureExtent extent() const override {
        FeatureExtent result{0, 0, 0};
        for (const Entry& e : entries) {
            FeatureExtent child = e.feature->extent();
            result.radius = std::max(result.radius, child.radius);
            result.below = std::max(result.below, child.below);
            result.above = std::max(result.above, child.above);
        }
        return result;
    }

    void generate(FeatureWriter& writer, Rng& rng) const override {
        if (entries.empty()) return;
        float pick = rng.nextFloat() * total;
        for (const Entry& e : entries) {
            if (pick < e.weight) {
                e.feature->generate(writer, rng);
                return;
            }
            pick -= e.weight;
        }
        entries.back().feature->generate(writer, rng);
    }

private:
    struct Entry {
        std::shared_ptr<const Feature> feature;
        float weight;
    };

    std::vector<Entry> entries;
    float total = 0.0f;
};

#endif