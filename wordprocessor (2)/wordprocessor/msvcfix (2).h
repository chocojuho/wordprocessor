#if ((_MSC_VER) <= 1600)
#include <string>
#define nullptr NULL

int stoi(std::string s);

std::string to_string(int value);

#endif
