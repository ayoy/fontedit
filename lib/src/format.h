#ifndef FORMAT_H
#define FORMAT_H

#include <string>
#include <iostream>
#include <iomanip>
#include "sourcecode.h"


/// This namespace defines available source code formats
namespace Format
{

using namespace SourceCode;

/// A type that identifies C-based language format.
struct c_based {};
/// A type that identifies Python language format.
struct python {};

/// C-style code (suitable also for C++)
struct C
{
    using lang = c_based;
    static constexpr std::string_view identifier = "c";
};

/// Arduino-flavoured C-style code
struct Arduino
{
    using lang = c_based;
    static constexpr std::string_view identifier = "arduino";
};

/// Python code format for List object
struct PythonList
{
    using lang = python;
    static constexpr std::string_view identifier = "python-list";
};

/// Python code format for Bytes object
struct PythonBytes
{
    using lang = python;
    static constexpr std::string_view identifier = "python-bytes";
};


constexpr auto available_formats = {
    C::identifier,
    Arduino::identifier,
    PythonList::identifier,
    PythonBytes::identifier
};

}

/// Type trait that is true for C-based language formats
template<typename T, typename = void>
struct is_c_based : std::false_type {};
template<typename T>
struct is_c_based<T, std::enable_if_t<std::is_same<typename T::lang, Format::c_based>::value>> : std::true_type {};

/// Type trait that is true for Python language formats
template<typename T, typename = void>
struct is_python : std::false_type {};
template<typename T>
struct is_python<T, std::enable_if_t<std::is_same<typename T::lang, Format::python>::value>> : std::true_type {};

static_assert (is_c_based<Format::C>::value, "***");
static_assert (is_c_based<Format::Arduino>::value, "***");
static_assert (!is_c_based<Format::PythonList>::value, "***");
static_assert (!is_c_based<Format::PythonBytes>::value, "***");

static_assert (!is_python<Format::C>::value, "***");
static_assert (!is_python<Format::Arduino>::value, "***");
static_assert (is_python<Format::PythonList>::value, "***");
static_assert (is_python<Format::PythonBytes>::value, "***");


//
// The code below defines operator<< for all structs from the
// SourceCode::Idiom namespace. These functions use type traits
// and compile-time ifs and to improve source code generation
// performance.
//

// Begin

template<typename T>
inline std::ostream& operator<<(std::ostream& s, SourceCode::Idiom::Begin<T> b)
{
    if constexpr (is_c_based<T>::value) {
        s << "//\n// " << b.font_name << "\n"
          << "// Font Size: " << b.font_size.width << "x" << b.font_size.height << "px\n"
          << "// Created: " << b.timestamp << "\n//\n";
        if constexpr (std::is_same<T, Format::Arduino>::value) {
            s << "\n#include <Arduino.h>\n";
        }
    } else if constexpr (is_python<T>::value) {
        s << "#\n# " << b.font_name << "\n"
          << "# Font Size: " << b.font_size.width << "x" << b.font_size.height << "px\n"
          << "# Created: " << b.timestamp << "\n#\n";
    }
    return s;
}


// BeginArray

template<typename T>
inline std::ostream& operator<<(std::ostream& s, SourceCode::Idiom::BeginArray<T> b)
{
    if constexpr (std::is_same<T, Format::C>::value) {
        s << "\n\nconst unsigned char " << b.array_name << "[] = {\n";
    } else if constexpr (std::is_same<T, Format::Arduino>::value) {
        s << "\n\nconst uint8_t " << b.array_name << "[] PROGMEM = {\n";
    } else if constexpr (std::is_same<T, Format::PythonList>::value) {
        s << "\n\n" << b.array_name << " = [\n";
    } else if constexpr (std::is_same<T, Format::PythonBytes>::value) {
        s << "\n\n" << b.array_name << " = b'' \\\n";
    }
    return s;
}


// BeginArrayRow

template<typename T>
inline std::ostream& operator<<(std::ostream& s, SourceCode::Idiom::BeginArrayRow<T> b)
{
    if constexpr (std::is_same<T, Format::PythonBytes>::value) {
        s << b.tab << "b'";
    } else {
        s << b.tab;
    }
    return s;
}


// Byte

template<typename T>
inline std::ostream& operator<<(std::ostream& s, SourceCode::Idiom::Byte<T> b)
{
    if constexpr (std::is_same<T, Format::PythonBytes>::value) {
        s << "\\x"
          << std::setw(2)
          << std::setfill('0')
          << std::uppercase
          << std::hex
          << static_cast<unsigned>(b.byte);
    } else {
        s << "0x"
          << std::setw(2)
          << std::setfill('0')
          << std::uppercase
          << std::hex
          << static_cast<unsigned>(b.byte)
          << ",";
    }
    return s;
}


// Comment

template<typename T>
inline std::ostream& operator<<(std::ostream& s, SourceCode::Idiom::Comment<T> b)
{
    if constexpr (is_c_based<T>::value) {
        s << " // " << b.comment;
    } else if constexpr (std::is_same<T, Format::PythonList>::value) {
        s << " # " << b.comment;
    }
    return s;
}


// LineBreak

template<typename T>
inline std::ostream& operator<<(std::ostream& s, SourceCode::Idiom::LineBreak<T>)
{
    if constexpr (std::is_same<T, Format::PythonBytes>::value) {
        s << "' \\\n";
    } else {
        s << "\n";
    }
    return s;
}


// EndArray

template<typename T>
inline std::ostream& operator<<(std::ostream& s, SourceCode::Idiom::EndArray<T>)
{
    if constexpr (is_c_based<T>::value) {
        s << "};\n";
    } else if constexpr (std::is_same<T, Format::PythonList>::value) {
        s << "]\n";
    }
    return s;
}


// End

template<typename T>
inline std::ostream& operator<<(std::ostream& s, SourceCode::Idiom::End<T>)
{
    s << "\n\n";
    return s;
}


#endif // FORMAT_H
