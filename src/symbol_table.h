#ifndef __NOVA_SYMBOL_TABLE_H__
#define __NOVA_SYMBOL_TABLE_H__

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

#include "token.h"

namespace nova 
{

struct SymbolRecord
{
    SymbolRecord(const std::string& symbol_name, int symbol_index, int symbol_line, int symbol_number)
        : name(symbol_name),
          index(symbol_index)
    {
        location.push_back(std::make_pair(symbol_line, symbol_number));
    }

    std::string name;
    int index;
    std::vector<std::pair<int, int>> location; 
};

typedef std::unique_ptr<SymbolRecord> SymbolRecordPtr;

inline SymbolRecordPtr makeSymbolRecord(const std::string& name, int index, int line, int column)
{
    return std::make_unique<SymbolRecord>(name, index, line, column);
}

class SymbolTable
{
public:
    SymbolTable()
        : current_index_(0)
    {
    }

    bool insert(const std::string& name, const TokenLocation& location);
    int lookup(const std::string& name);
    void printSymbolTable() const;

private:
    bool innerInsert(const std::string& name, const TokenLocation& location);

private:
    typedef std::unordered_map<std::string, SymbolRecordPtr> HashMap;

    HashMap hash_map_;
    int current_index_;
};
    
} // namespace nova

#endif
