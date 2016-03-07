#ifndef __NOVA_ERROR_H__
#define __NOVA_ERROR_H__

#include <string>

namespace nova 
{

void errorToken(const std::string& message);
void errorSyntax(const std::string& message);
    
} // namespace nova

#endif
