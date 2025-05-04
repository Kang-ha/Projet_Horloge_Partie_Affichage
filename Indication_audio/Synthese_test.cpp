#include <iostream>
#include <cstdlib>

int main() {
  const char* text = "Bonjour ceci est un test";
  std::string command = "espeak-ng \"" + std::string(text) + "\"";
  system(command.c_str());
  return 0;
}
