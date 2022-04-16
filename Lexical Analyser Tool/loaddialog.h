#ifndef LOADDIALOG_H
#define LOADDIALOG_H

#include <QDialog>

namespace Ui {
class LoadDialog;
}

class LoadDialog : public QDialog
{
    Q_OBJECT

public:
    QString table_name;
    QStringList tables;

public:
    explicit LoadDialog(QStringList tables, QWidget *parent = nullptr);
    ~LoadDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::LoadDialog *ui;
};

#endif // LOADDIALOG_H
