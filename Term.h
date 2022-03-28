#ifndef MNOGOCHLEN__TERM_H_
#define MNOGOCHLEN__TERM_H_

#include <string>
#include <vector>
#include <map>

class Term {
 public:
  int sign;
  float coeff;
  std::map<char, int> power;
  Term();
};

#endif //MNOGOCHLEN__TERM_H_
