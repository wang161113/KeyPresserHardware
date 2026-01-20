#include <QApplication>
#include "keypresserHardware.h"
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // 设置全局编码为UTF-8
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    
    // 加载QSS样式表
    QFile styleFile(":/style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&styleFile);
        app.setStyleSheet(stream.readAll());
        styleFile.close();
    }

    KeyPresserHardware keyPresser;
    keyPresser.setWindowIcon(QIcon(":/keypresser.ico"));
    keyPresser.setWindowTitle(QStringLiteral("KeyPresser硬件版"));

#ifdef QT_NO_DEBUG
    //if(!keyPresser.checkArduino()) { return -1; }
#endif

    keyPresser.show();
    return app.exec();
}
