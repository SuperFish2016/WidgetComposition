#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "widget/widgetcomposition.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    WidgetComposition* widget_composition_;
};

#endif // MAINWINDOW_H
