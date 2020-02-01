#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "f2b.h"
#include "fontfaceimporter.h"

#include <iostream>
#include <QGraphicsGridLayout>
#include <QGraphicsWidget>
#include <QDebug>
#include <QStyle>
#include <QFontDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui {new Ui::MainWindow}
{
    ui->setupUi(this);

    QFont font("Monaco", 24, QFont::Bold);
    font.setStyleStrategy(QFont::StyleStrategy::NoAntialias);

    auto importer = FontFaceImporter();
    auto face = importer.import_face(font);

    ui->glyphGraphicsView->displayGlyph(face['}']);

    connect(ui->actionNew, &QAction::triggered, [=] () {
        qDebug() << "new";
    });

    connect(ui->actionImport_Font, &QAction::triggered, [=] () {
        bool ok;
        QFont f = QFontDialog::getFont(&ok, QFont(), this, tr("Select Font"), QFontDialog::MonospacedFonts);
        qDebug() << "new" << f;
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
