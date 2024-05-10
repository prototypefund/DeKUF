#include "main_window.hpp"
#include "ui_main_window.h"

#include <core/sqlite_storage.hpp>
#include <core/survey_response.hpp>

namespace {
void setupTableWidget(QTableWidget& tableWidget)
{
    tableWidget.horizontalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);
    tableWidget.verticalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);

    SqliteStorage storage;
    int row = 0;
    for (const auto& responseEntry : storage.listSurveyResponses()) {
        tableWidget.insertRow(row);

        auto createdAtItem = new QTableWidgetItem; // NOLINT
        createdAtItem->setText(responseEntry.createdAt.toString());
        tableWidget.setItem(row, 0, createdAtItem);

        auto& response = responseEntry.response;
        QStringList commissionerNames;
        for (const auto& commissioner : response->commissioners)
            commissionerNames.push_back(commissioner->name);
        auto comissionersItem = new QTableWidgetItem; // NOLINT
        comissionersItem->setText(commissionerNames.join(", "));
        tableWidget.setItem(row, 1, comissionersItem);

        auto dataTable = new QTableWidget; // NOLINT
        dataTable->setColumnCount(2);
        dataTable->setHorizontalHeaderLabels({ "Key", "Data" });
        dataTable->horizontalHeader()->setSectionResizeMode(
            QHeaderView::ResizeToContents);
        dataTable->horizontalHeader()->setStretchLastSection(true);
        dataTable->verticalHeader()->setVisible(false);
        dataTable->verticalHeader()->setSectionResizeMode(
            QHeaderView::ResizeToContents);
        dataTable->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        int dataRow = 0;
        for (const auto& queryResponse : response->queryResponses) {
            dataTable->insertRow(dataRow);
            auto dataKeyItem = new QTableWidgetItem; // NOLINT
            dataKeyItem->setText(queryResponse->dataKey);
            dataTable->setItem(dataRow, 0, dataKeyItem);
            auto dataItem = new QTableWidgetItem; // NOLINT
            dataItem->setText(queryResponse->data);
            dataTable->setItem(dataRow, 1, dataItem);
            dataRow++;
        }
        tableWidget.setCellWidget(row, 2, dataTable);

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
