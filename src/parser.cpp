#include "parser.h"

#include "error.h"

namespace nova 
{

bool Parser::error_flag_ = false;

Parser::Parser(Scanner& scanner)
    : scanner_(scanner)
{
    scanner_.getNextToken();  // get first token
}

AstPtr Parser::parse()
{
    if (scanner_.getToken().getTokenType() == TokenType::kEndOfFile) 
    {
        errorReport("Unexpected end of file.");
        return ast_;
    }
    ast_ = parseStatementSequence();
    return ast_;
}

AstPtr Parser::parseStatementSequence()
{
    AstPtr head = parseStatement();
    AstPtr current_ptr = head;

    while (current_ptr != nullptr && !isEndOfStatementSequence()) 
    {
        expectToken(TokenValue::kSemicolon, ";", true);
        current_ptr->setNext(parseStatement());
        current_ptr = current_ptr->next();
    }
   
    return head;
}

bool Parser::isEndOfStatementSequence()
{
    switch (scanner_.getToken().getTokenValue()) 
    {
        case TokenValue::kElse:
        case TokenValue::kEnd:
        case TokenValue::kUntil:
            return true;

        case TokenValue::kSemicolon:
            return false;

        default:
        {
            if (scanner_.getToken().getTokenType() == TokenType::kEndOfFile) 
            {
                return true;   
            }
            else
            {
                return false;
            }
        }
    }
}

AstPtr Parser::parseStatement()
{
    switch (scanner_.getToken().getTokenValue()) 
    {
        case TokenValue::kIf:
            return parseIfStatement();                

        case TokenValue::kRepeat:
            return parseRepeatStatement();

        case TokenValue::kRead:
            return parseReadStatement();

        case TokenValue::kWrite:
            return parseWriteStatement();

        default:
        {
            if (scanner_.getToken().getTokenType() == TokenType::kIdentifier) 
            {
                return parseAssignStatement();   
            }
            else
            {
                errorReport("error: unknown token '" + scanner_.getToken().getTokenName() + "'");
                return nullptr;
            }
        }
    }
}

AstPtr Parser::parseIfStatement()
{
    TokenLocation loc = scanner_.getToken().getTokenLocation();
    if (!validateToken(TokenValue::kIf, true)) 
    {
        return nullptr;   
    }
    AstPtr test_part = parseExpression();
    if (!expectToken(TokenValue::kThen, "then", true)) 
    {
        return nullptr;   
    }
    AstPtr then_part = parseStatementSequence();
    AstPtr else_part = nullptr;

    switch (scanner_.getToken().getTokenValue()) 
    {
        case TokenValue::kEnd:
            break;

        case TokenValue::kElse:
        {
            scanner_.getNextToken();  // eat "else"
            else_part = parseStatementSequence();
            if (!expectToken(TokenValue::kEnd, "end", false)) 
            {
                return nullptr;
            }
            break;
        }

        default:
            errorReport("error: invalid token '" + scanner_.getToken().getTokenName() + "'");
            return nullptr;
    }
    scanner_.getNextToken();  // eat "end"
    return std::make_shared<IfStatementAst>(loc, AstType::kIf, test_part, then_part, else_part);
}

AstPtr Parser::parseRepeatStatement()
{
    TokenLocation loc = scanner_.getToken().getTokenLocation();
    if (!validateToken(TokenValue::kRepeat, true)) 
    {
        return nullptr;   
    }
    AstPtr body_part = parseStatementSequence();
    if (!expectToken(TokenValue::kUntil, "until", true)) 
    {
        return nullptr;   
    }
    AstPtr test_part = parseExpression();
    return std::make_shared<RepeatStatementAst>(loc, AstType::kRepeat, body_part, test_part);
}

AstPtr Parser::parseAssignStatement()
{
    TokenLocation loc = scanner_.getToken().getTokenLocation();
    if (!validateToken(TokenType::kIdentifier, false)) 
    {
        return nullptr;   
    }
    VariableAstPtr var = std::make_shared<VariableAst>(loc, AstType::kVariable, scanner_.getToken().getTokenName());
    scanner_.getNextToken();  // eat variable
    if (!expectToken(TokenValue::kAssign, ":=", true)) 
    {
        return nullptr;   
    }
    AstPtr expr = parseExpression();
    return std::make_shared<AssignStatementAst>(loc, AstType::kAssign, var, expr);
}

AstPtr Parser::parseReadStatement()
{
    TokenLocation loc = scanner_.getToken().getTokenLocation();
    if (!validateToken(TokenValue::kRead, true)) 
    {
        return nullptr;   
    }
    if (!expectToken(TokenType::kIdentifier, "identifier", false)) 
    {
        return nullptr;   
    }
    VariableAstPtr var = std::make_shared<VariableAst>(scanner_.getToken().getTokenLocation(), 
                                                       AstType::kVariable, 
                                                       scanner_.getToken().getTokenName());
    scanner_.getNextToken(); // eat variable
    return std::make_shared<ReadStatementAst>(loc, AstType::kRead, var);
}

AstPtr Parser::parseWriteStatement()
{
    TokenLocation loc = scanner_.getToken().getTokenLocation();
    if (!validateToken(TokenValue::kWrite, true)) 
    {
        return nullptr;   
    }
    AstPtr expr = parseExpression();
    return std::make_shared<WriteStatementAst>(loc, AstType::kWrite, expr);
}

AstPtr Parser::parseExpression()
{
    TokenLocation loc = scanner_.getToken().getTokenLocation();
    AstPtr left_part = parseSimpleExpression();

    std::string op_name = scanner_.getToken().getTokenName();
    TokenValue op_value = scanner_.getToken().getTokenValue();
    if (op_value != TokenValue::kLess && op_value != TokenValue::kEqual)  // < or =
    {
        return left_part;
    }

    scanner_.getNextToken();  // eat operator
    AstPtr right_part = parseSimpleExpression();
    return std::make_shared<ExpressionAst>(loc, AstType::kExpression, op_name, op_value, left_part, right_part);
}

AstPtr Parser::parseSimpleExpression()
{
    TokenLocation loc = scanner_.getToken().getTokenLocation();
    AstPtr left_part = parseTerm();

    std::string op_name = scanner_.getToken().getTokenName();
    TokenValue op_value = scanner_.getToken().getTokenValue();
    while (op_value == TokenValue::kPlus || op_value == TokenValue::kMinus)  // + or -
    {
        scanner_.getNextToken();  // eat operator
        AstPtr node = parseTerm();
        left_part = std::make_shared<ExpressionAst>(loc, AstType::kExpression, op_name, op_value, left_part, node);
        op_name = scanner_.getToken().getTokenName();
        op_value = scanner_.getToken().getTokenValue();
    }
    
    return left_part;
}

AstPtr Parser::parseTerm()
{
    TokenLocation loc = scanner_.getToken().getTokenLocation();
    AstPtr left_part = parseFactor();

    std::string op_name = scanner_.getToken().getTokenName();
    TokenValue op_value = scanner_.getToken().getTokenValue();
    while (op_value == TokenValue::kMultiply || op_value == TokenValue::kDivide)   // * or /
    {
        scanner_.getNextToken();  // eat operator
        AstPtr node = parseFactor();
        left_part = std::make_shared<ExpressionAst>(loc, AstType::kExpression, op_name, op_value, left_part, node);
        op_name = scanner_.getToken().getTokenName();
        op_value = scanner_.getToken().getTokenValue();
    }
    
    return left_part;
}

AstPtr Parser::parseFactor()
{
    TokenLocation loc = scanner_.getToken().getTokenLocation();
    AstPtr result = nullptr;

    switch (scanner_.getToken().getTokenType()) 
    {
        case TokenType::kIdentifier:
        {
            result = std::make_shared<VariableAst>(loc, AstType::kVariable, scanner_.getToken().getTokenName());
            scanner_.getNextToken();  // eat variable
            break;
        }
            
        case TokenType::kNumber:
        {
            result = std::make_shared<ConstantAst>(loc, AstType::kConstant, scanner_.getToken().getIntValue());
            scanner_.getNextToken();  // eat constant number
            break;
        }

        default:
        {
            if (!expectToken(TokenValue::kLeftParenthesis, "(", true)) 
            {
                return nullptr;   
            }
            result = parseExpression();
            if (!expectToken(TokenValue::kRightParenthesis, ")", true)) 
            {
                return nullptr;   
            }
            break;
        }
    }

    return result;
}

bool Parser::validateToken(TokenType type, bool advance_next_token)
{
    if (scanner_.getToken().getTokenType() != type) 
    {
        return false;   
    }
    if (advance_next_token) 
    {
        scanner_.getNextToken();   
    }
    return true;
}

bool Parser::validateToken(TokenValue value, bool advance_next_token)
{
    if (scanner_.getToken().getTokenValue() != value) 
    {
        return false;   
    }
    if (advance_next_token) 
    {
        scanner_.getNextToken();   
    }
    return true;
}

bool Parser::expectToken(TokenType type, const std::string& type_description, bool advance_next_token)
{
    if (scanner_.getToken().getTokenType() != type) 
    {
        errorReport("Expected '" + type_description + "', but find " + scanner_.getToken().getTokenTypeDescription() + 
                    " " + scanner_.getToken().getTokenName());
        return false;
    }
    if (advance_next_token) 
    {
        scanner_.getNextToken();   
    }
    return true;
}
    
bool Parser::expectToken(TokenValue value, const std::string& token_name, bool advance_next_token)
{
    if (scanner_.getToken().getTokenValue() != value) 
    {
        errorReport("Expected '" + token_name + "', but find " + scanner_.getToken().getTokenName());
        return false;
    }
    if (advance_next_token) 
    {
        scanner_.getNextToken();   
    }
    return true;
}

void Parser::errorReport(const std::string& message)
{
    errorSyntax(scanner_.getToken().getTokenLocation().toString() + message);
}

} // namespace nova
