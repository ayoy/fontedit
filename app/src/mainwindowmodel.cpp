#include "mainwindowmodel.h"
#include "sourcecoderunnable.h"
#include <f2b.h>

#include <QDebug>
#include <QThreadPool>
#include <QFile>
#include <QDataStream>
#include <QDir>

#include <iostream>
#include <thread>

Q_DECLARE_METATYPE(SourceCodeOptions::BitNumbering);

namespace SettingsKey {
static const QString bitNumbering = "source_code_options/bit_numbering";
static const QString invertBits = "source_code_options/invert_bits";
static const QString includeLineSpacing = "source_code_options/include_line_spacing";
static const QString format = "source_code_options/format";
static const QString documentPath = "source_code_options/document_path";
}

MainWindowModel::MainWindowModel(QObject *parent) :
    QObject(parent)
{
    sourceCodeOptions_.bit_numbering =
            qvariant_cast<SourceCodeOptions::BitNumbering>(
                settings_.value(SettingsKey::bitNumbering, SourceCodeOptions::LSB)
                );
    sourceCodeOptions_.invert_bits = settings_.value(SettingsKey::invertBits, false).toBool();

    formats_.insert(QString::fromStdString(std::string(Format::C::identifier)), "C/C++");
    formats_.insert(QString::fromStdString(std::string(Format::Arduino::identifier)), "Arduino");
    formats_.insert(QString::fromStdString(std::string(Format::PythonList::identifier)), "Python List");
    formats_.insert(QString::fromStdString(std::string(Format::PythonBytes::identifier)), "Python Bytes");

    currentFormat_ = settings_.value(SettingsKey::format, formats_.firstKey()).toString();

    connect(this, &MainWindowModel::runnableFinished,
            this, &MainWindowModel::sourceCodeChanged,
            Qt::BlockingQueuedConnection);

    qDebug() << "output format:" << currentFormat_;
    registerInputEvent(UserIdle);
}

void MainWindowModel::restoreSession()
{
    auto path = settings_.value(SettingsKey::documentPath).toString();
    if (!path.isNull()) {
        openDocument(path, true);
    } else {
        updateDocumentTitle();
    }
}

void MainWindowModel::registerInputEvent(InputEvent e)
{
    auto state { uiState_ };
    if (std::holds_alternative<InterfaceAction>(e)) {
        auto action = std::get<InterfaceAction>(e);
        switch (action) {
        case ActionImportFont:
        case ActionOpen:
        case ActionAddGlyph:
        case ActionSave:
            break;
        case ActionCopy:
        case ActionPaste:
        case ActionUndo:
        case ActionRedo:
//        case ActionResetGlyph:
//        case ActionResetFont:
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
            state.set(ActionOpen);
            break;
        case UserLoadedFace:
            state.reset();
            state.set(ActionImportFont);
            state.set(ActionOpen);
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
//            state.set(ActionResetGlyph);
//            state.set(ActionResetFont);
            break;
        }
    }

    if (state != uiState_) {
        uiState_ = state;
        emit uiStateChanged(uiState_);
    }
}

void MainWindowModel::updateDocumentTitle()
{
    QString name;
    if (documentPath_.has_value()) {
        QFileInfo fileInfo {documentPath_.value()};
        name = fileInfo.baseName();
    } else {
        name = tr("New Font");
    }

    if (auto faceModel = fontFaceViewModel_.get()) {
        if (faceModel->isModifiedSinceSave()) {
            name += " - ";
            name += tr("Edited");
        }
    }
    if (name != documentTitle_) {
        documentTitle_ = name;
        emit documentTitleChanged(documentTitle_);
    }
}

void MainWindowModel::importFont(const QFont &font)
{
    fontFaceViewModel_ = std::make_unique<FontFaceViewModel>(font);
    registerInputEvent(UserLoadedFace);
    updateDocumentPath({});
    emit faceLoaded(fontFaceViewModel_->face());
    updateDocumentTitle();
}

void MainWindowModel::openDocument(const QString &fileName)
{
    openDocument(fileName, false);
}

void MainWindowModel::openDocument(const QString &fileName, bool failSilently)
{
    try {
        fontFaceViewModel_ = std::make_unique<FontFaceViewModel>(fileName);

        qDebug() << "face loaded from" << fileName;

        registerInputEvent(UserLoadedFace);
        updateDocumentPath(fileName);
        updateDocumentTitle();
        emit faceLoaded(fontFaceViewModel_->face());

    } catch (std::runtime_error& e) {
        updateDocumentPath({});
        updateDocumentTitle();

        if (failSilently) {
            qCritical() << e.what();
        } else {
            emit documentError(QString::fromStdString(e.what()));
        }
    }
}

void MainWindowModel::saveDocument(const QString& fileName)
{
    try {
        fontFaceViewModel_->saveToFile(fileName);

        qDebug() << "face saved to" << fileName;

        updateDocumentPath(fileName);
        updateDocumentTitle();
    } catch (std::runtime_error& e) {
        emit documentError(QString::fromStdString(e.what()));
    }
}

void MainWindowModel::closeCurrentDocument()
{
    fontFaceViewModel_.release();
    updateDocumentPath({});
    updateDocumentTitle();
    registerInputEvent(UserIdle);
    emit documentClosed();
}

void MainWindowModel::setActiveGlyphIndex(std::size_t index)
{
    if (fontFaceViewModel_->activeGlyphIndex().has_value() and
            fontFaceViewModel_->activeGlyphIndex().value() == index)
    {
        return;
    }

    try {
        fontFaceViewModel_->setActiveGlyphIndex(index);
        registerInputEvent(UserLoadedGlyph);

        emit activeGlyphChanged(fontFaceViewModel_->activeGlyph());
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

void MainWindowModel::setIncludeLineSpacing(bool enabled)
{
    sourceCodeOptions_.include_line_spacing = enabled;
    settings_.setValue(SettingsKey::invertBits, enabled);
    reloadSourceCode();
}

void MainWindowModel::setOutputFormat(const QString& format)
{
    currentFormat_ = formats_.key(format, formats_.first());
    settings_.setValue(SettingsKey::format, currentFormat_);
    reloadSourceCode();
}

void MainWindowModel::updateDocumentPath(const std::optional<QString>& path)
{
    documentPath_ = path;
    if (path.has_value()) {
        settings_.setValue(SettingsKey::documentPath, path.value());
    } else {
        settings_.remove(SettingsKey::documentPath);
    }
}

void MainWindowModel::reloadSourceCode()
{
    /// WIP :)

    emit sourceCodeUpdating();

    auto r = new SourceCodeRunnable { faceModel()->face(), sourceCodeOptions_, currentFormat_ };
    r->setCompletionHandler([&](const QString& output) {
        emit runnableFinished(output);
    });
    r->setAutoDelete(true);

    QThreadPool::globalInstance()->start(r);
}
