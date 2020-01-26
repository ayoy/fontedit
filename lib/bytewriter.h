#ifndef BYTEWRITER_H
#define BYTEWRITER_H

#include <cstdint>
#include <string>

/**
 * @brief The byte writer for the conversion algorithm to send the result to.
 */
class ByteWriterInterface
{
public:
    /**
     * @brief Start a new file
     *
     * Call this method once at the begin, before any data is written.
     */
    virtual std::string begin(const std::string &timestamp) const = 0;

    /**
     * @brief Start a new array of bytes
     * @param name The name of the array.
     */
    virtual std::string beginArray(const std::string &name) const = 0;

    /**
     * @brief Start a new row of array of bytes
     */
    virtual std::string beginArrayRow() const = 0;

    /**
     * @brief Write a single byte to the output array
     * @param byte
     */
    virtual std::string byte(uint8_t byte) const = 0;

    /**
     * @brief Add a comment to the output
     * @param comment
     */
    virtual std::string comment(const std::string &comment) const = 0;

    /**
    * @brief Add a linebreak to the output
    */
    virtual std::string lineBreak() const = 0;

    /**
     * @brief End an array of bytes
     */
    virtual std::string endArray() const = 0;

    /**
     * @brief End a file
     *
     * Call this method once at the end, after all data was written.
     */
    virtual std::string end() const = 0;

    virtual ~ByteWriterInterface() = default;
};


class ByteWriter : public ByteWriterInterface
{
public:
    static const std::string identifier;
    static const std::string description;

    std::string beginArrayRow() const override;
    std::string lineBreak() const override;
    std::string end() const override;
};


class CCodeGenerator : public ByteWriter
{
public:
    static const std::string identifier;
    static const std::string description;

    std::string begin(const std::string &timestamp) const override;
    std::string beginArray(const std::string &name) const override;
    std::string byte(uint8_t byte) const override;
    std::string comment(const std::string &comment) const override;
    std::string endArray() const override;
};


class ArduinoCodeGenerator : public CCodeGenerator
{
public:
    static const std::string identifier;
    static const std::string description;

    std::string begin(const std::string &timestamp) const override;
    std::string beginArray(const std::string &name) const override;
};


class PythonListCodeGenerator : public ByteWriter
{
public:
    static const std::string identifier;
    static const std::string description;

    std::string begin(const std::string &timestamp) const override;
    std::string beginArray(const std::string &name) const override;
    std::string byte(uint8_t byte) const override;
    std::string comment(const std::string &comment) const override;
    std::string endArray() const override;
};


class PythonBytesCodeGenerator : public ByteWriter
{
public:
    static const std::string identifier;
    static const std::string description;

    std::string begin(const std::string &timestamp) const override;
    std::string beginArray(const std::string &name) const override;
    std::string beginArrayRow() const override;
    std::string byte(uint8_t byte) const override;
    std::string comment(const std::string &comment) const override;
    std::string endArray() const override;
};

#endif // BYTEWRITER_H
