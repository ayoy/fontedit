#include "sourcecoderunnable.h"

void SourceCodeRunnable::run()
{
    QString output;

    if (format_ == Format::Arduino::identifier) {
        output = QString::fromStdString(generator_.generate<Format::Arduino>(face_));
    } else if (format_ == Format::C::identifier) {
        output = QString::fromStdString(generator_.generate<Format::C>(face_));
    } else if (format_ == Format::PythonList::identifier) {
        output = QString::fromStdString(generator_.generate<Format::PythonList>(face_));
    } else if (format_ == Format::PythonBytes::identifier) {
        output = QString::fromStdString(generator_.generate<Format::PythonBytes>(face_));
    }

    handler_(output);
}
