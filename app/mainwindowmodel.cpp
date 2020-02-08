#include "mainwindowmodel.h"

MainWindowModel::MainWindowModel(QObject *parent) :
    QObject(parent)
{

}

void MainWindowModel::loadFont(const QFont &font)
{
    fontFaceViewModel_ = std::make_unique<FontFaceViewModel>(font);
    registerInputEvent(UserLoadedFace);
}

void MainWindowModel::registerInputEvent(InputEvent e)
{
    auto state { actionsState_ };
    if (std::holds_alternative<ButtonAction>(e)) {
        auto action = std::get<ButtonAction>(e);
        switch (action) {
        case ActionImportFont:
        case ActionAddGlyph:
        case ActionSave:
            break;
        case ActionCopy:
        case ActionPaste:
        case ActionUndo:
        case ActionRedo:
        case ActionResetGlyph:
        case ActionResetFont:
        case ActionPrint:
        case ActionExport:
        default:
            break;
        }
    } else if (std::holds_alternative<UserAction>(e)) {
        auto action = std::get<UserAction>(e);
        switch (action) {
        case UserIdle:
            state.reset();
            state.set(ActionImportFont);
            break;
        case UserLoadedFace:
            state.reset();
            state.set(ActionImportFont);
            state.set(ActionAddGlyph);
            state.set(ActionSave);
            state.set(ActionPrint);
            state.set(ActionExport);
            break;
        case UserLoadedGlyph:
            state.set(ActionCopy);
            break;
        case UserEditedGlyph:
            state.set(ActionResetGlyph);
            state.set(ActionResetFont);
            break;
        }
    }

    if (state != actionsState_) {
        actionsState_ = state;
        emit actionsChanged(actionsState_);
    }
}
