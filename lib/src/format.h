#ifndef FORMAT_H
#define FORMAT_H

#include <string>
#include <iostream>
#include <iomanip>
#include "sourcecode.h"


namespace Format
{

using namespace SourceCode;

struct c_based {};
struct python_based {};

struct C
{
    using lang = c_based;
    static constexpr std::string_view identifier = "c";
};

struct Arduino
{
    using lang = c_based;
    static constexpr std::string_view identifier = "arduino";
};


struct PythonList
{
    using lang = python_based;
    static constexpr std::string_view identifier = "python-list";
};


struct PythonBytes
{
    using lang = python_based;
    static constexpr std::string_view identifier = "python-bytes";
};


constexpr auto available_formats = {
    C::identifier,
    Arduino::identifier,
    PythonList::identifier,
    PythonBytes::identifier
};

}


template<typename T, typename = void>
struct is_c_based : std::false_type {};
template<typename T>
struct is_c_based<T, std::enable_if_t<std::is_same<typename T::lang, Format::c_based>::value>> : std::true_type {};

template<typename T, typename = void>
struct is_python_based : std::false_type {};
template<typename T>
struct is_python_based<T, std::enable_if_t<std::is_same<typename T::lang, Format::python_based>::value>> : std::true_type {};

static_assert (is_c_based<Format::C>::value, "***");
static_assert (is_c_based<Format::Arduino>::value, "***");
static_assert (!is_c_based<Format::PythonList>::value, "***");
static_assert (!is_c_based<Format::PythonBytes>::value, "***");

static_assert (!is_python_based<Format::C>::value, "***");
static_assert (!is_python_based<Format::Arduino>::value, "***");
static_assert (is_python_based<Format::PythonList>::value, "***");
static_assert (is_python_based<Format::PythonBytes>::value, "***");


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
    } else if constexpr (is_python_based<T>::value) {
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
          << static_cast<uint>(b.byte);
    } else {
        s << "0x"
          << std::setw(2)
          << std::setfill('0')
          << std::uppercase
          << std::hex
          << static_cast<uint>(b.byte)
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
