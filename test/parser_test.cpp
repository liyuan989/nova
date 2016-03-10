#include <iostream>

#include "parser.h"

using namespace nova;

void printSpace(int count)
{
    for (int i = 0; i < count; ++i) 
    {
        std::cout << " "; 
    }
}

void printTree(AstPtr root, int& count)
{
    count += 4;
    while (root != nullptr) 
    {
        printSpace(count);
        switch (root->getAstType()) 
        {
            case AstType::kIf:
            {
                IfStatementAstPtr ptr = std::dynamic_pointer_cast<IfStatementAst>(root);
                if (ptr) 
                {
                    std::cout << "If" << std::endl;                                   
                    printTree(ptr->testPart(), count);
                    printTree(ptr->thenPart(), count);
                    if (ptr->elsePart()) 
                    {
                        printTree(ptr->elsePart(), count);
                    }
                }
                break;
            }

            case AstType::kRepeat:
            {
                RepeatStatementAstPtr ptr = std::dynamic_pointer_cast<RepeatStatementAst>(root);
                if (ptr) 
                {
                    std::cout << "Repeat" << std::endl;   
                    printTree(ptr->bodyPart(), count);
                    printTree(ptr->testPart(), count);
                }
                break;
            }

            case AstType::kAssign:
            {
                AssignStatementAstPtr ptr = std::dynamic_pointer_cast<AssignStatementAst>(root);
                if (ptr) 
                {
                    std::cout << "Assign to: " << ptr->variable()->name() << std::endl;
                    printTree(ptr->expression(), count);
                }
                break;
            }

            case AstType::kRead:
            {
                ReadStatementAstPtr ptr = std::dynamic_pointer_cast<ReadStatementAst>(root);
                if (ptr) 
                {
                    std::cout << "Read: " << ptr->variable()->name() << std::endl;   
                }
                break;
            }

            case AstType::kWrite:
            {
                WriteStatementAstPtr ptr = std::dynamic_pointer_cast<WriteStatementAst>(root);
                if (ptr) 
                {
                    std::cout << "Write" << std::endl;
                    printTree(ptr->expression(), count);
                }
                break;
            }

            case AstType::kExpression:
            {
                ExpressionAstPtr ptr = std::dynamic_pointer_cast<ExpressionAst>(root);
                if (ptr) 
                {
                    std::cout << "Op: " << ptr->operatorName() << std::endl;
                    printTree(ptr->leftPart(), count);
                    printTree(ptr->rightPart(), count);
                }
                break;
            }

            case AstType::kConstant:
            {
                ConstantAstPtr ptr = std::dynamic_pointer_cast<ConstantAst>(root);
                if (ptr) 
                {
                    std::cout << "const: " << ptr->intValue() << std::endl;   
                }
                break;
            }

            case AstType::kVariable:
            {
                VariableAstPtr ptr = std::dynamic_pointer_cast<VariableAst>(root);
                if (ptr) 
                {
                    std::cout << "Id: " << ptr->name() << std::endl;                   
                }
                break;
            }

            default:
                std::cout << "Unknown error." << std::endl;
                return;
        }

        root = root->next();
    }
    count -= 4;
}

int main(int argc, char* argv[])
{
    Scanner scanner("test.tiny");
    Parser parser(scanner);
    int count = -4;
    printTree(parser.parse(), count);
    return 0;
}
