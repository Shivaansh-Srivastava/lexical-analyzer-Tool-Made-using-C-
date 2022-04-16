#include "parser.h"

Parser::Parser(int x)
{
    switch(x)
        {
            case 1:     //c++
                keywords={"int","char","float","class","struct","new","delete","static","if","else","for","while","vector","string","double","continue","break","goto","void","return","set","unordered_map","map","sizeof","cout","cin","main","endl","#include","using","namespace","std","public","private","protected"};
                break;
            case 2:     //c
                keywords={"int","char","float","struct","if","else","for","while","double","continue","break","goto","void","return","sizeof","malloc","calloc","printf","scanf","main","#include"};
                break;
            case 3:     //JavaScript
                keywords={"val","let","alert","prompt","console","log","document","element","getElementById","const","Object","Array","function","class","extends"};
                break;
            case 4:     //Python
                keywords={"eval","def","if","else","for","range","input","print","return","while","class"};
        }
        brackets={"(",")","{","}","[","]"};
        operators={"+","-","*","/","%","=","<",">",".","++","--","<<",">>"};
        delim={",",";",":"};
        keys=0;
        bracks=0;
        ops=0;
        delims=0;
        ids=0;
        flagger=0;
        counter1=0;
        counter2=0;
}

bool Parser::Iskeyword(QString s)
{
    auto it=keywords.find(s);
    if(it!=keywords.end())
    {
        //token_count.push_back(s);
        return true;
    }
    return false;
}

//To check for operators
bool Parser::Isoperator(QString s)
{
    auto it=operators.find(s);
    if(it!=operators.end())
    {
        //token_count.push_back(s);
        return true;
    }
    else
    return false;
}

//To check for brackets
bool Parser::Isbrackets(QString s)
{
    auto it=brackets.find(s);
    if(it!=brackets.end())
    {
        //token_count.push_back(s);
        return true;
    }
    else
    return false;
}

//To check for delimiters
bool Parser::Isdelim(QString s)
{
    auto it=delim.find(s);
    if(it!=delim.end())
    {
        //token_count.push_back(s);
        return true;
    }
    else
    {
        return false;
    }
}

//Function to check for valid identifiers
bool Parser::valid_id(QString s)
{
    if(!(s[0]=='_' || (s[0]>='a' && s[0]<='z') || (s[0]>='A' && s[0]<='Z')))
    {
        return false;
    }
    for(int i=1;i<s.length();i++)
    {
        if(!((s[i]>='a' && s[i]<='z') || (s[i]>='A' && s[i]<='Z') || (s[i]>='0' && s[i]<='9') || s[i]=='_'))
        return false;
    }
    return true;
}

//To split the word properly between identifier and any special symbol
QString Parser::Splitter(QString s)
{
//    qDebug() << s;
    int len = s.length();
    std::string comment="//";
    std::string s1 = s.toStdString();
    auto itr=s1.find(comment);
    if(itr!=std::string::npos)
    {
        for(int i=itr;i<len;i++)
            s[i]=' ';
    }
    QString st;
    std::vector<QString> extra;
    for(int i=0;i<s.length();i++)
    {
        if(!((s[i]>='a' && s[i]<='z') || (s[i]>='A' && s[i]<='Z') || (s[i]>='0' && s[i]<='9') || s[i]==' ' || s[i]=='#' || s[i]=='"' || s[i]=='.'))
        {
            if(s[i]=='<' && i<s.length()-1 && s[i+1]=='<')//for <<
            {
                extra.push_back("<<");
                s[i]=' ';
                s[i+1]=' ';
            }
            if(s[i]=='>' && i<s.length()-1 && s[i+1]=='>')//for >>
            {
                extra.push_back(">>");
                s[i]=' ';
                s[i+1]=' ';
            }
            if(s[i]=='+' && i<s.length()-1 && s[i+1]=='+')//for ++
            {
                extra.push_back("++");
                s[i]=' ';
                s[i+1]=' ';
            }
            if(s[i]=='-' && i<s.length()-1 && s[i+1]=='-')//for --
            {
                extra.push_back("--");
                s[i]=' ';
                s[i+1]=' ';
            }
            QString str(1,s[i]);
            extra.push_back(str);
            s[i]=' ';
        }
    }

    st=s;
    for(auto it=extra.begin();it!=extra.end();it++)
    {
        st=st+" "+(*it);
    }
    return st;
}

bool Parser::Isequal(std::vector<QString> a, std::vector<QString> b)
{
    sort(a.begin(),a.end());
    sort(b.begin(),b.end());
    int len1=a.size();
    int len2=b.size();
    if(len1!=len2)
    {
        return false;
    }
    int b1,bb1,b2,bb2,b3,bb3;
    b1=count(a.begin(),a.end(),"(");
    b2=count(a.begin(),a.end(),"{");
    b3=count(a.begin(),a.end(),"[");
    bb1=count(b.begin(),b.end(),")");
    bb2=count(b.begin(),b.end(),"}");
    bb3=count(b.begin(),b.end(),"]");
    if(b1!=bb1)
    {
        //cout<<"Error: () brackets are not balanced"<<endl;
        return false;
    }
    if(b2!=bb2)
    {
        //cout<<"Error: {} brackets are not balanced"<<endl;
        return false;
    }
    if(b3!=bb3)
    {
        return false;
    }
    return true;
}

void Parser::addKeyword(QString s)
{
    keywords.insert(s);
}

bool Parser::IsComment(QString line)
{
    if(line.length() == 0)
        return false;

    int i=0;
    while(line[i]==' ')
        i++;
    if(line[i]=='/' && line[i+1]=='/')
        return true;
    return false;
}

int Parser::IsMultiComment(QString line)
{
    std::string s = line.toStdString();
    auto itr=s.find("/*");
        if(itr!=std::string::npos)
        {
            counter1++;
            flagger=1;
            return itr;
        }
        return -1;
    return 0;
}

void Parser::commentBalance()
{
    if(counter1!=counter2)
        errors.push_back("Warning: You have a /* and */ mismatch or are not equal");
}

void Parser::scolanCheck(std::vector<QString> line)
{
    QRegularExpression exp("[int,char,void,vector,set,string,float,double,long]+ [a-zA-Z0-9_:]+[\\(]+[int,char,vector,string,float,double,long]*[a-zA-Z0-9\\,\\* ]*[\\)]+[\\{]*[\\(]*");
    std::set<std::string> sp={"#include","while","for","struct","class","void","{","}","(",")","if","else","public","private","protected"};
    std::string word;
    QString buffer;
    for(auto it=line.begin();it!=line.end();it++)
    {
        QRegularExpressionMatch match = exp.match(*it);
        if(match.hasMatch())
        {
            continue;
        }
        else
        {
            buffer = Splitter(*it);
            std::stringstream s(buffer.toStdString());
            s >> word;
            auto itr=sp.find(word);
            if(itr==sp.end())
            {
                QString str=(*it);
                int i=str.length()-1;
                while(str[i]==' ')
                    i--;
                if(str[i]!=';')
                {
                    errors.push_back("Warning: Missing Semicolan in line- "+str);
                }

            }
        }

    }
}

//To find keywords related error
void Parser::FindError(std::vector<QString> l)
{
    std::string word;
    QString buff;
    for(auto it=l.begin();it!=l.end();it++)
    {
        std::vector<QString> seq;
        QString tline=(*it);
        QString test("");
        buff=Splitter(tline);
        std::stringstream s(buff.toStdString());
        while(s >> word)
        {
            seq.push_back(QString::fromStdString(word));
        }
        for(auto itr=seq.begin();itr!=seq.end();itr++)
        {
            if(!Iskeyword(*itr))
            {
                test=test+(*itr);
                if(Iskeyword(test))
                {
                    errors.push_back("Warning: Possible error at: "+tline);
                    test.isEmpty();
                }
            }
        }
        QString rev("");
        //reverse(seq.begin(),seq.end());
        for(auto itr=seq.rbegin();itr!=seq.rend();itr++)
        {
            //cout<<"Outer:"<<*itr<<endl;
            if(!Iskeyword(*itr) && !(Isoperator(*itr) || Isdelim(*itr) || Isbrackets(*itr)))
            {
                if(rev.isEmpty() && !(Isoperator(*itr) || Isdelim(*itr) || Isbrackets(*itr)))
                {
                    rev=(*itr);
                }
                else
                {
                    if(!(Isoperator(*itr) || Isdelim(*itr) || Isbrackets(*itr)))
                    rev=(*itr)+rev;
                }
                //cout<<rev<<endl;
                if(Iskeyword(rev))
                {
                    errors.push_back("Warning: Possible error at: "+tline);
                    test.isEmpty();
                }
            }
        }
    }
    //cout<<"Have visited FindErrors"<<endl;
}
