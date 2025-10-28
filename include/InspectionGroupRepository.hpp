#pragma once
#include <pqxx/pqxx>
#include <optional>
#include "datatype.hpp"

class InspectionGroupRepository
{
public:
    explicit InspectionGroupRepository(pqxx::connection &cx);

    bool create(int64_t id);
    std::optional<int64_t> read(int64_t id);
    bool remove(int64_t id);
    bool exists(int64_t id);

private:
    pqxx::connection &cx_;
};
