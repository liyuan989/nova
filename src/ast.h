#ifndef __NOVA_AST_H__
#define __NOVA_AST_H__

#include <string>
#include <memory>

#include "token.h"

namespace nova 
{

enum class AstType 
{
    kNone,
    kIf,
    kRepeat,
    kAssign,
    kRead,
    kWrite,
    kExpression,
    kConstant,
    kVariable,
};

enum class ExpressionType 
{ 
    kVoid,
    kInteger,
    kBoolean,
};

class Ast;
class IfStatementAst;
class RepeatStatementAst;
class AssignStatementAst;
class ReadStatementAst;
class WriteStatementAst;
class ExpressionAst;
class ConstantAst;
class VariableAst;

typedef std::shared_ptr<Ast> AstPtr;
typedef std::shared_ptr<IfStatementAst> IfStatementAstPtr;
typedef std::shared_ptr<RepeatStatementAst> RepeatStatementAstPtr;
typedef std::shared_ptr<AssignStatementAst> AssignStatementAstPtr;
typedef std::shared_ptr<ReadStatementAst> ReadStatementAstPtr;
typedef std::shared_ptr<WriteStatementAst> WriteStatementAstPtr;
typedef std::shared_ptr<ExpressionAst> ExpressionAstPtr;
typedef std::shared_ptr<ConstantAst> ConstantAstPtr;
typedef std::shared_ptr<VariableAst> VariableAstPtr;

class Ast
{
public:
    Ast(const TokenLocation& location, AstType type);
    virtual ~Ast() = default;

    AstType getAstType() const
    {
        return ast_type_;
    }

    void setNext(const AstPtr& p)
    {
        next_ = p;
    }

    AstPtr next() const
    {
        return next_;
    }

    TokenLocation getTokenLocation() const
    {
        return location_;
    }

    void setExpressionType(ExpressionType type)
    {
        expression_type_ = type;
    }

    ExpressionType getExpressionType() const
    {
        return expression_type_;
    }

    const std::string getExpressionName() const;

private:
    TokenLocation location_;
    AstType ast_type_;
    ExpressionType expression_type_;
    AstPtr next_;
};

class IfStatementAst : public Ast
{
public:
    IfStatementAst(const TokenLocation& location, AstType type, AstPtr test_part, AstPtr then_part, AstPtr else_part);
    virtual ~IfStatementAst() = default;
   
    AstPtr testPart() const
    {
        return test_part_;
    }

    AstPtr thenPart() const
    {
        return then_part_;
    }

    AstPtr elsePart() const
    {
        return else_part_;
    }
        
private:
    AstPtr test_part_;
    AstPtr then_part_;
    AstPtr else_part_;
};

class RepeatStatementAst : public Ast
{
public:
    RepeatStatementAst(const TokenLocation& location, AstType type, AstPtr body_part, AstPtr test_part);
    virtual ~RepeatStatementAst() = default;

    AstPtr bodyPart() const
    {
        return body_part_;
    }

    AstPtr testPart() const
    {
        return test_part_;
    }

private:
    AstPtr body_part_;
    AstPtr test_part_;
};

class AssignStatementAst : public Ast
{
public:
    AssignStatementAst(const TokenLocation& location, AstType type, VariableAstPtr var, AstPtr expr);
    virtual ~AssignStatementAst() = default;

    VariableAstPtr variable() const
    {
        return variable_;
    }

    AstPtr expression() const
    {
        return expression_;
    }

private:
    VariableAstPtr variable_;
    AstPtr expression_;
};

class ReadStatementAst : public Ast
{
public:
    ReadStatementAst(const TokenLocation& location, AstType type, VariableAstPtr var);
    virtual ~ReadStatementAst() = default;

    VariableAstPtr variable() const
    {
        return variable_;
    }

private:
    VariableAstPtr variable_;
};

class WriteStatementAst : public Ast
{
public:
    WriteStatementAst(const TokenLocation& location, AstType type, AstPtr expr);
    virtual ~WriteStatementAst() = default;

    AstPtr expression() const
    {
        return expression_;
    }

private:
    AstPtr expression_;
};

class ExpressionAst  : public Ast
{
public:
    ExpressionAst(const TokenLocation& location, AstType type, AstPtr left_part);

    ExpressionAst(const TokenLocation& location, 
                  AstType type, 
                  const std::string& name, 
                  TokenValue token_value,
                  AstPtr left_part, 
                  AstPtr right_part);

    virtual ~ExpressionAst() = default;

    const std::string& operatorName() const
    {
        return operator_name_;
    }

    TokenValue operatorTokenValue() const
    {
        return operator_value_;
    }

    AstPtr leftPart() const
    {
        return left_part_;
    }

    AstPtr rightPart() const
    {
        return right_part_;
    }

private:
    std::string operator_name_;
    TokenValue operator_value_;
    AstPtr left_part_;
    AstPtr right_part_;
};

class ConstantAst : public Ast
{
public:
    ConstantAst(const TokenLocation& location, AstType type, int64_t value);
    virtual ~ConstantAst() = default;

    int64_t intValue() const
    {
        return int_value_;
    }

private:
    int64_t int_value_;
};

class VariableAst : public Ast
{
public:
    VariableAst(const TokenLocation& location, AstType type, const std::string& var_name);
    virtual ~VariableAst() = default;

    const std::string& name() const
    {
        return name_;
    }

private:
    std::string name_;
};
    
} // namespace nova

#endif
