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

        auto commissionerItem = new QTableWidgetItem; // NOLINT
        commissionerItem->setText(responseEntry.survey->commissioner->name);
        tableWidget.setItem(row, 1, commissionerItem);

        auto dataTable = new QTableWidget(&tableWidget); // NOLINT
        dataTable->setColumnCount(2);
        dataTable->setHorizontalHeaderLabels({ "Data point", "Cohorts" });
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
            auto dataPointItem = new QTableWidgetItem; // NOLINT
            // TODO: Look up the data point key by queryId here.
            dataPointItem->setText(queryResponse->queryId);
            dataTable->setItem(dataRow, 0, dataPointItem);

            auto cohortTable = new QTableWidget(dataTable); // NOLINT
            cohortTable->setColumnCount(2);
            cohortTable->setHorizontalHeaderLabels({ "Cohort", "Delta" });
            cohortTable->horizontalHeader()->setSectionResizeMode(
                QHeaderView::Stretch);
            cohortTable->verticalHeader()->setVisible(false);
            cohortTable->setSizeAdjustPolicy(
                QAbstractScrollArea::AdjustToContents);
            int cohortRow = 0;
            for (auto it = queryResponse->cohortData.keyValueBegin();
                 it != queryResponse->cohortData.keyValueEnd(); ++it) {
                cohortTable->insertRow(cohortRow);

                auto cohortItem = new QTableWidgetItem; // NOLINT
                cohortItem->setText(it->first);
                cohortTable->setItem(cohortRow, 0, cohortItem);

                auto deltaItem = new QTableWidgetItem; // NOLINT
                deltaItem->setText(QString::number(it->second));
                cohortTable->setItem(cohortRow, 1, deltaItem);

                cohortRow++;
            }
            dataTable->setCellWidget(dataRow, 1, cohortTable);

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
