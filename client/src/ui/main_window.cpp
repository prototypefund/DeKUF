#include "main_window.hpp"
#include "ui_main_window.h"

#include <core/sqlite_storage.hpp>
#include <core/survey_response.hpp>

namespace {
void setupTableWidget(QTableWidget& tableWidget)
{
    // TODO: Probably better to set these in the designer.
    tableWidget.setColumnCount(1);
    tableWidget.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget.setEditTriggers(QAbstractItemView::NoEditTriggers);

    // For a first PoC, we're reading data directly from the daemon's SQLite
    // database, but we plan to switch to IPC:
    // https://gitlab.com/privact/dekuf/-/issues/14
    SqliteStorage storage;
    int row = 0;
    for (const auto& response : storage.listSurveyResponses()) {
        tableWidget.insertRow(row);
        // TODO: Show something more user-friendly than the serialized
        //       response.
        QString text(response->toJsonByteArray());
        text.replace("\n", " ");
        auto item = new QTableWidgetItem; // NOLINT
        item->setText(text);
        tableWidget.setItem(row, 0, item);
        row++;
    }
}
}

MainWindow::MainWindow(QWidget* parent)
    : ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupTableWidget(*ui->tableWidget);
}

MainWindow::~MainWindow() { delete ui; }
