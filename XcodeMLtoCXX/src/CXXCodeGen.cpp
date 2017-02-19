#include <cassert>
#include <sstream>
#include <string>

#include "CXXCodeGen.h"

namespace CXXCodeGen {

const space_t space = {};

const newline_t newline = {};

const indent_t indent;

const indent_t unindent;

std::string Stream::str() {
  return  ss.str();
}

Stream& Stream::operator <<(const space_t&) {
  const std::string separaters = "\n\t ";
  if (separaters.find(lastChar) != std::string::npos) {
    ss << " ";
    lastChar = " ";
  }
  return *this;
}

}
