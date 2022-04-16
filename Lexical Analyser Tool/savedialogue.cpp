#include "savedialogue.h"
#include "ui_savedialogue.h"

SaveDialogue::SaveDialogue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveDialogue)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setWindowIcon(QIcon(":/images/images/qt_main_window_icon.png"));
}

SaveDialogue::~SaveDialogue()
{
    delete ui;
}

void SaveDialogue::on_pushButton_clicked()
{
    this->table_name = ui->lineEdit->text();
    close();
}

