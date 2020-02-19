#include "glypheditcommand.h"

GlyphEditCommand::GlyphEditCommand(std::function<void()> undo,
                                   std::function<void()> redo,
                                   QUndoCommand *parent) :
    QUndoCommand(parent),
    undo_ { undo },
    redo_ { redo }
{
}
