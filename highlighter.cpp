#include "highlighter.h"
#include "textblockdata.h"

#include <QMessageBox>
#include <QDebug>

HighLighter::HighLighter(QTextDocument *parent):QSyntaxHighlighter(parent)
{

    HighLightingRule rule;

    keywordfmt.setForeground(Qt::darkBlue);
    //keywordfmt.setFontWeight(QFont::Bold);
    const QString keywordPatterns[] = {
            QStringLiteral("\\bchar\\b"), QStringLiteral("\\bclass\\b"), QStringLiteral("\\bconst\\b"),
            QStringLiteral("\\bdouble\\b"), QStringLiteral("\\benum\\b"), QStringLiteral("\\bexplicit\\b"),
            QStringLiteral("\\bfriend\\b"), QStringLiteral("\\binline\\b"), QStringLiteral("\\bint\\b"),
            QStringLiteral("\\blong\\b"), QStringLiteral("\\bnamespace\\b"), QStringLiteral("\\boperator\\b"),
            QStringLiteral("\\bprivate\\b"), QStringLiteral("\\bprotected\\b"), QStringLiteral("\\bpublic\\b"),
            QStringLiteral("\\bshort\\b"), QStringLiteral("\\bsignals\\b"), QStringLiteral("\\bsigned\\b"),
            QStringLiteral("\\bslots\\b"), QStringLiteral("\\bstatic\\b"), QStringLiteral("\\bstruct\\b"),
            QStringLiteral("\\btemplate\\b"), QStringLiteral("\\btypedef\\b"), QStringLiteral("\\btypename\\b"),
            QStringLiteral("\\bunion\\b"), QStringLiteral("\\bunsigned\\b"), QStringLiteral("\\bvirtual\\b"),
            QStringLiteral("\\bvoid\\b"), QStringLiteral("\\bvolatile\\b"), QStringLiteral("\\bbool\\b"),
            QStringLiteral("\\btrue\\b"),QStringLiteral("\\bfalse\\b")
        };

    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordfmt;
        HighLightingRules.append(rule);

    }

    ReadSyntaxHighlighter(":/qm/qm/C.txt");
    for(const QString &pattern : m_keywordlist){
        const QString a = QString("\\b%1\\b").arg(pattern);
        rule.pattern = QRegularExpression(a);
        rule.format = keywordfmt;
        HighLightingRules.append(rule);
    }

    classfmt.setFontWeight(QFont::Bold);
    classfmt.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression(QStringLiteral("(#(if|else)\\s*[A-Za-z]+(?=\\s))|(#[a-zA-Z]+\\s)"));            //  宏匹配,QT里似乎/b#冲突，识别不出，故而直接匹配#
    rule.format = classfmt;
    HighLightingRules.append(rule);



    tempfmt1.setForeground(Qt::darkRed);
    rule.pattern = QRegularExpression(QStringLiteral("\\b[a-zA-Z0-9_]+(?=::)"));            //临时，等以后词法分析后替换
    rule.format = tempfmt1;
    HighLightingRules.append(rule);


    quotationfmt.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QStringLiteral("\".*\""));            //匹配""
    rule.format = quotationfmt;
    HighLightingRules.append(rule);


    bracketfmt.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QStringLiteral("\\<.*\\>"));              //匹配<>
    rule.format = bracketfmt;
    HighLightingRules.append(rule);



    //functionfmt.setFontItalic(true);
    //functionfmt.setFontWeight(QFont::Bold);
    functionfmt.setForeground(Qt::darkCyan);
    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));               //匹配(以前的单词
    rule.format = functionfmt;
    HighLightingRules.append(rule);


    singlelineCommentfmt.setForeground(Qt::darkGray);
    rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));                  //匹配单行注释
    rule.format = singlelineCommentfmt;
    HighLightingRules.append(rule);

    multilineCommentfmt.setForeground(Qt::darkGray);

    commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));


}

void HighLighter::highlightBlock(const QString &text){
    highlightParenthesis(text);
    //if(ok == false)return;              //高亮开关
    for(const HighLightingRule &rule : qAsConst(HighLightingRules)){
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while(matchIterator.hasNext()){
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(),match.capturedLength(),rule.format);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;

    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;

        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }

        else {
            commentLength = endIndex - startIndex + match.capturedLength();
        }

        setFormat(startIndex, commentLength, multilineCommentfmt);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);

    }

}



void HighLighter::ReadSyntaxHighlighter(const QString &path){
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::critical(nullptr,QString("读取错误"),QString("文件%1读取错误"),"确定");
        qDebug()<<"Open file " << path << "error" << __FUNCTION__;
    }
    QTextStream ReadSyntaxStream(&file);
    QString keyword;
    QString LineString;
    QStringList LineStrList;
    QStringList KeywordList;
    QRegExp re("\\s+");
    QRegExp sp("[ ]+");
    //int r,g,b;
    while(!ReadSyntaxStream.atEnd()){
        LineString = ReadSyntaxStream.readLine();
        if(LineString.startsWith(QString("$$"))){               //$$表示注释
            continue;
        }
        LineString.replace(re," ");
        LineStrList = LineString.split(sp);
        //qDebug()<<LineStrList<<endl;
        //if(LineStrList.size()!=4)continue;
        keyword = LineStrList.at(0);
        //r = LineStrList.at(1).toInt();
        //g = LineStrList.at(2).toInt();
        //b = LineStrList.at(3).toInt();
        //QMap<QString,QColor> Map_SynHighLighter;                //储存RGB颜色,此处为了统一关键词颜色不使用自定义颜色
        //Map_SynHighLighter.insert(keyword,QColor(r,g,b));
        KeywordList.append(keyword);
        m_keywordlist = KeywordList;
    }
    //qDebug()<<KeywordList;
}

int HighLighter::FindNextAvailableChar(const QString &text,char ch,int pos){        //寻找下一个不在引号里的字符ch，返回其位置
    int length = text.length();
    if(pos>length)
        return -1;

    bool comment_flag_1 = false;            //用于检测是否在单引号里
    bool comment_flag_2 = false;            //用于检测是否在双引号里
    int i = pos;
    while(i<length){
        if(text[i]=='\'' && comment_flag_2 == false){           //单引号不在双引号中
            if(i == 0 || (i != 0 && text[i]!='\\')){
                if(comment_flag_1 == false)
                    comment_flag_1 = true;
                else
                    comment_flag_1 = false;
                ++i;
                continue;
            }
        }

        if(text[i]=='"' && comment_flag_1 == false){            //双引号不在单引号中
            if(i == 0 || (i!=0 && text[i] !='\\')){
                if(comment_flag_2 == false)
                    comment_flag_2 = true;
                else
                    comment_flag_2 = false;
                ++i;
                continue;
            }
        }

        if(comment_flag_1 == true || comment_flag_2 == true){
            ++i;                //在引号里，忽略所有字符
            continue;
        }

        else{               //不在引号li
            if(text[i] == ch){
                break;
            }
            else{
                ++i;
            }
        }
    }

    if(i == length){
        return -1;
    }

    else{
        return i;
    }
}

void HighLighter::highlightParenthesis(const QString & text){
    TextBlockData *data = new TextBlockData;

    //int leftpos = text.indexOf('(');
    int leftpos = FindNextAvailableChar(text,'(');
    while(leftpos!=-1){
        ParenthesisInfo *info = new ParenthesisInfo;
        info->character = '(';
        info->position = leftpos;
        data->insert(info);
        //leftpos = text.indexOf('(',leftpos+1);
        leftpos = FindNextAvailableChar(text,'(',leftpos+1);
    }

    //int rightpos = text.indexOf(')');
    int rightpos = FindNextAvailableChar(text,')');
    while(rightpos!=-1){
        ParenthesisInfo *info = new ParenthesisInfo;
        info->character = ')';
        info->position = rightpos;
        data->insert(info);
        //rightpos = text.indexOf(')',rightpos+1);
        rightpos = FindNextAvailableChar(text,')',rightpos+1);
    }

    //leftpos = text.indexOf('[');
    leftpos = FindNextAvailableChar(text,'[');
    while(leftpos!=-1){
        ParenthesisInfo *info = new ParenthesisInfo;
        info->character = '[';
        info->position = leftpos;
        data->insert(info);
        //leftpos = text.indexOf('[',leftpos+1);
        leftpos = FindNextAvailableChar(text,'[',leftpos+1);
    }

    //rightpos = text.indexOf(']');
    rightpos = FindNextAvailableChar(text,']');
    while(rightpos!=-1){
        ParenthesisInfo *info = new ParenthesisInfo;
        info->character = ']';
        info->position = rightpos;
        data->insert(info);
        //rightpos = text.indexOf(']',rightpos+1);
        rightpos = FindNextAvailableChar(text,']',rightpos+1);
    }

    leftpos = text.indexOf('{');
    leftpos = FindNextAvailableChar(text,'{');
    while(leftpos!=-1){
        ParenthesisInfo *info = new ParenthesisInfo;
        info->character = '{';
        info->position = leftpos;
        data->insert(info);
        //leftpos = text.indexOf('{',leftpos+1);
        leftpos = FindNextAvailableChar(text,'{',leftpos+1);
    }

    //rightpos = text.indexOf('}');
    rightpos = FindNextAvailableChar(text,'}');
    while(rightpos!=-1){
        ParenthesisInfo *info = new ParenthesisInfo;
        info->character = '}';
        info->position = rightpos;
        data->insert(info);
        //rightpos = text.indexOf('}',rightpos+1);
        rightpos = FindNextAvailableChar(text,'}',rightpos+1);
    }

    setCurrentBlockUserData(data);

}
