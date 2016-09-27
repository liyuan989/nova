#include "symbol_table.h"

#include <iostream>

namespace nova {

bool SymbolTable::insert(const std::string& name, const TokenLocation& location) {
    bool result = innerInsert(name, location);
    if (result) {
        ++current_index_;   
    }
    return result;
}

bool SymbolTable::innerInsert(const std::string& name, const TokenLocation& location) {
    if (hash_map_.find(name) != hash_map_.end()) {
        hash_map_[name]->location.push_back(std::make_pair(location.line(), location.column()));
        return false;
    } else {
        hash_map_[name] = makeSymbolRecord(name, current_index_, location.line(), location.column());
        return true;
    }
}

int SymbolTable::lookup(const std::string& name) const {
    HashMap::const_iterator it = hash_map_.find(name);
    if (it != hash_map_.end()) {
        return it->second->index;   
    }
    return -1;
}

void SymbolTable::printSymbolTable() const {
    std::cout << "Symbol Table:" << std::endl;
    std::cout << "Variable Name    index    Line    Number" << std::endl;
    for (auto& record_pair : hash_map_) {
        std::cout << record_pair.second->name << "\t" << record_pair.second->index << "\t";
        for (auto location_pair : record_pair.second->location) {
            std::cout << location_pair.first << "  " << location_pair.second << "\t";   
        }
        std::cout << std::endl;   
    }   
}
    
} // namespace nova
