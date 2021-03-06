#pragma once

#include <string>
#include <vector>
#include "sql_statement.h"

namespace cmd{
enum DropType{
    DROP_TABLE,
    DROP_DATABASE
};

class DropStatement : public SqlStatement{
public:
    DropStatement();
    ~DropStatement() override = default;
    void execute() override;

    explicit DropStatement(std::string table_name);
    explicit DropStatement(DropType type, std::string name);

    DropType get_type() const;
    std::string get_name() const;

private:
    void check_valid() override;

    DropType type_;
    std::string name_;
};
} //namespace cmd
