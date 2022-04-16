#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "codeeditor.h"
#include "parser.h"
#include "savedialogue.h"
#include "loaddialog.h"
#include "deletedialog.h"

#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <iostream>
#include <QString>
#include <sstream>
#include <memory>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/images/images/qt_main_window_icon.png"));

    const QStringList location = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    QString db_location = location.at(0);

    if(!QDir(db_location).exists()){
        QDir().mkdir(db_location);
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(db_location + "/analysis_report.db");
    database = db;

//    if(!database.open()){
//        qDebug() << "problem";
//    } else {
//        qDebug() << database;
//    }




    ui->language_selector->addItem("C++");
    ui->language_selector->addItem("C");
    ui->language_selector->addItem("JavaScript");
    ui->language_selector->addItem("Python");

    ui->statusbar->addPermanentWidget(ui->progressBar);

    ui->line->hide();
    ui->pushButton_3->hide();
    ui->progressBar->hide();

    CodeEditor* code_editor = new CodeEditor();
    code_editor->setObjectName("code_editor");
    QFont font;
    font.setFamily("Courier");
    code_editor->setFont(font);
    ui->stackedWidget->insertWidget(0, code_editor);
    ui->stackedWidget->setCurrentIndex(0);

    ui->horizontalFrame_2->hide();




}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_pushButton_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_pushButton_4_clicked()
{
    QString filter = "";
    QString selected_language = ui->language_selector->currentText();
    if(selected_language == "C"){
        filter = "C file (*.c)";
    } else if(selected_language == "C++"){
        filter = "C++ file (*.cpp)";
    } else if(selected_language == "JavaScript"){
        filter = "JavaScript file (*.js)";
    } else if(selected_language == "Python"){
        filter = "Python file (.py)";
    }


    QString file_name = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath(), filter);
    QFile file(file_name);

    if(!file.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << file_name;
    } else {
        QTextStream in(&file);
        QString text = in.readAll();
        ui->stackedWidget->setCurrentIndex(0);
        CodeEditor* editor = qobject_cast<CodeEditor*>(ui->stackedWidget->currentWidget());
        editor->setPlainText(text);
    }

    file.close();
}


void MainWindow::on_pushButton_clicked()
{
    ui->line->show();
    ui->pushButton_3->show();
    ui->progressBar->show();
    ui->pushButton->setEnabled(false);
    ui->stackedWidget->setCurrentIndex(1);

    CodeEditor* editor = qobject_cast<CodeEditor*>(ui->stackedWidget->widget(0));
    QString input = editor->toPlainText();
    ui->horizontalFrame_2->hide();

    parse_code(input);
}


void MainWindow::on_actionSave_table_triggered()
{
    std::unique_ptr<SaveDialogue> sd(new SaveDialogue());
    sd->setModal(true);
    sd->exec();

    QString table_name = sd->table_name;
    if(table_name.length() != 0){
        if(!database.open()){
            qDebug() << "problem opening database";
        } else {
            database.open();
            QSqlQuery qry;
            qry.prepare("CREATE TABLE "+table_name+"(Lexeme varchar(50), Tokens varchar(50));");

            if(qry.exec()){
                qDebug() << database.tables();

                int n = ui->tableWidget->rowCount();
                for(int i{}; i<n; ++i){
                    QString lexeme = ui->tableWidget->item(i, 0)->text();
                    QString token = ui->tableWidget->item(i, 1)->text();

                    QSqlQuery qry2;
                    qry2.prepare("INSERT INTO "+table_name+" values ('"+lexeme+"', '"+token+"');");
                    qry2.exec();
                }

            } else {
                qDebug() << "error in creating table";
            }

            database.close();
        }
    }

}

void MainWindow::on_actionLoad_table_triggered()
{
    database.open();
    std::unique_ptr<LoadDialog> ld(new LoadDialog(database.tables()));
    ld->setModal(true);
    ld->exec();

    QString table_name = ld->table_name;
    if(table_name.length() != 0){
        ui->stackedWidget->setCurrentIndex(2);

        QSqlQueryModel *model = new QSqlQueryModel();
        QSqlQuery qry;
        qry.prepare("SELECT * FROM "+table_name+";");
        qry.exec();

        model->setQuery(qry);
        ui->tableView->setModel(model);
    }

    database.close();
}

void MainWindow::on_actionDelete_table_triggered()
{
    database.open();
    std::unique_ptr<DeleteDialog> dd(new DeleteDialog(database.tables()));
    dd->setModal(true);
    dd->exec();

    QStringList table_names = dd->table_names;
    if(table_names.size() != 0){
        for(auto &i: table_names){
            QSqlQuery qry;
            qry.prepare("DROP TABLE "+i+";");

            qry.exec();
        }
    }

    database.close();
}


void MainWindow::parse_code(QString input)
{
    ui->progressBar->setValue(0);
    QString buffer;
    QString searchexp="[a-zA-Z]+[a-zA-Z0-9]*\\.[a-zA-Z0-9._]+";//Regular expression for class objects
    QRegularExpression exp1(searchexp);
    QString searchexp2="[a-zA-Z]+[a-zA-z0-9]*\\.[a-zA-Z0-9._]+\\[()]";//Regular Expression for class objects
    QRegularExpression exp2(searchexp2);
    QString searchexp3="[a-zA-Z]+\\.[h]";
    QRegularExpression exp3(searchexp3);
    QRegularExpression exp4("[0-9]+");

    int x = ui->language_selector->currentIndex();
    std::unique_ptr<Parser> p(new Parser(x+1));

    QStringList list = input.split("\n");

    for(auto &i: list){
        int pos = p->IsMultiComment(i);
        auto itr=i.toStdString().find("*/");
        if(itr!=std::string::npos)
        {
            p->flagger=0;
            i = i.mid(itr+2, i.length()-1);
            p->counter2++;
        }
        if(p->IsComment(i)==false && (p->flagger==0||(pos!=-1 && p->flagger==1)))//To check if the read line is comment or not
        {
            if(pos!=-1 && p->flagger==1)
            {
                i = i.mid(0,pos-1);
            }

            if(i.length()==0)
                continue;
            p->line.push_back(i);
            buffer = p->Splitter(i);

            std::stringstream ss(buffer.toStdString());
            std::string a;
//            qDebug() << buffer;
            while(ss >> a) //storing the individual words in tokens set
            {
                QString word = QString::fromStdString(a);
                p->token_count.push_back(word);
                p->tokens.insert(word);
                if(word.compare("(")==0 || word.compare("{")==0 || word.compare("[")==0)
                {
                    p->opening.push_back(word);
                }
                if(word.compare(")")==0 || word.compare("}")==0 || word.compare("]")==0)
                {
                    p->closing.push_back(word);
                }
            }
        }
    }


    ui->tableWidget->setRowCount(1);
    int i{};
    for(auto it=p->tokens.begin();it!=p->tokens.end();it++)
        {
            QString s=(*it);
            QRegularExpressionMatch match1 = exp1.match(s);
            QRegularExpressionMatch match2 = exp2.match(s);
            QRegularExpressionMatch match3 = exp3.match(s);
            QRegularExpressionMatch match4 = exp4.match(s);


            ui->tableWidget->insertRow(ui->tableWidget->rowCount());
            ui->tableWidget->setItem(i, 0, new QTableWidgetItem(s));
//            qDebug() << s;

            if(match3.hasMatch())
            {
                p->fheads.push_back(s);
                //cout<<s<<": Header File"<<endl;
                ui->tableWidget->setItem(i, 1, new QTableWidgetItem("Header File"));
                p->ids++;
            }
            else if(p->Isoperator(s))
            {
                p->fops.push_back(s);
                //cout<<s<<": Operator"<<endl;
                ui->tableWidget->setItem(i, 1, new QTableWidgetItem("Operator"));
                p->ops++;
            }
            else if(p->Isbrackets(s))
            {
                p->fbracks.push_back(s);
                //cout<<s<<": Bracket"<<endl;
                ui->tableWidget->setItem(i, 1, new QTableWidgetItem("Bracket"));
                p->bracks++;
            }
            else if(p->Isdelim(s))
            {
                p->fdelims.push_back(s);
                //cout<<s<<": Delimiter"<<endl;
                ui->tableWidget->setItem(i, 1, new QTableWidgetItem("Delimiter"));
                p->delims++;
            }
            else if(p->Iskeyword(s))
            {
                p->fkeys.push_back(s);
                //cout<<s<<": Keyword"<<endl;
                ui->tableWidget->setItem(i, 1, new QTableWidgetItem("Keyword"));
                p->keys++;
            }
            else if(match1.hasMatch())//For class objects
            {
                p->fvids.push_back(s);
                //cout<<s<<": Class Object type Identifier"<<endl;
                ui->tableWidget->setItem(i, 1, new QTableWidgetItem("Class Object type Identifier"));
                p->ids++;
            }
            else if(match2.hasMatch())//For class objects-2
            {
                p->fvids.push_back(s);
                //cout<<s<<": Class Object type Identifier"<<endl;
                ui->tableWidget->setItem(i, 1, new QTableWidgetItem("Class Object type Identifier"));
                p->ids++;
            }
            else
            {
                if(p->valid_id(s))
                {
                    p->fvids.push_back(s);
//                        cout<<s<<": Valid Identifier"<<endl;
                    ui->tableWidget->setItem(i, 1, new QTableWidgetItem("Valid Identifier"));
                    p->ids++;
                }
                else if(s[0]=='"')
                {
                    QString sub = s.mid(1,s.length()-1);
                    p->fsids.push_back(sub);
                    //cout<<sub<<": String Identifier"<<endl;
                    ui->tableWidget->setItem(i, 1, new QTableWidgetItem("String Identifier"));
                    p->ids++;
                }
                else if(s[s.length()-1]=='"')
                {
                    QString sub = s.mid(0,s.length()-1);
                    p->fsids.push_back(sub);
                    //cout<<sub<<": String Identifier"<<endl;
                    ui->tableWidget->setItem(i, 1, new QTableWidgetItem("String Identifier"));
                    p->ids++;
                }
                else if(match4.hasMatch())
                {
                    p->fnumids.push_back(s);
                    //cout<<s<<": Numeric Identifier"<<endl;
                    ui->tableWidget->setItem(i, 1, new QTableWidgetItem("Numeric Identifier"));
                    p->ids++;
                }
                else
                {
                    p->finids.push_back(s);
                    //cout<<s<<": Invalid Identifier"<<endl;
                    ui->tableWidget->setItem(i, 1, new QTableWidgetItem("Invalid Identifier"));
                    p->ids++;
                }
            }

//            qDebug() << i;
            i++;
//            ui->tableWidget->item(i,0)->setTextAlignment(Qt::AlignCenter);
//            ui->tableWidget->item(i,1)->setTextAlignment(Qt::AlignCenter);
        }
    ui->tableWidget->removeRow(i);

//    keys,bracks,ops,delims,ids
    ui->tableWidget_2->setItem(0, 1, new QTableWidgetItem(QString::number(p->keys)));
    ui->tableWidget_2->setItem(1, 1, new QTableWidgetItem(QString::number(p->ops)));
    ui->tableWidget_2->setItem(2, 1, new QTableWidgetItem(QString::number(p->delims)));
    ui->tableWidget_2->setItem(3, 1, new QTableWidgetItem(QString::number(p->bracks)));
    ui->tableWidget_2->setItem(4, 1, new QTableWidgetItem(QString::number(p->ids)));

//    Error Handling
    if(p->Isequal(p->opening, p->closing)){
        p->scolanCheck(p->line);
        p->FindError(p->line);
        p->commentBalance();

        if(p->errors.size()!=0){
            ui->horizontalFrame_2->show();
            ui->label_2->setText("errors found; displayed result may not be accurate");
        }
    } else {
        ui->horizontalFrame_2->show();
        ui->label_2->setText("balancing brackets issue");
    }



    ui->progressBar->setValue(100);
    ui->pushButton->setEnabled(true);

}

