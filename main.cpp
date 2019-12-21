#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QPixmap>
#include <QSplashScreen>
#include <QTimer>
#include <QTranslator>

#include <QDebug>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(Image);                     //加载qrc
    QApplication a(argc, argv);

    QPixmap Loginlogo(":/Icons/icons/logo.png");
    QSplashScreen splash(Loginlogo);

    for(qint64 i = 200000000;i>0;--i)
        splash.show();

    //splash.showMessage("Opening...");

    QCoreApplication::setOrganizationName("Arkenol in Hust");
    QCoreApplication::setApplicationName("Padlike");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;                      //命令行支持
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();         //-h 添加帮助信息
    parser.addVersionOption();              //-v 添加版本信息
    //QCommandLineOption coloroption(QStringList()<<"color",QGuiApplication::translate("main","the color of the windows area"),QGuiApplication::translate("main","color"),"0");
    //parser.addOption(coloroption);
    parser.addPositionalArgument("file","The file to open");        //添加文件参数
    parser.process(a);

    //QString color = parser.value(coloroption);
    //MainWindow.setcolor(QColor(color));

    QTranslator t;              //语言,有部分未翻译。
    if(t.load(":/qm/qm/qt_zh_CN.qm")){
        a.installTranslator(&t);
        //qDebug()<<"ok";
    }

    // 安装语言的另一种方式 记载 qt 库实例的信息
    // QTranslator t;
    // if( t.load("qt_zh_CN.qm", QLibraryInfo::location(QLibraryInfo::TranslationsPath)) )
    // {
    //    a.installTranslator(&t);
    // }


    MainWindow *w = MainWindow::NewInstance();              //单例模式
    int status = -1;

    if(!parser.positionalArguments().isEmpty()){            //文件参数处理
        QFileInfo info(parser.positionalArguments().first());
        if(info.exists()){
            w->OpenFile(parser.positionalArguments().first());
        }
        else{
            QMessageBox::warning(nullptr,"警告",QString("文件 %1 不存在！").arg(parser.positionalArguments().first()),"确定");
        }
    }

    /*if(w!=nullptr){
        if(argc > 1){
            QFileInfo info(argv[1]);

            if(info.exists()){
                w->OpenFile(argv[1]);
            }
        }
    }*/

    w->show();

    splash.finish(w);

    status = a.exec();

    return status;
}
