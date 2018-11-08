#include <utility>
#include <show_statement.h>
//todo: remove
#include <iostream>

cmd::ShowStatement::ShowStatement() : type_(cmd::TABLE) {}

cmd::ShowStatement::ShowStatement(cmd::ShowType type) : type_(type) {}

void cmd::ShowStatement::execute(st_e::IEngineStorage& storage_engine) {
    auto table = storage_engine.get_table_by_name(name_);
    std::cout << table->get_sql();
}

cmd::ShowType cmd::ShowStatement::get_type() {
    return type_;
}

std::string cmd::ShowStatement::get_name() {
    return name_;
}

cmd::ShowStatement::ShowStatement(cmd::ShowType type, std::string name) : type_(type), name_(name){}


