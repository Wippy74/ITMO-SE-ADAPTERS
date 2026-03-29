#pragma once

#include "write.h"
#include <ostream>

class OutAdapter : public WriteAdapter {
public:
  OutAdapter(std::ostream& out) : WriteAdapter(out, '\n') {}
};

inline OutAdapter Out(std::ostream& out) {
  return OutAdapter(out);
}