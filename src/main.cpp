#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <pqxx/pqxx>
#include <boost/program_options.hpp>
#include "datatype.hpp"
#include "InspectionRegionRepository.hpp"
#include "InspectionGroupRepository.hpp"

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

    std::ifstream fin_points(points_file), fin_categories(categories_file), fin_groups(groups_file);
    if (!fin_points || !fin_categories || !fin_groups)
    {
        std::cerr << "Cannot open one or more data files in " << data_dir << "\n";
        return 2;
    }


    // 2. read data from file
    std::vector<InspectionRegion> regions;
    std::vector<int64_t> group_ids;
    std::string p_line, c_line, g_line;
    while (std::getline(fin_points, p_line) &&
           std::getline(fin_categories, c_line) &&
           std::getline(fin_groups, g_line))
    {
        double x = 0.0, y = 0.0;
        sscanf(p_line.c_str(), "%lf %lf", &x, &y);
        int category = static_cast<int>(std::stod(c_line));
        int64_t group = static_cast<int64_t>(std::stod(g_line));
        regions.emplace_back(0, group, static_cast<float>(x), static_cast<float>(y), category); // id自动递增（暂设0）
        group_ids.push_back(group);
    }
    fin_points.close();
    fin_categories.close();
    fin_groups.close();
    std::cout << "Loaded " << regions.size() << " regions.\n";

    //3. db transaction
    std::string conn_str = "host=127.0.0.1 port=5432 dbname=postgres user=postgres";
    try
    {
        pqxx::connection conn(conn_str);
        
        std::cout << "111111" << std::endl;

        InspectionGroupRepository group_repo(conn);
        InspectionRegionRepository region_repo(conn);
        
        std::cout << "222222" << std::endl;

        // dedup group_ids
        std::set<int64_t> unique_group_ids(group_ids.begin(), group_ids.end());

        std::cout << "333333" << std::endl;

        // insert 
        for (int64_t gid : unique_group_ids)
        {
            std::cout << "insert: " << gid << std::endl;
            group_repo.create(gid);
        }

        for (const auto &reg : regions)
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
