#include "mainwindowmodel.h"
#include <f2b.h>

#include <QDebug>

#include <iostream>

Q_DECLARE_METATYPE(SourceCodeOptions::BitNumbering);

namespace SettingsKey {
static const QString bitNumbering = "source_code_options/bit_numbering";
static const QString invertBits = "source_code_options/invert_bits";
static const QString format = "source_code_options/format";
}

MainWindowModel::MainWindowModel(QObject *parent) :
    QObject(parent)
{
    sourceCodeOptions_.bit_numbering =
            qvariant_cast<SourceCodeOptions::BitNumbering>(
                settings_.value(SettingsKey::bitNumbering, SourceCodeOptions::LSB)
                );
    sourceCodeOptions_.invert_bits = settings_.value(SettingsKey::invertBits, false).toBool();

    formats_.insert("C/C++", QString::fromStdString(std::string(Format::C::identifier)));
    formats_.insert("Arduino", QString::fromStdString(std::string(Format::Arduino::identifier)));
    formats_.insert("Python List", QString::fromStdString(std::string(Format::PythonList::identifier)));
    formats_.insert("Python Bytes", QString::fromStdString(std::string(Format::PythonBytes::identifier)));

    currentFormat_ = settings_.value(SettingsKey::format, formats_.first()).toString();

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
    reloadSourceCode();
}

void MainWindowModel::setInvertBits(bool enabled)
{
    sourceCodeOptions_.invert_bits = enabled;
    settings_.setValue(SettingsKey::invertBits, enabled);
    reloadSourceCode();
}

void MainWindowModel::setMSBEnabled(bool enabled)
{
    auto bitNumbering = enabled ? SourceCodeOptions::MSB : SourceCodeOptions::LSB;
    sourceCodeOptions_.bit_numbering = bitNumbering;
    settings_.setValue(SettingsKey::bitNumbering, bitNumbering);
    reloadSourceCode();
}

void MainWindowModel::setOutputFormat(const QString &format)
{
    currentFormat_ = formats_.value(format, formats_.first());
    settings_.setValue(SettingsKey::format, currentFormat_);
    reloadSourceCode();
}

void MainWindowModel::reloadSourceCode()
{
    auto selectedFormatIdentifier = "arduino";
    if (selectedFormatIdentifier == Format::Arduino::identifier) {
        FontSourceCodeGenerator<Format::Arduino> generator(1, 1, 1);
        auto output = QString::fromStdString(generator.generate(faceModel()->face()));
        qDebug() << output;
        emit sourceCodeChanged(output);
    }
}
