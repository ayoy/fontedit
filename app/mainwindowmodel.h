#ifndef MAINWINDOWMODEL_H
#define MAINWINDOWMODEL_H

#include "fontfaceviewmodel.h"
#include <f2b.h>
#include <memory>
#include <functional>
#include <optional>
#include <bitset>


enum MainWindowAction {
    ActionImportFont = 0,
    ActionAddGlyph,
    ActionSave,
    ActionCopy,
    ActionPaste,
    ActionUndo,
    ActionRedo,
    ActionResetGlyph,
    ActionResetFont,
    ActionPrint,
    ActionExport,
    ActionCount,
    ActionFirst = ActionImportFont
};

class MainWindowModel: public QObject
{
    Q_OBJECT

public:
    using ActionsState = std::bitset<MainWindowAction::ActionCount>;

    enum StateFlag {
        Empty             = 1<<ActionImportFont,
        CanCopy           = 1<<ActionCopy,
        CanPaste          = 1<<ActionPaste,
        CanUndo           = 1<<ActionUndo,
        CanRedo           = 1<<ActionRedo,
        CanReset          = 1<<ActionResetFont | 1<<ActionResetGlyph,
        WithFace          = 1<<ActionImportFont | 1<<ActionAddGlyph | 1<<ActionSave | 1<<ActionPrint | 1<<ActionExport,
        WithGlyph         = WithFace | CanCopy,
        WithEditedGlyph   = WithGlyph | CanReset
    };
    Q_DECLARE_FLAGS(State, StateFlag)

    MainWindowModel(QObject *parent = nullptr);

    void loadFont(const QFont& font);

    FontFaceViewModel* faceModel() const {
        return fontFaceViewModel_.get();
    }

    const ActionsState& menuActions() const { return actionsState_; }
    State state() const { return state_; }
    void setState(State s);

signals:
    void actionsChanged(ActionsState state);

private:
    void setEnabledActions(std::vector<MainWindowAction> actions);

    State state_ { Empty };
    ActionsState actionsState_ { 1<<ActionImportFont };
    std::unique_ptr<FontFaceViewModel> fontFaceViewModel_ {};
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MainWindowModel::State)

#endif // MAINWINDOWMODEL_H
