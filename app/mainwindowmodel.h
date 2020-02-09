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
    enum InterfaceAction {
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
        ActionTabCode,
        ActionCount,
        ActionFirst = ActionImportFont
    };

    enum UserAction {
        UserIdle = 0,
        UserLoadedFace,
        UserLoadedGlyph,
        UserEditedGlyph
    };

    using UIState = std::bitset<ActionCount>;
    using InputEvent = std::variant<InterfaceAction,UserAction>;

    explicit MainWindowModel(QObject *parent = nullptr);

    void importFont(const QFont& font);

    FontFaceViewModel* faceModel() const {
        return fontFaceViewModel_.get();
    }

    const UIState& uiState() const { return uiState_; }

    void registerInputEvent(InputEvent e);

signals:
    void uiStateChanged(UIState state);
    void faceLoaded(const Font::Face &face);

private:
    UIState uiState_ { 1<<ActionImportFont };
    std::unique_ptr<FontFaceViewModel> fontFaceViewModel_ {};
};

#endif // MAINWINDOWMODEL_H
