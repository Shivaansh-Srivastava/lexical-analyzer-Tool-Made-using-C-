#ifndef MYTEXTSTREAM_H
#define MYTEXTSTREAM_H

#include <QString>
#include <QStringBuilder>
#include <QTextStream>

class MyTextStream : public QTextStream
{
public:
    MyTextStream(QString* str) : QTextStream(str){};
    MyTextStream& operator>>(QString & str);
};

#endif // MYTEXTSTREAM_H
