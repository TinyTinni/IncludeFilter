// IncludeFilter.cpp : Defines the entry point for the application.
//

#include "ui/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(QStringLiteral(":icons/windowIcon.png")));
    MainWindow w;
    w.show();
    return app.exec();
}
