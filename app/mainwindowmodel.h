#ifndef MAINWINDOWMODEL_H
#define MAINWINDOWMODEL_H

#include "fontfaceviewmodel.h"
#include <f2b.h>
#include <memory>
#include <functional>
#include <optional>
#include <bitset>
#include <variant>

#include <QMap>
#include <QSettings>

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

    FontFaceViewModel* faceModel() const {
        return fontFaceViewModel_.get();
    }

    const UIState& uiState() const { return uiState_; }

    Qt::CheckState invertBits() const {
        return sourceCodeOptions_.invert_bits ? Qt::Checked : Qt::Unchecked;
    }

    Qt::CheckState msbEnabled() const {
        return sourceCodeOptions_.bit_numbering == SourceCodeOptions::MSB
                ? Qt::Checked : Qt::Unchecked;
    }

    const QMap<QString,QString>& outputFormats() const {
        return formats_;
    }

    QString outputFormat() const {
        return formats_.value(currentFormat_, formats_.first());
    }

    void registerInputEvent(InputEvent e);

public slots:
    void importFont(const QFont& font);
    void setActiveGlyphIndex(std::size_t index);
    void prepareSourceCodeTab();
    void setInvertBits(bool enabled);
    void setMSBEnabled(bool enabled);
    void setOutputFormat(const QString &format); // human-readable

signals:
    void uiStateChanged(UIState state);
    void faceLoaded(const Font::Face &face);
    void activeGlyphChanged(const Font::Glyph &glyph);
    void sourceCodeChanged(const QString &sourceCode);

private:
    void reloadSourceCode();

    UIState uiState_ { 1<<ActionImportFont };
    std::unique_ptr<FontFaceViewModel> fontFaceViewModel_ {};
    SourceCodeOptions sourceCodeOptions_;

    QMap<QString, QString> formats_; // identifier <-> human-readable
    QString currentFormat_; // identifier
    QSettings settings_;
};

#endif // MAINWINDOWMODEL_H
