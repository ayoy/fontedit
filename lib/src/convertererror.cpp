#include "convertererror.h"

const ConverterError ConverterError::NoError = { "No error" };

ConverterError & ConverterError::operator =(std::string summary)
{
    this->summary = std::move(summary);
    description = "";
    return *this;
}

bool ConverterError::operator ==(const ConverterError &rhs) const
{
    return summary == rhs.summary and description == rhs.description;
}

bool ConverterError::operator !=(const ConverterError &rhs) const
{
    return !(*this == rhs);
}
