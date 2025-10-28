#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <pqxx/pqxx>
#include <boost/program_options.hpp>
#include <cassert>

#include "datapoint.hpp"
#include "PointRepository.hpp"
#include "GroupRepository.hpp"

int main(int argc, char *argv[])
{
    // 1. parse args
    namespace po = boost::program_options;
    std::string data_dir;

    po::options_description desc("Options");
    desc.add_options()("data_directory", po::value<std::string>(&data_dir)->required(), "data directory path");
    po::variables_map vm;
    
    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Argument error: " << ex.what() << "\n"
                  << desc << std::endl;
        return 1;
    }

    std::string points_file = data_dir + "/points.txt";
    std::string categories_file = data_dir + "/categories.txt";
    std::string groups_file = data_dir + "/groups.txt";

    std::ifstream fin_points(points_file);
    std::ifstream fin_categories(categories_file);
    std::ifstream fin_groups(groups_file);

    if (!fin_points || !fin_categories || !fin_groups)
    {
        std::cerr << "Cannot open one or more data files in " << data_dir << "\n";
        return 2;
    }


    // 2. read data from file
    std::vector<datapoint> points;
    std::set<int64_t> groups;

    std::string line_point;
    std::string line_categories;
    std::string line_group;
    
    int point_id = 0;
    while (std::getline(fin_points, line_point) &&
           std::getline(fin_categories, line_categories) &&
           std::getline(fin_groups, line_group))
    {
        double x = 0.0, y = 0.0;
        sscanf(line_point.c_str(), "%lf %lf", &x, &y);
        int category = static_cast<int>(std::stod(line_categories));
        int64_t group = static_cast<int64_t>(std::stod(line_group));
        points.emplace_back(point_id, group, static_cast<float>(x), static_cast<float>(y), category); // id自动递增（暂设0）
        groups.insert(group);
        point_id++;
    }
    
    assert(fin_points.peek() == EOF);
    assert(fin_categories.peek() == EOF);
    assert(fin_groups.peek() == EOF);

    fin_points.close();
    fin_categories.close();
    fin_groups.close();
    std::cout << "Loaded " << points.size() << " points.\n";

    //3. db transaction
    std::string conn_str = "host=127.0.0.1 port=5432 dbname=solution_data user=postgres";
    try
    {
        pqxx::connection conn(conn_str);

        GroupRepository group_repo(conn);
        PointRepository region_repo(conn);
        
        // insert 
        for (int64_t gid : groups)
        {
            std::cout << "insert: " << gid << std::endl;
            group_repo.create(gid);
        }

        for (const auto &reg : points)
        {
            std::cout << "insert: (" << reg.coord_x << "," << reg.coord_y << ")" << std::endl;
            region_repo.create(reg);
        }

        std::cout << "All data inserted to DB!" << std::endl;

        // 可加查询测试
        // auto list = region_repo.list_by_group(0);
        // for (auto& region : list) { /* ... */ }
    }
    catch (const std::exception &ex)
    {
        std::cerr << "DB error: " << ex.what() << "\n";
        return 3;
    }

    return 0;
}
