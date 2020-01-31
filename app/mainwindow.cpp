#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "glyphwidget.h"
#include "pixelwidget.h"
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
    ui->glyphGraphicsView->setupFontLayout(11, 16, 30);

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
