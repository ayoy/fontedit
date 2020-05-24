#include "sourcecoderunnable.h"
#include <QElapsedTimer>
#include <QDebug>

void SourceCodeRunnable::run()
{
    QString output;

    QElapsedTimer timer;
    timer.start();
    if (format_ == f2b::Format::Arduino::identifier) {
        output = QString::fromStdString(generator_.generate<f2b::Format::Arduino>(face_, fontArrayName_));
    } else if (format_ == f2b::Format::C::identifier) {
        output = QString::fromStdString(generator_.generate<f2b::Format::C>(face_, fontArrayName_));
    } else if (format_ == f2b::Format::PythonList::identifier) {
        output = QString::fromStdString(generator_.generate<f2b::Format::PythonList>(face_, fontArrayName_));
    } else if (format_ == f2b::Format::PythonBytes::identifier) {
        output = QString::fromStdString(generator_.generate<f2b::Format::PythonBytes>(face_, fontArrayName_));
    }
    qDebug() << "Generation finished in" << timer.elapsed() << "ms";

    setFinished(true);
    if (!isCanceled()) {
        handler_(output);
    }
}

bool SourceCodeRunnable::isFinished()
{
    std::scoped_lock lock { mutex_ };
    return m_finished;
}

bool SourceCodeRunnable::isCanceled()
{
    std::scoped_lock lock { mutex_ };
    return m_canceled;
}

void SourceCodeRunnable::setCanceled(bool canceled)
{
    std::scoped_lock lock { mutex_ };
    m_canceled = canceled;
}

void SourceCodeRunnable::setFinished(bool finished)
{
    std::scoped_lock lock { mutex_ };
    m_finished = finished;
}
