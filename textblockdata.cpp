#include "textblockdata.h"

TextBlockData::TextBlockData()
{

}

void TextBlockData::insert(ParenthesisInfo *info){
    int i = 0;
    while(i<m_parentheses.size() && info->position > m_parentheses.at(i)->position){
        ++i;
    }
    m_parentheses.insert(i,info);
}

QVector<ParenthesisInfo*> TextBlockData::parentheses(){
    return m_parentheses;
}
