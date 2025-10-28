#pragma once
#include <pqxx/pqxx>
#include <optional>
#include <vector>
#include "datatype.hpp"

class InspectionRegionRepository
{
public:
    explicit InspectionRegionRepository(pqxx::connection &cx);

    bool create(const InspectionRegion &region);
    std::optional<InspectionRegion> read(int64_t id);
    bool update_coords(int64_t id, float x, float y);
    bool update_category(int64_t id, int category);
    std::vector<InspectionRegion> list_by_group(int64_t group_id);
    bool remove(int64_t id);

private:
    pqxx::connection &cx_;
};
