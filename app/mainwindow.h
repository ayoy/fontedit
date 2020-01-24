#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupFontLayout(uint8_t width, uint8_t height, qreal size);
    void setupGraphicsView();

    Ui::MainWindow *ui;
    std::unique_ptr<QGraphicsScene> graphicsScene;
};
#endif // MAINWINDOW_H
