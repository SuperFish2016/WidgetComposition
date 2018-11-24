#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    widget_composition_ = new WidgetComposition;
    setCentralWidget(widget_composition_);
    this->setWindowTitle("IMF Composition");
}

MainWindow::~MainWindow()
{
    delete ui;
}
