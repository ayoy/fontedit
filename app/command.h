#ifndef COMMAND_H
#define COMMAND_H

#include <QUndoCommand>

class Command : public QUndoCommand
{
public:
    Command(const QString& name,
            std::function<void()> undo,
            std::function<void()> redo,
            QUndoCommand *parent = nullptr) :
        QUndoCommand(name, parent),
        undo_ { undo },
        redo_ { redo }
    {}

    void undo() override { undo_(); }
    void redo() override { redo_(); }

private:
    std::function<void()> undo_;
    std::function<void()> redo_;
};

#endif // COMMAND_H
