#ifndef TREEFEATURE_H
#define TREEFEATURE_H

#include <cstdlib>

#include "Feature.h"

struct TreeConfig {
    int minHeight;
    int maxHeight;
    int canopyRadius;
    uint16_t log;
    uint16_t leaves;
};

class TreeFeature : public Feature {
public:
    explicit TreeFeature(TreeConfig config) : cfg(config) {}

    FeatureExtent extent() const override {
        return {cfg.canopyRadius, 0, cfg.maxHeight + 1};
    }

    void generate(FeatureWriter& w, Rng& rng) const override {
        int height = rng.nextInt(cfg.minHeight, cfg.maxHeight);
        int r = cfg.canopyRadius;

        for (int dy = 0; dy < height; dy++) w.set(0, dy, 0, cfg.log);

        for (int dy = height - 2; dy <= height + 1; dy++) {
            int layerRadius = dy > height - 1 ? r - 1 : r;
            for (int dx = -layerRadius; dx <= layerRadius; dx++)
                for (int dz = -layerRadius; dz <= layerRadius; dz++) {
                    bool corner = std::abs(dx) == layerRadius && std::abs(dz) == layerRadius;
                    bool trim = corner && (dy > height - 1 || rng.chance(0.5f));
                    if (!trim) w.setIfAir(dx, dy, dz, cfg.leaves);
                }
        }
    }

private:
    TreeConfig cfg;
};

#endif