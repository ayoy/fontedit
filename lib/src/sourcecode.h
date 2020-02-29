#ifndef SOURCECODE_H
#define SOURCECODE_H

#include <string>
#include <iostream>
#include <variant>

namespace SourceCode
{

struct Tab {};
struct Space { std::size_t num_spaces; };

using TabWidth = std::variant<Tab,Space>;

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
struct BeginArrayRow {
    TabWidth tab;
};

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


inline std::ostream& operator<<(std::ostream& o, const SourceCode::TabWidth& t) {
    if (std::holds_alternative<SourceCode::Space>(t)) {
        o << std::string(std::get<SourceCode::Space>(t).num_spaces, ' ');
    } else {
        o << "\t";
    }
    return o;
}

#endif // SOURCECODE_H
