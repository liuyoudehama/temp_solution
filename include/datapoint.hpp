#pragma once
#include <cstdint>

class datapoint
{
public:
    int64_t id;
    int64_t group_id;
    float coord_x = 0.0f;
    float coord_y = 0.0f;
    int category = 0;
public:
    datapoint()
    : id(0), group_id(0), coord_x(0.0f), coord_y(0.0f), category(0)
    {}
    datapoint(int64_t id, int64_t group_id,
                                   float coord_x, float coord_y,
                                   int category)
    : id(id), group_id(group_id), coord_x(coord_x), coord_y(coord_y), category(category)
    {}
    ~datapoint()
    {}
};