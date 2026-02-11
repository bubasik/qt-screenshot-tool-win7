#include <QApplication>
#include "screenshottool.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Устанавливаем иконку приложения (опционально)
    // app.setWindowIcon(QIcon(":/icons/app.png"));
    
    ScreenshotTool tool;
    tool.setWindowTitle("📸 Скриншотер • Qt 5.12.12");
    tool.resize(600, 500);
    tool.show();
    
    return app.exec();
}
