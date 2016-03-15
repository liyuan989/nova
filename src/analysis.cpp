#include "analysis.h"

#include "parser.h"
#include "error.h"

namespace nova 
{

void Analysis::buildSymbolTable()
{
    Func pre_func = [this](AstPtr node)
    {
        if (node->getAstType() == AstType::kVariable) 
        {
            VariableAstPtr ptr = std::dynamic_pointer_cast<VariableAst>(node);
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
    Func post_func = std::bind(&Analysis::checkNode, this, std::placeholders::_1);
    traversal(root_, Func(), post_func);
}
 
void Analysis::traversal(AstPtr node, Func pre_func, Func post_func)
{
    while (node != nullptr)
    {
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
                RepeatStatementAstPtr ptr = std::dynamic_pointer_cast<RepeatStatementAst>(node);
                if (ptr) 
                {
                    traversal(ptr->bodyPart(), pre_func, post_func);
                    traversal(ptr->testPart(), pre_func, post_func);
                }
                break;
            }

            case AstType::kAssign:
            {
                AssignStatementAstPtr ptr = std::dynamic_pointer_cast<AssignStatementAst>(node);
                if (ptr) 
                {
                    traversal(ptr->variable(), pre_func, post_func);
                    traversal(ptr->expression(), pre_func, post_func);
                }
                break;
            }

            case AstType::kRead:
            {
                ReadStatementAstPtr ptr = std::dynamic_pointer_cast<ReadStatementAst>(node);
                if (ptr) 
                {
                    traversal(ptr->variable(), pre_func, post_func);       
                }
                break;
            }

            case AstType::kWrite:
            {
                WriteStatementAstPtr ptr = std::dynamic_pointer_cast<WriteStatementAst>(node);
                if (ptr) 
                {
                    traversal(ptr->expression(), pre_func, post_func);                
                }
                break;
            }

            case AstType::kExpression:
            {
                ExpressionAstPtr ptr = std::dynamic_pointer_cast<ExpressionAst>(node);
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

        node = node->next();
    }
}
   
void Analysis::checkNode(AstPtr node)
{
    switch (node->getAstType()) 
    {
        case AstType::kVariable:
        case AstType::kConstant:
        {
            node->setExpressionType(ExpressionType::kInteger);
            break;
        }

        case AstType::kExpression:
        {
            ExpressionAstPtr ptr = std::dynamic_pointer_cast<ExpressionAst>(node);
            if (ptr) 
            {
                if (ptr->operatorTokenValue() == TokenValue::kEqual ||
                    ptr->operatorTokenValue() == TokenValue::kLess) 
                {
                    ptr->setExpressionType(ExpressionType::kBoolean);   
                }
                else
                {
                    if (ptr->leftPart()->getExpressionType() == ExpressionType::kInteger &&
                        ptr->rightPart()->getExpressionType() == ExpressionType::kInteger) 
                    {
                        ptr->setExpressionType(ExpressionType::kInteger);      
                    }
                    else
                    {
                        std::string message = "cannot covert from '" + ptr->rightPart()->getExpressionName() +
                            "' to '" + ptr->leftPart()->getExpressionName() + "'";
                        errorReport(message);
                    }
                }
            }
            break;
        }

        case AstType::kIf:
        {
            IfStatementAstPtr ptr = std::dynamic_pointer_cast<IfStatementAst>(node);
            if (ptr) 
            {
                if (ptr->testPart()->getExpressionType() != ExpressionType::kBoolean) 
                {
                    std::string message = "cannot convert from '" + ptr->testPart()->getExpressionName() + 
                        "' to 'boolean'";
                    errorReport(message);
                }   
            }
            break;
        }

        case AstType::kRepeat:
        {
            RepeatStatementAstPtr ptr = std::dynamic_pointer_cast<RepeatStatementAst>(node);
            if (ptr) 
            {
                if (ptr->testPart()->getExpressionType() != ExpressionType::kBoolean) 
                {
                    std::string message = "cannot convert from '" + ptr->testPart()->getExpressionName() + 
                        "' to 'boolean'";
                    errorReport(message);
                }   
            }
            break;
        }

        case AstType::kAssign:
        {
            AssignStatementAstPtr ptr = std::dynamic_pointer_cast<AssignStatementAst>(node);
            if (ptr) 
            {
                if (ptr->expression()->getExpressionType() != ExpressionType::kInteger) 
                {
                    std::string message = "cannot convert from '" + ptr->expression()->getExpressionName() + 
                        "' to 'integer'";
                    errorReport(message);
                }   
            }
            break;
        }

        case AstType::kRead:
        {
            ReadStatementAstPtr ptr = std::dynamic_pointer_cast<ReadStatementAst>(node);
            if (ptr) 
            {
                if (ptr->variable()->getExpressionType() != ExpressionType::kInteger) 
                {
                    std::string message = "cannot convert from '" + ptr->variable()->getExpressionName() + 
                        "' to 'integer'";
                    errorReport(message);
                }   
            }
            break;
        }

        case AstType::kWrite:
        {
            WriteStatementAstPtr ptr = std::dynamic_pointer_cast<WriteStatementAst>(node);
            if (ptr) 
            {
                if (ptr->expression()->getExpressionType() != ExpressionType::kInteger) 
                {
                    std::string message = "cannot convert from '" + ptr->expression()->getExpressionName() + 
                        "' to 'integer'";
                    errorReport(message);
                }      
            }
            break;
        }

        default:
            break;   
    }
}

void Analysis::errorReport(const std::string& message)
{
    errorSyntax(message);
}

void Analysis::printSymbolTable() const
{
    symbol_table_.printSymbolTable();
}

} // namespace nova
