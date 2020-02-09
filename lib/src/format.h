#ifndef FORMAT_H
#define FORMAT_H

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "sourcecode.h"

namespace Format
{

using namespace SourceCode;

struct C
{
    static constexpr std::string_view identifier = "C/C++";

    template<Idiom I>
    static std::string output(const Element::Byte<I>& e) {
        std::stringstream s;

        s << "0x"
          << std::setw(2)
          << std::setfill('0')
          << std::uppercase
          << std::hex
          << static_cast<uint>(e.byte)
          << ",";

        return s.str();
    }

    template<Idiom I>
    static std::string output(const Element::Bare<I>& e) {
        (void)e;
        std::stringstream s;

        switch (I) {
        case IdiomBeginArrayRow:
            s << "\t";
            break;
        case IdiomLineBreak:
            s << "\n";
            break;
        case IdiomEndArray:
            s << "};\n";
            break;
        case IdiomEnd:
            s << "\n\n";
            break;
        default:
            break;
        }

        return s.str();
    }

    template<Idiom I>
    static std::string output(const Element::String<I>& e) {
        std::stringstream s;

        switch (I) {
        case IdiomBegin:
            s << "//\n// Font Data\n// Created: " << e.arg << "\n//\n";
            break;
        case IdiomBeginArray:
            s << "\n\nconst unsigned char " << e.arg << "[] = {\n";
            break;
        case IdiomComment:
            s << " // " << e.arg;
            break;
        default:
            break;
        }

        return s.str();
    }
};

}

#endif // FORMAT_H
