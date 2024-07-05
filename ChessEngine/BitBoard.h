#pragma once

#include <stdint.h>

class BitBoard
{
private:
    uint64_t board;
public:
    BitBoard(uint64_t board);

   
    bool contains(int rank, int file);

};

