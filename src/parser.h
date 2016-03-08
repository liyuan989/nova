#ifndef __NOVA_PARSER_H__
#define __NOVA_PARSER_H__

#include "scanner.h"
#include "ast.h"

namespace nova 
{

class Parser
{
public:
    explicit Parser(Scanner& scanner);
    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;

    AstPtr parse();

    static void setErrorFlag(bool flag)
    {
        error_flag_ = flag;
    }

    static bool getErrorFlag()
    {
        return error_flag_;
    }

private:
    bool validateToken(TokenType type, bool advance_next_token);
    bool validateToken(TokenValue value, bool advance_next_token);
    bool expectToken(TokenType type, const std::string& type_description, bool advance_next_token);
    bool expectToken(TokenValue value, const std::string& token_name, bool advance_next_token);
    void errorReport(const std::string& message);

    AstPtr parseStatementSequence();
    AstPtr parseIfStatement();
    AstPtr parseRepeatStatement();
    AstPtr parseAssignStatement();
    AstPtr parseReadStatement();
    AstPtr parseWriteStatement();
    AstPtr parseExpression();

private:
    Scanner& scanner_;
    AstPtr ast_;

    static bool error_flag_;
};
    
} // namespace nova

#endif
