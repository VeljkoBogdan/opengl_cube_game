#ifndef OPENGL_TEST_VERTEXARENA_H
#define OPENGL_TEST_VERTEXARENA_H
#include <cstdint>
#include <cstdlib>
#include <map>


class VertexArena {
    std::map<uint32_t, uint32_t> freeBlocks; // start -> size (in vertices)
public:
    VertexArena(const uint32_t capacity = 0) {
        if (capacity) freeBlocks[0] = capacity;
    }

    bool alloc(const uint32_t n, uint32_t& outStart) {
        for (auto it = freeBlocks.begin(); it != freeBlocks.end(); ++it) {
            if (it->second < n) continue;
            outStart = it->first;
            uint32_t restStart = it->first + n;
            uint32_t rest = it->second - n;
            freeBlocks.erase(it);
            if (rest) freeBlocks[restStart] = rest;
            return true;
        }
        return false; // arena full
    }

    void free(uint32_t start, uint32_t n) {
        auto next = freeBlocks.lower_bound(start);

        // merge with the block right after
        if (next != freeBlocks.end() && start + n == next->first) {
            n += next->second;
            next = freeBlocks.erase(next);
        }
        // merge with the block right before
        if (next != freeBlocks.begin()) {
            auto prev = std::prev(next);
            if (prev->first + prev->second == start) {
                prev->second += n;
                return;
            }
        }
        freeBlocks[start] = n;
    }
};


#endif //OPENGL_TEST_VERTEXARENA_H
