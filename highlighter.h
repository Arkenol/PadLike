#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QRegExp>
#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QTextStream>

class HighLighter:public QSyntaxHighlighter
{
    Q_OBJECT

public:
    HighLighter(QTextDocument *parent = nullptr);

    QStringList m_keywordlist;

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighLightingRule{
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighLightingRule>HighLightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordfmt;
    QTextCharFormat classfmt;
    QTextCharFormat singlelineCommentfmt;
    QTextCharFormat multilineCommentfmt;
    QTextCharFormat quotationfmt;
    QTextCharFormat bracketfmt;
    QTextCharFormat functionfmt;
    QTextCharFormat tempfmt1;
    QTextCharFormat tempfmt2;
    QTextCharFormat tempfmt3;

    void ReadSyntaxHighlighter(const QString &path);
    void highlightParenthesis(const QString & text);
    int FindNextAvailableChar(const QString &text,char ch,int pos = 0);

};

#endif // HIGHLIGHTER_H
