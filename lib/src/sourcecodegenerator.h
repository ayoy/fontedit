#ifndef SOURCECODEGENERATOR_H
#define SOURCECODEGENERATOR_H

#include "bytewriter.h"
#include <sstream>
#include <ctime>
#include <memory>

struct SourceCodeOptions
{
    enum BitNumbering { LSB, MSB };

    BitNumbering bit_numbering { LSB };
    bool invert_bits { false };
};

class SourceCodeGeneratorInterface {
public:
    virtual void begin() = 0;
    virtual void begin_array(const std::string &name) = 0;
    virtual void begin_array_row() = 0;
    virtual void write_byte(uint8_t byte) = 0;
    virtual void add_comment(const std::string &comment) = 0;
    virtual void add_line_break() = 0;
    virtual void end_array() = 0;
    virtual void end() = 0;
    virtual std::string source_code() = 0;
    virtual ~SourceCodeGeneratorInterface() = default;
};

template<typename T>
class SourceCodeGenerator: public SourceCodeGeneratorInterface
{
public:
    explicit SourceCodeGenerator(SourceCodeOptions options);

    SourceCodeGenerator(SourceCodeGenerator &&other) = default;

    virtual ~SourceCodeGenerator() = default;

    std::string source_code() override { return stream_.str(); }

    void begin() override;
    void begin_array(const std::string &name) override;
    void begin_array_row() override;
    void write_byte(uint8_t byte) override;
    void add_comment(const std::string &comment) override;
    void add_line_break() override;
    void end_array() override;
    void end() override;

protected:
    std::ostringstream& stream() { return stream_; }
    const SourceCodeOptions options;

    std::string get_current_timestamp() const;
    uint8_t format_byte(uint8_t byte) const;

private:
    std::unique_ptr<T> bytewriter_ { std::make_unique<T>() };
    std::ostringstream stream_ {};
};

template<typename T>
SourceCodeGenerator<T>::SourceCodeGenerator(SourceCodeOptions options):
        options(std::move(options))
{}

template<typename T>
std::string SourceCodeGenerator<T>::get_current_timestamp() const
{
    time_t     now = time(nullptr);
    struct tm  tstruct;
    char       buf[23];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%d/%m/%Y at %H:%M:%S", &tstruct);

    return buf;
}

template<typename T>
uint8_t SourceCodeGenerator<T>::format_byte(uint8_t byte) const
{
    if (options.bit_numbering == SourceCodeOptions::MSB) {
        uint8_t reversedByte = 0;
        for (uint8_t i = 0; i < 8; i++) {
            if ((byte & (1<<i)) != 0) {
                uint8_t setBit = static_cast<uint8_t>(1<<(7-i));
                reversedByte |= setBit;
            }
        }
        byte = reversedByte;
    }
    if (options.invert_bits) {
        byte = ~byte;
    }
    return byte;
}

template<typename T>
void SourceCodeGenerator<T>::begin()
{
    stream_.flush();
    stream_ << bytewriter_->begin(std::move(get_current_timestamp()));
}

template<typename T>
void SourceCodeGenerator<T>::begin_array(const std::string &name)
{
    stream_ << bytewriter_->begin_array(name);
}

template<typename T>
void SourceCodeGenerator<T>::begin_array_row()
{
    stream_ << bytewriter_->begin_array_row();
}

template<typename T>
void SourceCodeGenerator<T>::write_byte(uint8_t byte)
{
    stream_ << bytewriter_->byte(format_byte(byte));
}

template<typename T>
void SourceCodeGenerator<T>::add_comment(const std::string &comment)
{
    stream_ << bytewriter_->comment(comment);
}

template<typename T>
void SourceCodeGenerator<T>::add_line_break()
{
    stream_ << bytewriter_->lineBreak();
}

template<typename T>
void SourceCodeGenerator<T>::end_array()
{
    stream_ << bytewriter_->end_array();
}

template<typename T>
void SourceCodeGenerator<T>::end()
{
    stream_ << bytewriter_->end();
}

#endif // SOURCECODEGENERATOR_H
