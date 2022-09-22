#ifndef FAKER_H_
#define FAKER_H_
#include "http.h"

class Faker {
public: /* comment ************************************************************/
  void alter_keywords(HttpResponse &reg);
  void alter_images(HttpResponse &re);
};
#endif // FAKER_H_
