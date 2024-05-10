#include "main_window.hpp"
#include "ui_main_window.h"

#include <core/sqlite_storage.hpp>
#include <core/survey_response.hpp>

namespace {
void setupTableWidget(QTableWidget& tableWidget)
{
    tableWidget.horizontalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);

    // For a first PoC, we're reading data directly from the daemon's SQLite
    // database, but we plan to switch to IPC:
    // https://gitlab.com/privact/dekuf/-/issues/14
    SqliteStorage storage;
    int row = 0;
    for (const auto& responseEntry : storage.listSurveyResponses()) {
        tableWidget.insertRow(row);

        auto createdAtItem = new QTableWidgetItem; // NOLINT
        createdAtItem->setText(responseEntry.createdAt.toString());
        tableWidget.setItem(row, 0, createdAtItem);

        // TODO: Show something more user-friendly than the serialized
        //       response.
        QString data(responseEntry.response->toJsonByteArray());
        data.replace("\n", " ");
        auto dataItem = new QTableWidgetItem; // NOLINT
        dataItem->setText(data);
        tableWidget.setItem(row, 1, dataItem);

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
