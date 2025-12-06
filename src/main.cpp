#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("Remote System");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("RemoteSystem");
    
    MainWindow window;
    window.show();
    
    return app.exec();
}

