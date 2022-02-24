#ifndef DELETEDIALOG_H
#define DELETEDIALOG_H

#include <QDialog>

namespace Ui {
class DeleteDialog;
}

class DeleteDialog : public QDialog
{
    Q_OBJECT

public:
    QStringList table_names;
    QStringList tables;

public:
    explicit DeleteDialog(QStringList tables, QWidget *parent = nullptr);
    ~DeleteDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::DeleteDialog *ui;
};

#endif // DELETEDIALOG_H
