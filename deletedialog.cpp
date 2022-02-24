#include "deletedialog.h"
#include "ui_deletedialog.h"

#include <QStandardItem>
#include <QStandardItemModel>
#include <QModelIndexList>
#include <QDebug>

DeleteDialog::DeleteDialog(QStringList tables, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setWindowIcon(QIcon(":/images/images/qt_main_window_icon.png"));

    this->tables = tables;

    QStandardItemModel *model = new QStandardItemModel();
    QStringList horizontalHeader;

    horizontalHeader.append("Tables");

    model->setHorizontalHeaderLabels(horizontalHeader);

    ui->tableView->setModel(model);

    for(auto &i: tables){
        QStandardItem *row = new QStandardItem(i);
        model->appendRow(QList<QStandardItem*>() << row);
    }
}

DeleteDialog::~DeleteDialog()
{
    delete ui;
}

void DeleteDialog::on_pushButton_clicked()
{
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();
    for(auto &i: selection){
        table_names.append(tables.at(i.row()));
    }

    close();
}

