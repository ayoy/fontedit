#ifndef MAINWINDOWMODEL_H
#define MAINWINDOWMODEL_H

#include "fontfaceviewmodel.h"
#include <f2b.h>
#include <memory>
#include <functional>
#include <optional>
#include <bitset>
#include <variant>

class MainWindowModel: public QObject
{
    Q_OBJECT

public:
    enum ButtonAction {
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

    enum UserAction {
        UserIdle = 0,
        UserLoadedFace,
        UserLoadedGlyph,
        UserEditedGlyph
    };

    using ActionsState = std::bitset<ActionCount>;
    using InputEvent = std::variant<ButtonAction,UserAction>;

    explicit MainWindowModel(QObject *parent = nullptr);

    void loadFont(const QFont& font);

    FontFaceViewModel* faceModel() const {
        return fontFaceViewModel_.get();
    }

    const ActionsState& menuActions() const { return actionsState_; }

    void registerInputEvent(InputEvent e);

signals:
    void actionsChanged(ActionsState state);

private:

    ActionsState actionsState_ { 1<<ActionImportFont };
    std::unique_ptr<FontFaceViewModel> fontFaceViewModel_ {};
};


#endif // MAINWINDOWMODEL_H
