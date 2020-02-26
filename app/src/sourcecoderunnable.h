#ifndef SOURCECODERUNNABLE_H
#define SOURCECODERUNNABLE_H

#include <QRunnable>
#include <QString>
#include <f2b.h>
#include <memory>
#include <functional>
#include <optional>

class SourceCodeRunnable : public QRunnable
{
    using CompletionHandler = std::function<void(const QString&)>;

public:
    SourceCodeRunnable(Font::Face face, SourceCodeOptions options, const QString& format)
        : QRunnable(),
          face_ { std::move(face) },
          generator_ { options },
          format_ { format.toStdString() } {};

    void run() override;

    void setCompletionHandler(CompletionHandler handler) {
        handler_ = std::move(handler);
    }

private:
    Font::Face face_;
    FontSourceCodeGenerator generator_;
    std::string format_;
    CompletionHandler handler_ {};
};

#endif // SOURCECODERUNNABLE_H
