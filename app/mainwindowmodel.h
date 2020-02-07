#ifndef MAINWINDOWMODEL_H
#define MAINWINDOWMODEL_H

#include "fontfaceviewmodel.h"
#include <f2b.h>
#include <memory>
#include <functional>
#include <optional>

class MainWindowModel: QObject
{
public:
    MainWindowModel(QObject *parent = nullptr);

    void setFont(const QFont& font);

    FontFaceViewModel* faceModel() const {
        return fontFaceViewModel_.get();
    }

private:
    std::unique_ptr<FontFaceViewModel> fontFaceViewModel_ {};
};

#endif // MAINWINDOWMODEL_H
