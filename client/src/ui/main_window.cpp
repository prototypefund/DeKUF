#include "main_window.hpp"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget* parent)
    : ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }
