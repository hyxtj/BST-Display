/***************************************************************************
  文件名称：main.cpp
  功    能：窗口实例创建
  说    明：
***************************************************************************/
#include <QApplication>
#include "BSTWindow.h"

int main(int argc, char* argv[]) {
    
    QApplication app(argc, argv); 

    BSTWindow window;
    window.show();

    return app.exec();
}