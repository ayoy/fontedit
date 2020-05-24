#ifndef COMMAND_H
#define COMMAND_H

#include <QUndoCommand>
#include "facewidget.h"
#include "mainwindowmodel.h"

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

    int id() const override { return -1; }

protected:
    std::function<void()> undo_;
    std::function<void()> redo_;
};


class SwitchActiveGlyphCommand : public QUndoCommand
{
public:
    SwitchActiveGlyphCommand(FaceWidget* faceWidget,
                             MainWindowModel* viewModel,
                             std::size_t fromIndex,
                             std::size_t toIndex,
                             QUndoCommand *parent = nullptr) :
        QUndoCommand(QObject::tr("Switch Active Glyph"), parent),
        faceWidget_ { faceWidget },
        viewModel_ { viewModel },
        fromIndex_ { fromIndex },
        toIndex_ { toIndex }
    {}

    void undo() override {
        faceWidget_->setCurrentGlyphIndex(fromIndex_);
        viewModel_->setActiveGlyphIndex(fromIndex_);
    }

    void redo() override {
        faceWidget_->setCurrentGlyphIndex(toIndex_);
        viewModel_->setActiveGlyphIndex(toIndex_);
    }

    bool isObsolete() const {
        return fromIndex_ == toIndex_;
    }

    int id() const override { return 0xa5b939e9; }

    bool mergeWith(const QUndoCommand *other) override {
        if (other->id() != id())
            return false;

        auto otherCommand = static_cast<const SwitchActiveGlyphCommand *>(other);

        toIndex_ = otherCommand->toIndex_;
        return true;
    }

    void setToIndex(std::size_t index) {
        toIndex_ = index;
    }

private:
    FaceWidget* faceWidget_;
    MainWindowModel* viewModel_;
    std::size_t fromIndex_;
    std::size_t toIndex_;
};

#endif // COMMAND_H
