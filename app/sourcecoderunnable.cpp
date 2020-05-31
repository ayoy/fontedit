#include "sourcecoderunnable.h"
#include <QElapsedTimer>
#include <QDebug>

void SourceCodeRunnable::run()
{
    QString output;

    QElapsedTimer timer;
    timer.start();
    if (format_ == f2b::format::arduino::identifier) {
        output = QString::fromStdString(generator_.generate<f2b::format::arduino>(face_, fontArrayName_));
    } else if (format_ == f2b::format::c::identifier) {
        output = QString::fromStdString(generator_.generate<f2b::format::c>(face_, fontArrayName_));
    } else if (format_ == f2b::format::python_list::identifier) {
        output = QString::fromStdString(generator_.generate<f2b::format::python_list>(face_, fontArrayName_));
    } else if (format_ == f2b::format::python_bytes::identifier) {
        output = QString::fromStdString(generator_.generate<f2b::format::python_bytes>(face_, fontArrayName_));
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
