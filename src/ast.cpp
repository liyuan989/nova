#include "ast.h"

namespace nova 
{

Ast::Ast(const TokenLocation& location, AstType type)
    : location_(location), ast_type_(type), expression_type_(ExpressionType::kVoid)
{
}

const std::string Ast::getExpressionName() const
{
    switch (expression_type_) 
    {
        case ExpressionType::kVoid:
            return "void";

        case ExpressionType::kInteger:
            return "integer";

        case ExpressionType::kBoolean:
            return "boolean";

        default:
            return "unknown";
    } 
}

IfStatementAst::IfStatementAst(const TokenLocation& location, 
                               AstType type, 
                               AstPtr test_part, 
                               AstPtr then_part, 
                               AstPtr else_part)
    : Ast(location, type),
      test_part_(test_part),
      then_part_(then_part),
      else_part_(else_part)
{
}
    
RepeatStatementAst::RepeatStatementAst(const TokenLocation& location, 
                                       AstType type, 
                                       AstPtr body_part, 
                                       AstPtr test_part)
    : Ast(location, type),
      body_part_(body_part),
      test_part_(test_part)
{
}

AssignStatementAst::AssignStatementAst(const TokenLocation& location, 
                                       AstType type, 
                                       VariableAstPtr var, 
                                       AstPtr expr)
    : Ast(location, type),
      variable_(var),
      expression_(expr)
{
}

ReadStatementAst::ReadStatementAst(const TokenLocation& location, AstType type, VariableAstPtr var)
    : Ast(location, type), variable_(var)
{
}

WriteStatementAst::WriteStatementAst(const TokenLocation& location, AstType type, AstPtr expr)
    : Ast(location, type), expression_(expr)
{
}

ExpressionAst::ExpressionAst(const TokenLocation& location, 
                             AstType type, 
                             const std::string& name,
                             TokenValue token_value,
                             AstPtr left_part, 
                             AstPtr right_part)
    : Ast(location, type),
      operator_name_(name),
      operator_value_(token_value),
      left_part_(left_part),
      right_part_(right_part)
{
}

ExpressionAst::ExpressionAst(const TokenLocation& location, 
                             AstType type, 
                             AstPtr left_part) 
    : Ast(location, type),
      left_part_(left_part)
{
}


ConstantAst::ConstantAst(const TokenLocation& location, AstType type, int64_t value)
    : Ast(location, type), int_value_(value)
{
}

VariableAst::VariableAst(const TokenLocation& location, AstType type, const std::string& var_name)
    : Ast(location, type), name_(var_name)
{
}

} // namespace nova
