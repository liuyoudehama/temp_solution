#include "InspectionGroupRepository.hpp"

InspectionGroupRepository::InspectionGroupRepository(pqxx::connection& cx) : cx_(cx) {
  cx_.prepare("group_insert", "INSERT INTO inspection_group (id) VALUES ($1)");
  cx_.prepare("group_select", "SELECT id FROM inspection_group WHERE id = $1");
  cx_.prepare("group_delete", "DELETE FROM inspection_group WHERE id = $1");
  cx_.prepare("group_exists", "SELECT EXISTS (SELECT 1 FROM inspection_group WHERE id = $1)");
}

bool InspectionGroupRepository::create(int64_t id) {
  pqxx::work tx{cx_};
  tx.exec_prepared("group_insert", id);
  tx.commit();
  return true;
}

std::optional<int64_t> InspectionGroupRepository::read(int64_t id) {
  pqxx::work tx{cx_};
  pqxx::result r = tx.exec_prepared("group_select", id);
  std::optional<int64_t> out;
  if (!r.empty()) {
    out = r[0][0].as<int64_t>();
  }
  tx.commit();
  return out;
}

bool InspectionGroupRepository::remove(int64_t id) {
  pqxx::work tx{cx_};
  pqxx::result r = tx.exec_prepared("group_delete", id);
  bool ok = (r.affected_rows() == 1);
  tx.commit();
  return ok;
}

bool InspectionGroupRepository::exists(int64_t id) {
  pqxx::work tx{cx_};
  pqxx::result r = tx.exec_prepared("group_exists", id);
  bool found = false;
  if (!r.empty()) {
    found = r[0][0].as<bool>();
  }
  tx.commit();
  return found;
}
