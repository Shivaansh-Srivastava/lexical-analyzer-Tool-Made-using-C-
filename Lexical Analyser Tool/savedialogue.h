#ifndef SAVEDIALOGUE_H
#define SAVEDIALOGUE_H

#include <QDialog>

namespace Ui {
class SaveDialogue;
}

class SaveDialogue : public QDialog
{
    Q_OBJECT

public:
    QString table_name;

public:
    explicit SaveDialogue(QWidget *parent = nullptr);
    ~SaveDialogue();

private slots:
    void on_pushButton_clicked();

private:
    Ui::SaveDialogue *ui;
};

#endif // SAVEDIALOGUE_H
