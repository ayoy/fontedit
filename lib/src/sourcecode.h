#ifndef SOURCECODE_H
#define SOURCECODE_H

#include "fontdata.h"
#include <string>
#include <iostream>
#include <variant>

namespace SourceCode
{

struct Tab {};
struct Space { std::size_t num_spaces; };

using Indentation = std::variant<Tab,Space>;

namespace Idiom {

template<typename T>
struct Begin {
    std::string font_name;
    Font::Size font_size;
    std::string timestamp;
};

template<typename T>
struct BeginArray {
    std::string array_name;
};

template<typename T>
struct BeginArrayRow {
    Indentation tab;
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


inline std::ostream& operator<<(std::ostream& o, const SourceCode::Indentation& t) {
    if (std::holds_alternative<SourceCode::Space>(t)) {
        o << std::string(std::get<SourceCode::Space>(t).num_spaces, ' ');
    } else {
        o << "\t";
    }
    return o;
}

inline bool operator==(const SourceCode::Indentation& lhs, const SourceCode::Indentation& rhs) {
    if (std::holds_alternative<SourceCode::Tab>(lhs) && std::holds_alternative<SourceCode::Tab>(rhs)) {
        return true;
    }
    if (std::holds_alternative<SourceCode::Space>(lhs) && std::holds_alternative<SourceCode::Space>(rhs)) {
        return std::get<SourceCode::Space>(lhs).num_spaces == std::get<SourceCode::Space>(rhs).num_spaces;
    }
    return false;
}

#endif // SOURCECODE_H
