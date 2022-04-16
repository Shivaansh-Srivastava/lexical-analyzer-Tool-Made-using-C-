#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <iostream>
#include <set>
#include <algorithm>
#include <vector>
#include <string>
#include <unordered_map>
#include <numeric>
#include <iterator>
#include <ctype.h>
#include <sstream>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

class Parser
{
public:
    std::set<QString> keywords;
    std::set<QString> brackets;
    std::set<QString> operators;
    std::set<QString> delim;
    std::set<QString> header_files;
    std::set<QString> tokens;//Unique tokens stored in this
    std::vector<QString> token_count;
    std::vector<QString> opening,closing;//Helps in balancing brackets test
    std::vector<QString> line;//To store all the code lines from the file
    std::vector<QString> errors;//To store all the error messages
    std::vector<QString> fkeys,fvids,fops,fdelims,fnumids,finids,fheads,fbracks,fsids;
    int keys,bracks,ops,delims,ids,flagger,counter1,counter2;

public:
    Parser(int x);
    bool Iskeyword(QString s);
    bool Isbrackets(QString s);
    bool Isoperator(QString s);
    bool Isdelim(QString s);
//    void Register();
    bool valid_id(QString s);
    QString Splitter(QString s);
    bool Isequal(std::vector<QString> a, std::vector<QString> b);
    void FindError(std::vector<QString> l);
//    void printError(std::vector<QString> e);
    void addKeyword(QString s);
//    void printTokens();
    bool IsComment(QString line);
    int IsMultiComment(QString line);
    void commentBalance();
    void scolanCheck(std::vector<QString> line);
};

#endif // PARSER_H
