#include "Functions.h"
#include <iostream>
#include <sstream>

bool FileExists(const sf::String &fileName) {
    std::ifstream infile(fileName);
    return infile.good();
}