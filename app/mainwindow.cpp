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
    setupGraphicsView();
    setupFontLayout(11, 16, 30);

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

void MainWindow::setupGraphicsView()
{
    graphicsScene = std::make_unique<QGraphicsScene>();
    ui->graphicsView->setScene(graphicsScene.get());
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    auto glyphView = graphicsScene->items().constLast();
    ui->graphicsView->fitInView(glyphView, Qt::KeepAspectRatio);
}

void MainWindow::setupFontLayout(uint8_t width, uint8_t height, qreal size)
{
    auto glyphWidget = new GlyphWidget(width, height, size);

    graphicsScene->setBackgroundBrush(QBrush(Qt::lightGray));
    graphicsScene->addItem(glyphWidget);
}
