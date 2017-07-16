#include "scanner.h"
#include "error.h"

namespace nova {

bool Scanner::error_flag_ = false;

Scanner::Scanner(const std::string& file_name)
    : file_name_(file_name),
      input_(file_name),
      line_(1),
      column_(0),
      current_char_(0),
      state_(State::kNone),
      token_(TokenType::kUnknownType, 
             TokenValue::kUnReserved, 
             TokenLocation(),
             -1,
             "",
             "") {
    addToken("if", makeTokenRecord(TokenType::kKeyword, TokenValue::kIf, -1));
    addToken("then", makeTokenRecord(TokenType::kKeyword, TokenValue::kThen, -1));
    addToken("else", makeTokenRecord(TokenType::kKeyword, TokenValue::kElse, -1));
    addToken("end", makeTokenRecord(TokenType::kKeyword, TokenValue::kEnd, -1));
    addToken("repeat", makeTokenRecord(TokenType::kKeyword, TokenValue::kRepeat, -1));
    addToken("until", makeTokenRecord(TokenType::kKeyword, TokenValue::kUntil, -1));
    addToken("read", makeTokenRecord(TokenType::kKeyword, TokenValue::kRead, -1));
    addToken("write", makeTokenRecord(TokenType::kKeyword, TokenValue::kWrite, -1));
    addToken("+", makeTokenRecord(TokenType::kOperator, TokenValue::kPlus, 5));
    addToken("-", makeTokenRecord(TokenType::kOperator, TokenValue::kMinus, 5));
    addToken("*", makeTokenRecord(TokenType::kOperator, TokenValue::kMultiply, 10));
    addToken("/", makeTokenRecord(TokenType::kOperator, TokenValue::kDivide, 10));
    addToken("=", makeTokenRecord(TokenType::kOperator, TokenValue::kEqual, 2));
    addToken("<", makeTokenRecord(TokenType::kOperator, TokenValue::kLess, 2));
    addToken("(", makeTokenRecord(TokenType::kDelimiter, TokenValue::kLeftParenthesis, -1));
    addToken(")", makeTokenRecord(TokenType::kDelimiter, TokenValue::kRightParenthesis, -1));
    addToken(";", makeTokenRecord(TokenType::kDelimiter, TokenValue::kSemicolon, -1));
    addToken(":=", makeTokenRecord(TokenType::kOperator, TokenValue::kAssign, 0));

    getNextChar();
}

void Scanner::addToken(const std::string& name, TokenRecordPtr&& record) {
    if (map_.find(name) == map_.end()) {
        map_[name] = std::move(record);
    }
}

void Scanner::getNextChar() {
   current_char_ = static_cast<char>(input_.get());
   if (current_char_ == '\n') {
        ++line_;
        column_ = 0;
   } else {
       ++column_;
   }
}
void Scanner::makeToken(TokenType token_type, 
                        TokenValue token_value, 
                        TokenLocation token_location,
                        int symbol_precedence, 
                        const std::string& name) {
    token_ = Token(token_type, token_value, token_location, symbol_precedence, name);
    buffer_.clear();
    state_ = State::kNone;
}

void Scanner::makeToken(TokenType token_type, 
                        TokenValue token_value, 
                        TokenLocation token_location,
                        int symbol_precedence, 
                        const std::string& name, 
                        const std::string& string_value) {
    token_ = Token(token_type, token_value, token_location, symbol_precedence, name, string_value);
    buffer_.clear();
    state_ = State::kNone;
}

void Scanner::makeToken(TokenType token_type, 
                        TokenValue token_value, 
                        TokenLocation token_location,
                        int symbol_precedence, 
                        const std::string& name, 
                        int64_t int_value) {
    token_ = Token(token_type, token_value, token_location, symbol_precedence, name, int_value);
    buffer_.clear();
    state_ = State::kNone;
}

char Scanner::peekChar() {
    char c = static_cast<char>(input_.peek());
    return c;
}

void Scanner::addToBuffer(char c) {
    buffer_.push_back(c);
}

void Scanner::popBuffer() {
    buffer_.pop_back();
}

void Scanner::errorReport(const std::string& message) {
    errorToken(getTokenLocation().toString() + message);
}

void Scanner::skipWhitespace() {
    while (std::isspace(current_char_)) {
        getNextChar(); 
    }
}

Token Scanner::getNextToken() {
    state_ = State::kStart;

    do {
        switch (state_) {
            case State::kStart:
                handleStartState();
                break;

            case State::kNumber:
                handleNumberState(); 
                break;

            case State::kIdentifier:
                handleIdentifierState(); 
                break;

            case State::kComment:
                handleCommentState();               
                break;

            case State::kOperator:
                handleOperatorState();
                break;

            case State::kEndOfFile:
                handEndOfFileState();
                break;

            default:
                errorReport("Match error state.");
                state_ = State::kNone;
                buffer_.clear();
                break;
        }
    } while (state_ != State::kNone);

    return token_;
}

void Scanner::handleStartState() {
    skipWhitespace();
    if (input_.eof()) {
        state_ = State::kEndOfFile;
    } else if (std::isdigit(current_char_)) {
        state_ = State::kNumber;
    } else if (std::isalpha(current_char_)) {
        state_ = State::kIdentifier;
    } else if (current_char_ == '{') {
        state_ = State::kComment;   
    } else {
        state_ = State::kOperator;
    }
}

void Scanner::handleCommentState() {
    updateTokenLocation();
    while (current_char_ != '}') {
        getNextChar();
        if (input_.eof()) {
            std::string message = "End of file happened in comment, } is expected, buf find ";
            errorReport(message + current_char_);
            buffer_.clear();
            break;
        }
    }
    state_ = State::kStart;
    getNextChar();
}

void Scanner::handleNumberState() {
    updateTokenLocation();
    while (std::isdigit(current_char_)) {
        addToBuffer(current_char_);
        getNextChar();   
    }
    makeToken(TokenType::kNumber, TokenValue::kUnReserved, getTokenLocation(), -1, buffer_, std::stoll(buffer_));
}

void Scanner::handleIdentifierState() {
    updateTokenLocation();
    while (std::isalpha(current_char_)) {
        addToBuffer(current_char_);
        getNextChar();
    }
    Map::iterator it = map_.find(buffer_);
    if (it != map_.end()) {
        makeToken(it->second->token_type, it->second->token_value, getTokenLocation(), it->second->symbol_precedence, buffer_);
    } else {
        makeToken(TokenType::kIdentifier, TokenValue::kUnReserved, getTokenLocation(), -1, buffer_);
    }
}

void Scanner::handleOperatorState() {
    updateTokenLocation();
    addToBuffer(current_char_);
    addToBuffer(peekChar());
    Map::iterator it = map_.find(buffer_);
    if (it != map_.end()) {
        getNextChar();
        makeToken(it->second->token_type, it->second->token_value, getTokenLocation(), it->second->symbol_precedence, buffer_);
        getNextChar();
    } else {
        popBuffer();
        it = map_.find(buffer_);
        if (it != map_.end()) {
            makeToken(it->second->token_type, it->second->token_value, getTokenLocation(), it->second->symbol_precedence, buffer_);
            getNextChar();
        } else {
            errorReport("error: invalid character '" + buffer_ + "'");
            state_ = State::kNone;
            buffer_.clear();
        }
    }
}

void Scanner::handEndOfFileState() {
    updateTokenLocation();
    makeToken(TokenType::kEndOfFile, TokenValue::kUnReserved, getTokenLocation(), -1, buffer_);
    input_.close();
}

} // namespace nova

