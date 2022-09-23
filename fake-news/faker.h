#ifndef FAKER_H_
#define FAKER_H_
#include "http.h"
#include <string>

class Faker {
public: /* comment ************************************************************/
  void alter_keywords(HttpResponse &reg);
  void alter_images(HttpResponse &re);
};

void replace_all(std::string &in, std::string &&match, std::string &&replace);
#endif // FAKER_H_
