#include "replacedialog.h"

ReplaceDialog::ReplaceDialog(QWidget *parent,QPlainTextEdit *TextEditer):FindDialog(parent,TextEditer)
{
    initWindow();
    connectSlot();
}

void ReplaceDialog::initWindow(){
    m_Replace.setText(tr("替换为:"));
    m_ReplaceBtn.setText(tr("替换(&R)"));
    m_ReplaceAllBtn.setText(tr("全部替换(&A)"));

    m_glayout.removeWidget(&m_matchbox);
    m_glayout.removeWidget(&m_RadioBox);
    m_glayout.removeWidget(&m_cancelbtn);

    m_glayout.addWidget(&m_Replace,1,0);
    m_glayout.addWidget(&m_ReplaceEdit,1,1);
    m_glayout.addWidget(&m_ReplaceAllBtn,1,2);

    m_glayout.addWidget(&m_matchbox,2,0);
    m_glayout.addWidget(&m_RadioBox,2,1);
    m_glayout.addWidget(&m_ReplaceAllBtn,2,2);

    m_glayout.addWidget(&m_cancelbtn,3,2);

    setFixedSize(450,170);
    setWindowTitle(tr("替换"));
}

void ReplaceDialog::onReplaceClicked(){
    QString src = m_findedit.text();
    QString target = m_ReplaceEdit.text();

    if((m_textediter!=nullptr) && (src!="") && (target!="")){
        QString selectedtext = m_textediter->textCursor().selectedText();

        if(selectedtext == src){
            m_textediter->insertPlainText(target);
        }

        onFindClicked();
    }
}

void ReplaceDialog::onReplaceAllClicked(){
    QString src = m_findedit.text();
    QString target = m_ReplaceEdit.text();

    if((m_textediter!=nullptr) && (src!="") && (target!="")){
        QString text = m_textediter->toPlainText();

        text.replace(src,target,m_matchbox.isChecked()? Qt::CaseSensitive : Qt::CaseInsensitive);

        m_textediter->clear();

        m_textediter->insertPlainText(text);
    }
}

void ReplaceDialog::connectSlot(){
    connect(&m_ReplaceBtn,SIGNAL(clicked()),this,SLOT(onReplaceClicked()));
    connect(&m_ReplaceAllBtn,SIGNAL(clicked()),this,SLOT(onReplaceAllClicked()));
}
