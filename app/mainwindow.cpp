#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "f2b.h"
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

    Font::Glyph g { Font::Size { 4, 4 } };
    g.pixels[2] = true;
    g.pixels[4] = true;
    ui->glyphGraphicsView->displayGlyph(g);

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
