#include "MainWindow.h"
#include <QApplication>
#include <QIcon>
#include <QStyleFactory>


int main(int argc, char** argv) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");
    app.setWindowIcon(QIcon(":/icons/icons/icon.svg"));
    MainWindow w;
    w.resize(1200, 700);
    w.show();
    return app.exec();
}