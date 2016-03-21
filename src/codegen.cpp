#include "codegen.h"

#include "error.h"

namespace nova 
{

bool CodeGenerator::error_flag_ = false;

CodeGenerator::CodeGenerator(Analysis& analyst, 
                             const AstPtr& ptr, 
                             const std::string& file_name,
                             bool trace_code)
    : analyst_(analyst),
      root_(ptr), 
      file_name_(file_name), 
      current_line_(0),
      tmp_offset_(0),
      trace_code_(trace_code)
{
}

void CodeGenerator::emitCodeLine(const std::string& code, const std::string& comment)
{
    ++current_line_;
    buffer_ << current_line_ << ":   " << code;
    if (trace_code_) 
    {
        buffer_ << "\t\t* " << comment;   
    }
    buffer_ << std::endl;
}

// opcode r,s,t
void CodeGenerator::emitRo(const std::string& code, 
                           Register r, 
                           Register s, 
                           Register t, 
                           const std::string& comment)
{
    ++current_line_;
    buffer_ << current_line_ << ":   " << code << " " << static_cast<int>(r) << "," 
            << static_cast<int>(s) << "," << static_cast<int>(t);
    if (trace_code_) 
    {
        buffer_ << "\t\t* " << comment;   
    }
    buffer_ << std::endl;
}

// opcode r,d(s)
void CodeGenerator::emitRm(const std::string& code, 
                           Register r, 
                           int64_t d, 
                           Register s, 
                           const std::string&comment)
{
    ++current_line_;
    emitRm(current_line_, code, r, d, s, comment);
}

void CodeGenerator::emitRm(int line, 
                           const std::string& code, 
                           Register r, 
                           int64_t d, 
                           Register s, 
                           const std::string& comment)
{
    buffer_ << line << ":   " << code << " " << static_cast<int>(r) << "," << d
            << "(" << static_cast<int>(s) << ")";
    if (trace_code_) 
    {
        buffer_ << "\t\t* " << comment;   
    }
    buffer_ << std::endl;
}

void CodeGenerator::emitCommentLine(const std::string& comment)
{
    if (trace_code_) 
    {
        buffer_ << comment << std::endl;
    }
}

void CodeGenerator::generatePrelude()
{
    emitCommentLine("* TINY Compilation to TM Code");
    emitCommentLine("* File: " + file_name_);
    emitCommentLine("* Standard prelude:");
    emitRm("LD", Register::mp, 0, Register::ac, "load maxaddress from location 0");
    emitRm("ST", Register::ac, 0, Register::ac, "clear location 0");
    emitCommentLine("* End of standard prelude.");
}

CodeBuffer CodeGenerator::generateCode()
{
    generatePrelude();
    generateStatementSequence(root_);
    emitCommentLine("* End of execution");
    emitRo("HALT", Register::ac, Register::ac, Register::ac);
    return buffer_.str();
}

void CodeGenerator::generateStatementSequence(AstPtr node)
{
    while (node != nullptr) 
    {
        switch (node->getAstType())
        {
            case AstType::kIf:
                generateIfStatement(node);
                break;

            case AstType::kRepeat:
                generateRepeatStatement(node);
                break;

            case AstType::kAssign:
                generateAssignStatement(node);
                break;

            case AstType::kRead:
                generateReadStatement(node);
                break;

            case AstType::kWrite:
                generateWriteStatement(node);
                break;

            case AstType::kExpression:
                generateExpression(node);
                break;

            default:
                errorReport("Invalid ast type");
                break;
        }
        node = node->next();
    }
}

void CodeGenerator::generateIfStatement(AstPtr node)
{
    IfStatementAstPtr ptr = std::dynamic_pointer_cast<IfStatementAst>(node);
    if (!ptr) 
    {
        return;   
    }

    emitCommentLine("* -> if");
    generateStatementSequence(ptr->testPart());
    emitCommentLine("* if: jump to else belongs here");

    ++current_line_;
    int saved_loc = current_line_;
    generateStatementSequence(ptr->thenPart());
    emitCommentLine("* if: jump to end belongs here");

    ++current_line_;
    int saved_loc2 = current_line_;
    emitRm(saved_loc, "JEQ", Register::ac, current_line_ - saved_loc, Register::pc, "if: jmp to false");

    if (ptr->elsePart()) 
    {
        generateStatementSequence(ptr->elsePart());  
    }

    emitRm(saved_loc2, "LDA", Register::pc, current_line_ - saved_loc2, Register::pc, "jmp to end");
    emitCommentLine("* <- if");
}

void CodeGenerator::generateRepeatStatement(AstPtr node)
{
    RepeatStatementAstPtr ptr = std::dynamic_pointer_cast<RepeatStatementAst>(node);
    if (!ptr) 
    {
        return;   
    }
    emitCommentLine("* -> repeat");
    emitCommentLine("* repeat: jump after body comes back here");
    int saved_loc = current_line_ + 1;
    generateStatementSequence(ptr->bodyPart());
    generateExpression(ptr->testPart());
    emitRm("JEQ", Register::ac, saved_loc - current_line_ - 2, Register::pc, "repeat: jmp back to body");
    emitCommentLine("* <- repeat");
}

void CodeGenerator::generateAssignStatement(AstPtr node)
{
    AssignStatementAstPtr ptr = std::dynamic_pointer_cast<AssignStatementAst>(node);
    if (!ptr) 
    {
        return; 
    }
    emitCommentLine("* -> assign");
    generateExpression(ptr->expression());
    int offset = analyst_.lookupSymbolTable(ptr->variable()->name());
    emitRm("ST", Register::ac, offset, Register::gp, "assign: store value");
    emitCommentLine("* <- assign");
}

void CodeGenerator::generateReadStatement(AstPtr node)
{
    ReadStatementAstPtr ptr = std::dynamic_pointer_cast<ReadStatementAst>(node);
    if (!ptr) 
    {
        return;   
    }
    emitRo("IN", Register::ac, Register::ac, Register::ac, "read integer value");
    int offset = analyst_.lookupSymbolTable(ptr->variable()->name());
    emitRm("ST", Register::ac, offset, Register::gp, "read: store value");
}

void CodeGenerator::generateWriteStatement(AstPtr node)
{
    WriteStatementAstPtr ptr = std::dynamic_pointer_cast<WriteStatementAst>(node);
    if (!ptr) 
    {
        return;   
    }
    generateExpression(ptr->expression());
    emitRo("OUT", Register::ac, Register::ac, Register::ac, "write ac");
}

void CodeGenerator::generateExpression(AstPtr node)
{
    switch (node->getAstType()) 
    {
        case AstType::kVariable:
            generateVariable(node);
            return;

        case AstType::kConstant:
            generateConstant(node);
            return;

        default:
            break;
    }

    ExpressionAstPtr ptr = std::dynamic_pointer_cast<ExpressionAst>(node);
    if (!ptr) 
    {
        return;   
    }

    emitCommentLine("* -> op");
    generateExpression(ptr->leftPart());
    emitRm("ST", Register::ac, tmp_offset_, Register::mp, "op: push left");
    ++tmp_offset_;
    generateExpression(ptr->rightPart());
    --tmp_offset_;
    emitRm("LD", Register::ac1, tmp_offset_, Register::mp, "op: load left");

    switch (ptr->operatorTokenValue()) 
    {
        case TokenValue::kPlus:
            emitRo("ADD", Register::ac, Register::ac1, Register::ac, "op +"); 
            break;

        case TokenValue::kMinus:
            emitRo("SUB", Register::ac, Register::ac1, Register::ac, "op -");
            break;

        case TokenValue::kMultiply:
            emitRo("MUL", Register::ac, Register::ac1, Register::ac, "op *");
            break;

        case TokenValue::kDivide:
            emitRo("DIV", Register::ac, Register::ac1, Register::ac, "op /");
            break;

        case TokenValue::kLess:
            emitRo("SUB", Register::ac, Register::ac1, Register::ac, "op <");
            emitRm("JLT", Register::ac, 2, Register::pc, "br if true");
            emitRm("LDC", Register::ac, 0, Register::ac, "false case");
            emitRm("LDA", Register::pc, 1, Register::pc, "unconditional jmp");
            emitRm("LDC", Register::ac, 1, Register::ac, "true case");
            break;

        case TokenValue::kEqual:
            emitRo("SUB", Register::ac, Register::ac1, Register::ac, "op =");
            emitRm("JEQ", Register::ac, 2, Register::pc, "br if true");
            emitRm("LDC", Register::ac, 0, Register::ac, "false case");
            emitRm("LDA", Register::pc, 1, Register::pc, "unconditional jmp");
            emitRm("LDC", Register::ac, 1, Register::ac, "true case");
            break;

        default:
            errorReport("Invalid operator");
            break;
    }
    emitCommentLine("* <- op");
}

void CodeGenerator::generateVariable(AstPtr node)
{
    VariableAstPtr ptr = std::dynamic_pointer_cast<VariableAst>(node);
    if (!ptr) 
    {
        return;   
    }
    emitCommentLine("* -> Id");
    int offset = analyst_.lookupSymbolTable(ptr->name());    
    emitRm("LD", Register::ac, offset, Register::gp, "load id value");
    emitCommentLine("* <- Id");
}

void CodeGenerator::generateConstant(AstPtr node)
{
    ConstantAstPtr ptr = std::dynamic_pointer_cast<ConstantAst>(node);
    if (!ptr) 
    {
        return;   
    }
    emitCommentLine("* -> Const");
    emitRm("LDC", Register::ac, ptr->intValue(), Register::ac, "load const");
    emitCommentLine("* <- Const");
}

void CodeGenerator::errorReport(const std::string& message)
{
    std::ostringstream o;
    o << current_line_; 
    errorCodeGen(o.str() + message);
}

} // namespace nova
