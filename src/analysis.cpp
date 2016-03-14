#include "analysis.h"

#include "parser.h"

namespace nova 
{

void Analysis::buildSymbolTable()
{
    Func pre_func = [&](AstPtr node)
    {
        if (node->getAstType() == AstType::kVariable) 
        {
            VariableAstPtr ptr = std::dynamic_pointer_cast<VariableAst>(ptr);
            if (ptr) 
            {
                symbol_table_.insert(ptr->name(), ptr->getTokenLocation());
            }
        }
    };

    traversal(root_, pre_func, Func());
}

void Analysis::typeCheck()
{

}
 
void Analysis::traversal(AstPtr node, Func pre_func, Func post_func)
{
    if (node == nullptr) 
    {
        return;   
    }

    if (pre_func) 
    {
        pre_func(node);   
    }

    switch (node->getAstType()) 
    {
        case AstType::kIf:
        {
            IfStatementAstPtr ptr = std::dynamic_pointer_cast<IfStatementAst>(node);
            if (ptr) 
            {
                traversal(ptr->testPart(), pre_func, post_func);
                traversal(ptr->thenPart(), pre_func, post_func);
                if (ptr->elsePart()) 
                {
                    traversal(ptr->elsePart(), pre_func, post_func);      
                }
            }
            break;
        }

        case AstType::kRepeat:
        {
            RepeatStatementAstPtr ptr = std::dynamic_pointer_cast<RepeatStatementAst>(ptr);
            if (ptr) 
            {
                traversal(ptr->bodyPart(), pre_func, post_func);
                traversal(ptr->testPart(), pre_func, post_func);
            }
            break;
        }

        case AstType::kAssign:
        {
            AssignStatementAstPtr ptr = std::dynamic_pointer_cast<AssignStatementAst>(ptr);
            if (ptr) 
            {
                traversal(ptr->variable(), pre_func, post_func);
                traversal(ptr->expression(), pre_func, post_func);
            }
            break;
        }

        case AstType::kRead:
        {
            ReadStatementAstPtr ptr = std::dynamic_pointer_cast<ReadStatementAst>(ptr);
            if (ptr) 
            {
                traversal(ptr->variable(), pre_func, post_func);       
            }
            break;
        }

        case AstType::kWrite:
        {
            WriteStatementAstPtr ptr = std::dynamic_pointer_cast<WriteStatementAst>(ptr);
            if (ptr) 
            {
                traversal(ptr->expression(), pre_func, post_func);                
            }
            break;
        }

        case AstType::kExpression:
        {
            ExpressionAstPtr ptr = std::dynamic_pointer_cast<ExpressionAst>(ptr);
            if (ptr) 
            {
                traversal(ptr->leftPart(), pre_func, post_func);
                traversal(ptr->rightPart(), pre_func, post_func);
            }
            break;
        }

        default:
            break;
    }

    if (post_func) 
    {
        post_func(node);   
    }
}
   
} // namespace nova
