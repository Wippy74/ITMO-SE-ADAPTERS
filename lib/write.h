#pragma once

#include <string>
#include <ostream>

class WriteAdapt {
public:
  virtual ~WriteAdapt() = default;
  WriteAdapt(std::ostream& out, const std::string& delim) : out_(out), delim_(delim) {};
  WriteAdapt(std::ostream& out, char delim) : out_(out), delim_(std::string(1, delim)) {};
  template <typename Data>
  Data Apply(Data& data) {
    for (//TODO const auto& d: data ?????) {
      out_ << d << delim_;
    }
    return Data;
  }
private:
  std::ostream& out_;
  std::string delim_;
};

template <typename Delimeter>
auto Write(std::ostream& out, Delimeter delim) {
  return WriteAdapt(out, delim);
};