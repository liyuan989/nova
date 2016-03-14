#ifndef __NOVA_ANALYSIS_H__
#define __NOVA_ANALYSIS_H__

#include "ast.h"
#include "symbol_table.h"

namespace nova 
{

class Analysis
{
public:
    explicit Analysis(AstPtr ast_root)
        : root_(ast_root)
    {
    }

    Analysis(const Analysis&) = delete;
    Analysis& operator=(const Analysis&) = delete;
    
    void buildSymbolTable();
    void typeCheck();

private:
    typedef std::function<void (AstPtr)> Func;

    void traversal(AstPtr node, Func pre_func, Func post_func);
    void checkNode(AstPtr node);
    void errorReport(const std::string& message);

private:
    AstPtr root_;
    SymbolTable symbol_table_;
};

} // namespace nova

#endif
