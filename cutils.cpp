#include <sstream>
#include "cutils.h"

std::string CUtils::formatSdkResult(const int32_t code, const std::string& msg, const std::string& head)
{
    std::ostringstream ostr;
    ostr << head << ", error_code: " << code << ", msg: " << msg;
    return ostr.str();
}
