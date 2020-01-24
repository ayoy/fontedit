#ifndef SOURCECODEGENERATOR_H
#define SOURCECODEGENERATOR_H

#include "bytewriter.h"
#include <sstream>
#include <ctime>
#include <memory>

struct SourceCodeOptions
{
    enum BitNumbering { LSB, MSB };


    SourceCodeOptions() {}
    SourceCodeOptions(BitNumbering bitNumbering, bool shouldInvertBits) :
        bitNumbering(bitNumbering),
        shouldInvertBits(shouldInvertBits)
    {}

    BitNumbering bitNumbering { LSB };
    bool shouldInvertBits { false };
};

class SourceCodeGeneratorInterface {
public:
    virtual void begin() = 0;
    virtual void beginArray(const std::string &name) = 0;
    virtual void beginArrayRow() = 0;
    virtual void writeByte(uint8_t byte) = 0;
    virtual void addComment(const std::string &comment) = 0;
    virtual void addLineBreak() = 0;
    virtual void endArray() = 0;
    virtual void end() = 0;
    virtual std::string sourceCode() = 0;
    virtual ~SourceCodeGeneratorInterface() = default;
};

template<typename T>
class SourceCodeGenerator: public SourceCodeGeneratorInterface
{
public:
    explicit SourceCodeGenerator(SourceCodeOptions options);

    SourceCodeGenerator(SourceCodeGenerator &&other) = default;

    virtual ~SourceCodeGenerator() = default;

    inline std::string sourceCode() override { return m_stream->str(); }

    void begin() override;
    void beginArray(const std::string &name) override;
    void beginArrayRow() override;
    void writeByte(uint8_t byte) override;
    void addComment(const std::string &comment) override;
    void addLineBreak() override;
    void endArray() override;
    void end() override;

protected:
    inline std::ostringstream &stream() { return *(m_stream.get()); }
    const SourceCodeOptions options;

    std::string getCurrentTimestamp() const;
    uint8_t formatByte(uint8_t byte) const;

private:
    std::unique_ptr<T> m_bytewriter;
    std::unique_ptr<std::ostringstream> m_stream;
};

template<typename T>
SourceCodeGenerator<T>::SourceCodeGenerator(SourceCodeOptions options):
        options(std::move(options)),
        m_bytewriter(std::make_unique<T>()),
        m_stream(std::make_unique<std::ostringstream>())
{}

template<typename T>
std::string SourceCodeGenerator<T>::getCurrentTimestamp() const
{
    time_t     now = time(nullptr);
    struct tm  tstruct;
    char       buf[23];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%d/%m/%Y at %H:%M:%S", &tstruct);

    return buf;
}

template<typename T>
uint8_t SourceCodeGenerator<T>::formatByte(uint8_t byte) const
{
    if (options.bitNumbering == SourceCodeOptions::MSB) {
        uint8_t reversedByte = 0;
        for (uint8_t i = 0; i < 8; i++) {
            if ((byte & (1<<i)) != 0) {
                uint8_t setBit = static_cast<uint8_t>(1<<(7-i));
                reversedByte |= setBit;
            }
        }
        byte = reversedByte;
    }
    if (options.shouldInvertBits) {
        byte = ~byte;
    }
    return byte;
}

template<typename T>
void SourceCodeGenerator<T>::begin()
{
    m_stream->flush();
    *m_stream << m_bytewriter->begin(std::move(getCurrentTimestamp()));
}

template<typename T>
void SourceCodeGenerator<T>::beginArray(const std::string &name)
{
    *m_stream << m_bytewriter->beginArray(name);
}

template<typename T>
void SourceCodeGenerator<T>::beginArrayRow()
{
    *m_stream << m_bytewriter->beginArrayRow();
}

template<typename T>
void SourceCodeGenerator<T>::writeByte(uint8_t byte)
{
    *m_stream << m_bytewriter->byte(formatByte(byte));
}

template<typename T>
void SourceCodeGenerator<T>::addComment(const std::string &comment)
{
    *m_stream << m_bytewriter->comment(comment);
}

template<typename T>
void SourceCodeGenerator<T>::addLineBreak()
{
    *m_stream << m_bytewriter->lineBreak();
}

template<typename T>
void SourceCodeGenerator<T>::endArray()
{
    *m_stream << m_bytewriter->endArray();
}

template<typename T>
void SourceCodeGenerator<T>::end()
{
    *m_stream << m_bytewriter->end();
}

#endif // SOURCECODEGENERATOR_H
