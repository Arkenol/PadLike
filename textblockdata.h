#ifndef TEXTBLOCKDATA_H
#define TEXTBLOCKDATA_H

#include <QTextBlockUserData>

struct ParenthesisInfo{
    char character;
    int position;
};


class TextBlockData : public QTextBlockUserData{

public:
    TextBlockData();

    QVector<ParenthesisInfo*>parentheses();
    void insert(ParenthesisInfo *info);

private:
    QVector<ParenthesisInfo*> m_parentheses;
};

#endif // TEXTBLOCKDATA_H
