#include <fstream>
#include <algorithm>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <shared_table.h>


#include "shared_table.h"
#include "configuration.h"

namespace fs = boost::filesystem;

namespace st_e {

SharedTable& SharedTable::get_instance() {
    static SharedTable instance;
    return instance;
}

const Table& SharedTable::get_table(const std::string& name) {
    auto table = cached_tables_.find(name);
    if (table != cached_tables_.end()) {
        return table->second;
    }

    return load_table(name);
}

Table& SharedTable::load_table(const std::string& table_name) {
    auto invariant_table_name = boost::to_lower_copy(table_name);
    auto meta_data_file_path = get_metadata_file_path(table_name);

    if (!fs::exists(meta_data_file_path)) {
        // TODO: throw exception!
    }

    std::ifstream data_file;
    data_file.exceptions(std::ifstream::badbit);
    data_file.open(meta_data_file_path.string());

    TableBuilder table_builder(invariant_table_name);

    std::string tmp_string_type;
    std::string tmp_col_name;
    st_e::Column::Type tmp_col_type;

    while (!data_file.eof()) {
        data_file >> tmp_string_type >> tmp_col_name >> std::skipws;

        //skip new lines
        while(data_file.peek() == '\n' || data_file.peek() == '\r') {
            data_file.get();
        }



        tmp_col_type = Column::get_type_from_string(tmp_string_type);
        table_builder.add_column(tmp_col_type, tmp_col_name);
    }
    data_file.close();
    return cached_tables_.insert_or_assign(invariant_table_name, table_builder.build()).first->second;
}

void SharedTable::save_table(const Table& table) {
    // first save to disk, then to RAM
    save_to_disk(table);
    cached_tables_.insert_or_assign(table.get_name(), table);
}

void SharedTable::save_to_disk(const Table& table) const {
    auto data_dir = get_metadata_file_path(table.get_name()).parent_path();
    // create metadata file
    auto metadata_file_path = get_metadata_file_path(table.get_name());
    // Todo: try-catch
    if (!fs::exists(data_dir)) {
        fs::create_directories(data_dir);
    }

    std::ofstream metada_data_file;
    metada_data_file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    metada_data_file.open(metadata_file_path.string());

    for (const auto& column : table.get_columns()) {
        metada_data_file << column.get_type_string() << " " << column.name << '\n';
    }
    metada_data_file.close();

    // create data file
    std::ofstream data_file;
    data_file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    data_file.open(get_data_file_path(table.get_name()).string(), std::ios::binary);

    // see data_block.h for details
    // set prev and next blocks to 0
    uint32_t no_block_pointer = 0;
    data_file.write(reinterpret_cast<char*>(&no_block_pointer), sizeof(uint32_t));
    data_file.write(reinterpret_cast<char*>(&no_block_pointer), sizeof(uint32_t));

    data_file.close();
}

fs::path SharedTable::get_metadata_file_path(const std::string& table_name) const {
    auto invariant_table_name = boost::to_lower_copy(table_name);

    // **/{data_dir_path}/{table_name}/{table_name.meta}
    auto meta_data_file_path = fs::current_path() /= fs::path(DATA_DIR_PATH) /= fs::path(invariant_table_name)
            /= fs::path(table_name + ".meta");

    return meta_data_file_path.string();
}

boost::filesystem::path SharedTable::get_data_file_path(const std::string& table_name) const {
    auto invariant_table_name = boost::to_lower_copy(table_name);

    // **/{data_dir_path}/{table_name}/{table_name.data}
    auto meta_data_file_path = fs::current_path() /= fs::path(DATA_DIR_PATH) /= fs::path(invariant_table_name)
            /= fs::path(table_name + ".data");

    return meta_data_file_path.string();
}

} // namespace st_e
