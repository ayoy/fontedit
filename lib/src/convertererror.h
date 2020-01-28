#ifndef CONVERSIONERROR_H
#define CONVERSIONERROR_H

#include <string>

struct ConverterError
{
    static const ConverterError NoError;

    ConverterError(std::string summary = "Unknown error",
                   std::string description = "") :
        summary(std::move(summary)),
        description(std::move(description))
    {}

    ConverterError & operator=(std::string summary);
    bool operator ==(const ConverterError &rhs) const;
    bool operator !=(const ConverterError &rhs) const;


    std::string summary;
    std::string description;
};

#endif // CONVERSIONERROR_H
