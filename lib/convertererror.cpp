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
    return (summary.compare(rhs.summary) == 0) and (description.compare(rhs.description) == 0);
}

bool ConverterError::operator !=(const ConverterError &rhs) const
{
    return !(*this == rhs);
}
