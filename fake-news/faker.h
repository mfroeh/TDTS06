#ifndef FAKER_H_
#define FAKER_H_
#include "http.h"
#include <string>

// This class contains methods for manipulating HttpResponses.
class Faker {
public:
  // Alters keywords by replacing Smiley with Trolly and Stockholm with
  // Linköping.
  void alter_keywords(HttpResponse &reg);
  // Alters embedded or linked images by replacing ./smiley.jpg with
  // ./trolly.jpg.
  void alter_images(HttpResponse &re);
};

// Replaces all occurances of 'match' with 'replace' inside of 'in'.
void replace_all(std::string &in, std::string &&match, std::string &&replace);
#endif // FAKER_H_
