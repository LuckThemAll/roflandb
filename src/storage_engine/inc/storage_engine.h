#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <map>
#include <string>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <shared_table.h>
#include <data_block.h>
#include <table_chunk.h>
#include <query_conditions.h>

namespace st_e {

struct SelectAnswer {
    std::vector<std::string> columns_names;
    std::vector<std::vector<std::string>> rows;
};

class StorageEngine {
public:
    using ConditionPtr = std::shared_ptr<cond::Condition>;
    using UpdateValues = std::shared_ptr<std::vector<std::pair<std::string, std::string>>>;
    StorageEngine( const StorageEngine&) = delete;
    StorageEngine& operator=(StorageEngine&) = delete;

    static StorageEngine& get_instance();

    void add_table(const Table& table);
    void delete_table(const std::string& table_name);

    const Table& get_table_by_name(const std::string& table_name);
    void insert(const std::string& table_name, const TableRow& row);
    void update(const std::string& table_name, UpdateValues update_values, ConditionPtr condition);
    void remove(const std::string& table_name, ConditionPtr condition);
    SelectAnswer select(std::string table_name, std::vector<std::string> columns_names, ConditionPtr condition);
    SelectAnswer select_all(std::string table_name, ConditionPtr condition);

private:
    StorageEngine() = default;

    SharedTable& tables_ = SharedTable::get_instance();

    DataBlock get_block(const std::string& table_name, uint32_t number);
    DataBlock get_first_block(const std::string& table_name);
    DataBlock get_last_block(const std::string& table_name);

    DataBlock append_new_block(const std::string& table_name, const DataBlock& block);
    void append_record_to_block(const std::vector<char>& buffer, const DataBlock& block, const Table& table);
    void rewrite_record(const DataBlock& block, TableChunk& table_chunk, const Table& table);
};

}//namespace st_e
