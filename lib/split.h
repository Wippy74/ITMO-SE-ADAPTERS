#pragma once

#include "lazy_pipeline.h"
#include "traits.h"

#include <memory>
#include <string>
#include <unordered_set>

template <template <typename, typename...> typename Container, typename... Args>
class SplitAdapter {
private:
  struct DelimSet {
    static const size_t kSize = 1024;
    std::unordered_set<char> delimiters;
    explicit DelimSet(const std::string& delim_string) {
      for (char c : delim_string) {
        delimiters.insert(c);
      }
    }
    bool IsDelimiter(char c) const {
      return delimiters.contains(c);
    }
  };
  std::shared_ptr<DelimSet> delim_set_;
public:
  explicit SplitAdapter(const std::string& delimiters) : delim_set_(std::make_shared<DelimSet>(delimiters)) {}

  template <typename Data>
  requires std::convertible_to<typename Data::Type, std::string>
  auto Apply(Data& flow) const {
    auto local_set = delim_set_;
    return flow.template Make<std::string, Container, Args...>([local_set](auto& input, auto& output) {
                                                              for (std::string& s : input) {
                                                                size_t l = 0;
                                                                for (size_t r = 0; r < s.size(); ++r) {
                                                                  if (local_set->IsDelimiter(s[r])) {
                                                                    if (r > l) {
                                                                      output.push_back(s.substr(l, r - l));
                                                                    }
                                                                    l = r + 1;
                                                                  }
                                                                }
                                                                if (l < s.size()) {
                                                                  output.push_back(s.substr(l));
                                                                }
                                                              } });
  }

  template <typename Data>
  requires std::derived_from<typename Data::Type, std::istream>
  auto Apply(Data& flow) const {
    auto local_set = delim_set_;
    return flow.template Make<std::string, Container, Args...>([local_set](auto& input, auto& output) {
                                                              for (auto& stream : input) {
                                                                  char buff[DelimSet::kSize];
                                                                  std::string w;
                                                                  while(stream) {
                                                                    stream.read(buff, DelimSet::kSize);
                                                                    std::streamsize count = stream.gcount();
                                                                    for (std::streamsize i = 0; i < count; ++i) {
                                                                      if (local_set->IsDelimiter(buff[i])) {
                                                                          if (!w.empty()) {
                                                                            output.push_back(std::move(w));
                                                                            w.clear();
                                                                          }
                                                                      } else {
                                                                        w += buff[i];
                                                                      }
                                                                    }
                                                                  }
                                                                  if (!w.empty()) {
                                                                    output.push_back(std::move(w));
                                                                  }
                                                              } });
  }
};

template <template <typename, typename...> typename Container = SameCont, typename... Args>
auto Split(const std::string& delims) {
  return SplitAdapter<Container, Args...>(delims);
}