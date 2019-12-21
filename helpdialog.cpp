#include "helpdialog.h"

HelpDiglog::HelpDiglog(QWidget* parent):QDialog(parent,Qt::Desktop | Qt::Drawer)
{
    initWindow();
    connectSlot();
}

void HelpDiglog::initWindow(){
    QPixmap logos(":/Icons/icons/fileIcon.png");
    logos = logos.scaled(84,88,Qt::KeepAspectRatio);

    m_Logo.setParent(this);
    m_Logo.setPixmap(logos);
    m_Logo.move(14,19);
    m_Logo.resize(84,88);

    m_Version.setParent(this);
    m_Version.move(115,29);
    m_Version.resize(218,20);
    m_Version.setText("PadLike Version 1.0");
    m_Version.setFont(QFont("楷书"));

    m_Author.setParent(this);
    m_Author.move(44,132);
    m_Author.resize(54,20);
    m_Author.setText("Author:");

    m_Authorurl.setParent(this);
    m_Authorurl.move(127,132);
    m_Authorurl.resize(113,20);
    m_Authorurl.setText("Arkenol");

    m_Mail.setParent(this);
    m_Mail.move(44,162);
    m_Mail.resize(81,19);
    m_Mail.setText("Email:");

    m_Mailurl.setParent(this);
    m_Mailurl.move(127,162);
    m_Mailurl.resize(146,19);
    m_Mailurl.setText("Arkenol@gmail.com");

    m_Homepage.setParent(this);
    m_Homepage.move(44,192);
    m_Homepage.resize(81,19);
    m_Homepage.setText("HomePage:");

    m_Homeurl.setParent(this);
    m_Homeurl.move(127,192);
    m_Homeurl.resize(235,19);
    m_Homeurl.setOpenExternalLinks(true);
    m_Homeurl.setText("https://www.github.com/Arkenol");

    m_btn.setParent(this);
    m_btn.move(156,379);
    m_btn.resize(88,24);
    m_btn.setText("确认");

    m_Editer.setParent(this);
    m_Editer.move(24,235);
    m_Editer.resize(377,115);
    m_Editer.setReadOnly(true);
    m_Editer.insertPlainText("Compile by QtCreator at 2018.01.01\n\n"
                             "Readme at https://www.github.com/Arkenol\n\n"
                             "Copyright @ 2010 Arkenol");


    QPalette p = palette();
    p.setColor(QPalette::Active, QPalette::Background, Qt::white);
    p.setColor(QPalette::Inactive, QPalette::Background, Qt::white);
    setPalette(p);

    setWindowTitle("About-PadLike");
    setFixedSize(420,427);
}

void HelpDiglog::connectSlot(){
    connect(&m_btn,SIGNAL(clicked()),this,SLOT(m_btnClicked()));
}

void HelpDiglog::m_btnClicked(){
    close();
}
