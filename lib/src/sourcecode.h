#ifndef SOURCECODE_H
#define SOURCECODE_H

#include "fontdata.h"
#include <string>
#include <iostream>
#include <variant>

namespace f2b  {

namespace source_code {

/// A struct representing a Tabulation character
struct tab {};

/// A struct representing a repeated space character (used instead of tabulator)
struct space { std::size_t num_spaces; };

/// Indentation can be either a Tab, or multiple Spaces.
using indentation = std::variant<tab,space>;

/**
 * This namespace gathers building blocks for a source code generator:
 * - begin (source code file)
 * - begin array
 * - begin array row
 * - constant definition
 * - value (e.g. byte)
 * - comment
 * - line break with an array
 * - end array
 * - end (source code file).
 *
 * All the structs in this namespace are templates taking Source Code Format
 * as a template parameter. They also take parameters as necessary (e.g.
 * \c array_name for \c BeginArray).
 * All the structs define the \c operator<< which outputs the relevant source
 * code idiom to the given output stream. \c FontSourceCodeGenerator uses them
 * with an \c std::stringstream to output the resulting source code of the font face.
 */

namespace idiom {

template<typename T>
struct begin {
    std::string font_name;
    font::glyph_size font_size;
    std::string timestamp;
};

template<typename T, typename V>
struct constant {
    std::string name;
    V value;
};

template<typename T, typename V>
struct begin_array {
    std::string array_name;
};

template<typename T, typename V>
struct begin_array_row {
    indentation tab;
};

template<typename T, typename V>
struct value {
    V value;
};

template<typename T, typename V = void>
struct comment {
    std::string comment;
};

template<typename T, typename V = void>
struct array_line_break {};

template<typename T, typename V = void>
struct end_array {};

template<typename T>
struct end {};

} // namespace Idiom

inline bool operator==(const indentation& lhs, const indentation& rhs) {
    if (std::holds_alternative<tab>(lhs) && std::holds_alternative<tab>(rhs)) {
        return true;
    }
    if (std::holds_alternative<space>(lhs) && std::holds_alternative<space>(rhs)) {
        return std::get<space>(lhs).num_spaces == std::get<space>(rhs).num_spaces;
    }
    return false;
}

inline std::ostream& operator<<(std::ostream& o, const indentation& t) {
    if (std::holds_alternative<space>(t)) {
        o << std::string(std::get<space>(t).num_spaces, ' ');
    } else {
        o << "\t";
    }
    return o;
}

} // namespace SourceCode
} // namespace f2b

#endif // SOURCECODE_H
