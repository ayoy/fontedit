#include "sourcecoderunnable.h"
#include <QElapsedTimer>
#include <QDebug>

void SourceCodeRunnable::run()
{
    QString output;

    QElapsedTimer timer;
    timer.start();
    if (format_ == Format::Arduino::identifier) {
        output = QString::fromStdString(generator_.generate<Format::Arduino>(face_, fontArrayName_));
    } else if (format_ == Format::C::identifier) {
        output = QString::fromStdString(generator_.generate<Format::C>(face_, fontArrayName_));
    } else if (format_ == Format::PythonList::identifier) {
        output = QString::fromStdString(generator_.generate<Format::PythonList>(face_, fontArrayName_));
    } else if (format_ == Format::PythonBytes::identifier) {
        output = QString::fromStdString(generator_.generate<Format::PythonBytes>(face_, fontArrayName_));
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
