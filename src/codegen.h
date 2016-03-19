#ifndef __NOVA_CODEGEN_H__
#define __NOVA_CODEGEN_H__

#include <vector>
#include <sstream>

#include "analysis.h"

namespace nova 
{

enum class Register 
{ 
    invalid = -1,
    ac = 0,         // accumulator
    ac1 = 1,        // accumulator1
    gp = 5,         // global pointer
    mp = 6,         // memory pointer
    pc = 7,         // program count
};

typedef std::string CodeBuffer;

class CodeGenerator
{
public:
    CodeGenerator(Analysis& analyst, 
                  const AstPtr& ptr, 
                  const std::string& file_name, 
                  bool trace_code = false);

    CodeBuffer generateCode();

    static bool getErrorFlag()
    {
        return error_flag_;
    }

private:
    void emitCodeLine(const std::string& code, const std::string& comment = std::string());

    void emitRo(const std::string& code, 
                Register r, 
                Register s, 
                Register t, 
                const std::string& comment = std::string());

    void emitRm(const std::string& code, 
                Register r, 
                int64_t d, 
                Register s, 
                const std::string& comment = std::string());

    void emitRm(int line, 
                const std::string& code, 
                Register r, 
                int64_t d, 
                Register s, 
                const std::string& comment = std::string());

    void emitCommentLine(const std::string& comment);

    void generatePrelude();
    void generateStatementSequence(AstPtr node);
    void generateExpression(AstPtr node);
    void generateIfStatement(AstPtr node);
    void generateRepeatStatement(AstPtr node);
    void generateAssignStatement(AstPtr node);
    void generateReadStatement(AstPtr node);
    void generateWriteStatement(AstPtr node);
    void generateVariable(AstPtr node);
    void generateConstant(AstPtr node);
    
    void errorReport(const std::string& message);

private:
    Analysis& analyst_;
    AstPtr root_;
    std::string file_name_;
    std::ostringstream buffer_;
    int current_line_;
    int tmp_offset_;
    bool trace_code_;

    static bool error_flag_;
};
    
} // namespace nova

#endif
