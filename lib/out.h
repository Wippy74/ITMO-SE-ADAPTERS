#pragma once

#include "write.h"
#include <ostream>

class OutAdapt : public WriteAdapt {
public:
  OutAdapt(std::ostream& out) : WriteAdapt(out, '\n') {}
};

inline OutAdapt Out(std::ostream& out) {
  return OutAdapt(out);
}