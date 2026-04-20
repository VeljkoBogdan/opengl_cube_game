#ifndef CHUNK_H
#define CHUNK_H

#include "../constants.h"

struct Chunk {
    int blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_LENGTH];
    unsigned int VBO, VAO;
    int vertexCount;
    bool dirty;
};

#endif