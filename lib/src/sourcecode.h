#ifndef SOURCECODE_H
#define SOURCECODE_H

#include <string>

namespace SourceCode
{

namespace Idiom {

template<typename T>
struct Begin {
    std::string font_name;
    std::string timestamp;
};

template<typename T>
struct BeginArray {
    std::string array_name;
};

template<typename T>
struct BeginArrayRow {};

template<typename T>
struct Byte {
    uint8_t byte;
};

template<typename T>
struct Comment {
    std::string comment;
};

template<typename T>
struct LineBreak {};

template<typename T>
struct EndArray {};

template<typename T>
struct End {};

};

}

#endif // SOURCECODE_H
