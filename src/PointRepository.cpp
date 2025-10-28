#include "PointRepository.hpp"

PointRepository::PointRepository(pqxx::connection& cx) : cx_(cx) {
  cx_.prepare("region_insert",
              "INSERT INTO inspection_region (id, group_id, coord_x, coord_y, category) "
              "VALUES ($1, $2, $3, $4, $5)");
  cx_.prepare("region_select", "SELECT id, group_id, coord_x, coord_y, category FROM inspection_region WHERE id = $1");
  cx_.prepare("region_update_coords", "UPDATE inspection_region SET coord_x = $1, coord_y = $2 WHERE id = $3");
  cx_.prepare("region_update_category", "UPDATE inspection_region SET category = $1 WHERE id = $2");
  cx_.prepare("region_list_by_group", "SELECT id, group_id, coord_x, coord_y, category FROM inspection_region WHERE group_id = $1");
  cx_.prepare("region_delete", "DELETE FROM inspection_region WHERE id = $1");
}

bool PointRepository::create(const datapoint& region) {
  pqxx::work tx{cx_};
  tx.exec_prepared("region_insert", region.id, region.group_id, region.coord_x, region.coord_y, region.category);
  tx.commit();
  return true;
}

std::optional<datapoint> PointRepository::read(int64_t id) {
  pqxx::work tx{cx_};
  pqxx::result r = tx.exec_prepared("region_select", id);
  std::optional<datapoint> out;
  if (!r.empty()) {
    datapoint reg;
    reg.id = r[0]["id"].as<int64_t>();
    reg.group_id = r[0]["group_id"].as<int64_t>();
    reg.coord_x = r[0]["coord_x"].as<float>();
    reg.coord_y = r[0]["coord_y"].as<float>();
    reg.category = r[0]["category"].as<int>();
    out = std::move(reg);
  }
  tx.commit();
  return out;
}

bool PointRepository::update_coords(int64_t id, float x, float y) {
  pqxx::work tx{cx_};
  pqxx::result r = tx.exec_prepared("region_update_coords", x, y, id);
  bool ok = (r.affected_rows() == 1);
  tx.commit();
  return ok;
}

bool PointRepository::update_category(int64_t id, int category) {
  pqxx::work tx{cx_};
  pqxx::result r = tx.exec_prepared("region_update_category", category, id);
  bool ok = (r.affected_rows() == 1);
  tx.commit();
  return ok;
}

std::vector<datapoint> PointRepository::list_by_group(int64_t group_id) {
  pqxx::work tx{cx_};
  pqxx::result r = tx.exec_prepared("region_list_by_group", group_id);
  std::vector<datapoint> out;
  out.reserve(r.size());
  for (auto const& row : r) {
    datapoint reg;
    reg.id = row["id"].as<int64_t>();
    reg.group_id = row["group_id"].as<int64_t>();
    reg.coord_x = row["coord_x"].as<float>();
    reg.coord_y = row["coord_y"].as<float>();
    reg.category = row["category"].as<int>();
    out.push_back(std::move(reg));
  }
  tx.commit();
  return out;
}

bool PointRepository::remove(int64_t id) {
  pqxx::work tx{cx_};
  pqxx::result r = tx.exec_prepared("region_delete", id);
  bool ok = (r.affected_rows() == 1);
  tx.commit();
  return ok;
}
