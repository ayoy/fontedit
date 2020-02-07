#include "mainwindowmodel.h"

MainWindowModel::MainWindowModel(QObject *parent) :
    QObject(parent)
{

}

void MainWindowModel::setFont(const QFont &font)
{
    fontFaceViewModel_ = std::make_unique<FontFaceViewModel>(font);
}
