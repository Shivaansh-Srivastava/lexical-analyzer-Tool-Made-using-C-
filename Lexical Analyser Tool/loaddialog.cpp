#include "loaddialog.h"
#include "ui_loaddialog.h"

#include <QStandardItem>
#include <QStandardItemModel>
#include <QModelIndexList>
#include <QDebug>

LoadDialog::LoadDialog(QStringList tables, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadDialog)
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

LoadDialog::~LoadDialog()
{
    delete ui;
}

void LoadDialog::on_pushButton_clicked()
{
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();
    table_name = tables.at(selection.at(0).row());

    close();
}

