#include "bytewriter.h"

const std::string ByteWriter::identifier = "";
const std::string ByteWriter::description = "";


const std::string ByteWriter::beginArrayRow() const
{
    return "\t";
}

const std::string ByteWriter::lineBreak() const
{
    return "\n";
}

const std::string ByteWriter::end() const
{
    return "\n\n";
}


//
// CCodeGenerator
//

const std::string CCodeGenerator::identifier = "c";
const std::string CCodeGenerator::description = "C/C++ unsigned char array";

std::string CCodeGenerator::begin(const std::string &timestamp) const
{
    return "//\n// Font Data\n// Created: " + timestamp + "\n//\n";
}

std::string CCodeGenerator::beginArray(const std::string &name) const
{
    return "\n\nconst unsigned char " + name + "[] = {\n";
}

std::string CCodeGenerator::byte(uint8_t byte) const
{
    constexpr char format[] { "0x%02X," };
    std::string byteString(5, '\0');
    std::sprintf(&byteString[0], format, byte);

    return byteString;
}

std::string CCodeGenerator::comment(const std::string &comment) const
{
    return " // " + comment;
}

const std::string CCodeGenerator::endArray() const
{
    return "};\n";
}


//
// ArduinoCodeGenerator
//

const std::string ArduinoCodeGenerator::identifier = "arduino";
const std::string ArduinoCodeGenerator::description = "Arduino-compatible array";

std::string ArduinoCodeGenerator::begin(const std::string &timestamp) const
{
    return CCodeGenerator::begin(timestamp) + "\n#include <Arduino.h>\n";
}

std::string ArduinoCodeGenerator::beginArray(const std::string &name) const
{
    return "\n\nconst uint8_t " + name + "[] PROGMEM = {\n";
}


//
// PythonListCodeGenerator
//

const std::string PythonListCodeGenerator::identifier = "pylist";
const std::string PythonListCodeGenerator::description = "Python list";


std::string PythonListCodeGenerator::begin(const std::string &timestamp) const
{
    return "#\n# Font Data\n# Created: " + timestamp + "\n#\n";
}

std::string PythonListCodeGenerator::beginArray(const std::string &name) const
{
    return "\n\n" + name + " = [\n";
}

std::string PythonListCodeGenerator::byte(uint8_t byte) const
{
    constexpr char format[] { "0x%02X," };
    std::string byteString(5, '\0');
    std::sprintf(&byteString[0], format, byte);

    return byteString;
}

std::string PythonListCodeGenerator::comment(const std::string &comment) const
{
    return " # " + comment;
}

const std::string PythonListCodeGenerator::endArray() const
{
    return "\n]\n";
}


//
// PythonBytesCodeGenerator
//

const std::string PythonBytesCodeGenerator::identifier = "pybytes";
const std::string PythonBytesCodeGenerator::description = "Python bytes object";

std::string PythonBytesCodeGenerator::begin(const std::string &timestamp) const
{
    return "#\n# Font Data\n# Created: " + timestamp + "\n#\n";
}

std::string PythonBytesCodeGenerator::beginArray(const std::string &name) const
{
    return "\n\n" + name + " = b'' \\\n";
}

const std::string PythonBytesCodeGenerator::beginArrayRow() const
{
    return "\t'";
}

std::string PythonBytesCodeGenerator::byte(uint8_t byte) const
{
    const auto format = "\\x%02X";
    std::string byteString(4, '\0');
    std::sprintf(&byteString[0], format, byte);

    return byteString;
}

std::string PythonBytesCodeGenerator::comment(const std::string &comment) const
{
    (void)comment;
    return "' \\";
}

const std::string PythonBytesCodeGenerator::endArray() const
{
    return "";
}
