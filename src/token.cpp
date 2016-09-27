#include "token.h"

namespace nova 
{

Token::Token(TokenType token_type, 
             TokenValue token_value, 
             TokenLocation token_location,
             int symbol_precedence,
             const std::string& name)
    : token_type_(token_type),
      token_value_(token_value),
      token_location_(token_location),
      symbol_precedence_(symbol_precedence),
      name_(name) {
}

Token::Token(TokenType token_type, 
             TokenValue token_value, 
             TokenLocation token_location,
             int symbol_precedence,
             const std::string& name,
             const std::string& string_value)
    : token_type_(token_type),
      token_value_(token_value),
      token_location_(token_location),
      symbol_precedence_(symbol_precedence),
      name_(name),
      string_value_(string_value) {
}

Token::Token(TokenType token_type, 
             TokenValue token_value, 
             TokenLocation token_location,
             int symbol_precedence,
             const std::string& name,
             int64_t int_value)
    : token_type_(token_type),
      token_value_(token_value),
      token_location_(token_location),
      symbol_precedence_(symbol_precedence),
      name_(name),
      int_value_(int_value) {
}

std::string Token::getTokenTypeDescription() const {
    std::string result;
    switch (token_type_) {
        case TokenType::kIdentifier:
            result = "identifier";
            break;
        case TokenType::kKeyword:
            result = "keyword";
            break;
        case TokenType::kOperator:
            result = "operator";
            break;
        case TokenType::kNumber:
            result = "number";
            break;
        case TokenType::kDelimiter:
            result = "delimiter";
            break;
        case TokenType::kEndOfFile:
            result = "eof";
            break;
        case TokenType::kUnknownType:
            result = "unknown";
            break;
        default:
            break;
    }
    return result;
}
    
} // namespace nova

