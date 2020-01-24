#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "glyphwidget.h"
#include "pixelitem.h"
#include <iostream>
#include <QGraphicsGridLayout>
#include <QGraphicsWidget>
#include <QDebug>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui {new Ui::MainWindow}
{
    ui->setupUi(this);
    setupGraphicsView();
    setupFontLayout(11, 16, 30);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupGraphicsView()
{
    graphicsScene.reset(new QGraphicsScene());
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
