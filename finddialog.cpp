#include "finddialog.h"

FindDialog::FindDialog(QWidget* parent , QPlainTextEdit* TextEditer):QDialog(parent,Qt::WindowCloseButtonHint | Qt::Drawer)
{
    initWindow();
    connectSlot();

    setLayout(&m_glayout);
    setFixedSize(450,120);
    setWindowTitle(tr("查找"));

    setPlainTextEdit(TextEditer);
}

void FindDialog::initWindow(){
    m_find.setText(tr("查找目标："));
    m_findbtn.setText(tr("查找下一个(&F)"));
    m_cancelbtn.setText(tr("取消"));
    m_matchbox.setText(tr("区分大小写(&C)"));
    m_RadioBox.setTitle(tr("方向"));
    m_upforward.setText(tr("向上(&U)"));
    m_downforward.setText(tr("向下"));
    m_downforward.setChecked(true);

    m_hlayout.addWidget(&m_upforward);
    m_hlayout.addWidget(&m_downforward);
    m_RadioBox.setLayout(&m_hlayout);

    m_glayout.addWidget(&m_find,0,0);
    m_glayout.addWidget(&m_findedit,0,1);
    m_glayout.addWidget(&m_findbtn,0,2);

    m_glayout.addWidget(&m_matchbox,1,0);
    m_glayout.addWidget(&m_RadioBox,1,1);
    m_glayout.addWidget(&m_cancelbtn,1,2);
}

void FindDialog::connectSlot(){
    connect(&m_findbtn,SIGNAL(clicked()),this,SLOT(onFindClicked()));
    connect(&m_cancelbtn,SIGNAL(clicked()),this,SLOT(onCancelClicked()));
}

void FindDialog::setPlainTextEdit(QPlainTextEdit *TextEditer){
    m_textediter = TextEditer;
}

void FindDialog::onFindClicked(){
    QString src = m_findedit.text();
    if((m_textediter!=nullptr)&&(src!="")){
        QString text = m_textediter->toPlainText();
        QTextCursor c = m_textediter->textCursor();
        int index = -1;
        if(m_downforward.isChecked()){
            index = text.indexOf(src, c.position(), m_matchbox.isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);

            if(index>=0){
                c.setPosition(index);
                c.setPosition(index+src.length(),QTextCursor::KeepAnchor);

                m_textediter->setTextCursor(c);
            }
        }

        if(m_upforward.isChecked()){
            index = text.lastIndexOf(src , c.position() - text.length() -1 , m_matchbox.isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);

            if(index>=0){
                c.setPosition(index + src.length());
                c.setPosition(index,QTextCursor::KeepAnchor);

                m_textediter->setTextCursor(c);
            }
        }

        if(index<0){
                QMessageBox msg(this);

                msg.setWindowTitle("Padlike");
                msg.setText(QString("找不到")+"\""+src+"\"");
                msg.setWindowFlag(Qt::Drawer);
                msg.setIcon(QMessageBox::Information);
                msg.setStandardButtons(QMessageBox::Ok);
                msg.setButtonText(QMessageBox::Ok,tr("确定"));
                msg.exec();
        }
    }
}

void FindDialog::onCancelClicked(){
    close();
}

bool FindDialog::event(QEvent *event){
    if(event->type() == QEvent::Close){
        hide();
        return true;
    }

    return QDialog::event(event);
}

FindDialog::~FindDialog(){

}
