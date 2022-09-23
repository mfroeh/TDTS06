#include "faker.h"
#include <iostream>
#include <regex>
#include <string>

using namespace std;

void replace_all(string &str, string &&replace, string &&replacement);

void Faker::alter_keywords(HttpResponse &reg) {
  replace_all(reg.raw, " Smiley", " Trolly");
  replace_all(reg.raw, " Stockholm", " Linköping");
  // regex_replace(reg.raw, regex{"(?<!=\")Smiley"}, "Trolly");
  // regex_replace(reg.raw, regex{"(?<!=\")Stockholm"}, "Linköping");
}

void Faker::alter_images(HttpResponse &re) {
  replace_all(re.raw, "smiley.jpg", "trolly.jpg");
}

void replace_all(string &str, string &&replace, string &&replacement) {
  int search_from{0};
  int pos{};
  while ((pos = str.find(replace, search_from)) != string::npos) {
    cout << "[Faker] Replacing " << str.substr(pos, replace.length())
         << " with " << replacement << endl;
    str.replace(pos, replace.length(), replacement);
    search_from = pos + replacement.length();
  }
}
