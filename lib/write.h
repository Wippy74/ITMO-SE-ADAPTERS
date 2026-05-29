#pragma once

#include <string>
#include <ostream>

class WriteAdapter {
public:
  WriteAdapter(std::ostream& out, const std::string& delim) : out_(out), delim_(delim) {};
  WriteAdapter(std::ostream& out, char delim) : out_(out), delim_(std::string(1, delim)) {};
  template <typename Data>
  Data Apply(Data& data) {
    for (const auto& d: data.Access()) {
      out_ << d << delim_;
    }
    return data;
  }
private:
  std::ostream& out_;
  std::string delim_;
};

template <typename Delimeter>
auto Write(std::ostream& out, Delimeter delim) {
  return WriteAdapter(out, delim);
}