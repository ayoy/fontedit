#include "mainwindowmodel.h"

#include <iostream>

MainWindowModel::MainWindowModel(QObject *parent) :
    QObject(parent)
{
    registerInputEvent(UserIdle);
}

void MainWindowModel::registerInputEvent(InputEvent e)
{
    auto state { uiState_ };
    if (std::holds_alternative<InterfaceAction>(e)) {
        auto action = std::get<InterfaceAction>(e);
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
        case ActionTabCode:
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
            state.set(ActionTabCode);
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

    if (state != uiState_) {
        uiState_ = state;
        emit uiStateChanged(uiState_);
    }
}

void MainWindowModel::importFont(const QFont &font)
{
    fontFaceViewModel_ = std::make_unique<FontFaceViewModel>(font);
    registerInputEvent(UserLoadedFace);
    emit faceLoaded(fontFaceViewModel_->face());
}

void MainWindowModel::setActiveGlyphIndex(std::size_t index)
{
    try {
        fontFaceViewModel_->set_active_glyph_index(index);
        registerInputEvent(UserLoadedGlyph);
        emit activeGlyphChanged(fontFaceViewModel_->active_glyph());
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
}

void MainWindowModel::prepareSourceCodeTab()
{
    auto selectedFormatIdentifier = "C/C++";
    if (selectedFormatIdentifier == Format::C::identifier) {
        FontSourceCodeGenerator<Format::C> generator(1, 1, 1, Format::C());
        std::cout << generator.generate(faceModel()->face()) << std::endl;
    }
}
