#include "mytextstream.h"
#include <QString>
#include <QStringBuilder>
#include <QTextStream>

MyTextStream &MyTextStream::operator>>(QString &str)
{
    QChar ch;
    while (!atEnd()) {
    QTextStream::operator >>(ch);
    if (ch == ' ') {
            break;
        }
    str = str % ch;
    }
    return *this;
}
