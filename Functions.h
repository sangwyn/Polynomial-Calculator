#ifndef MNOGOCHLEN__FUNCTIONS_H_
#define MNOGOCHLEN__FUNCTIONS_H_

#include <string>
#include <sstream>
#include <fstream>
#include <SFML/Graphics.hpp>

template <typename T>

std::string ToStr (const T & t) {
    std::ostringstream os;
    os << t;
    return os.str ();
}

bool FileExists(const sf::String &fileName);

#endif //MNOGOCHLEN__FUNCTIONS_H_
