#include "mainwindow.h"
#include <DApplication>
#include <DWidgetUtil>

DWIDGET_USE_NAMESPACE
int main(int argc, char *argv[])
{
    DApplication::loadDXcbPlugin();
    DApplication a(argc, argv);

     a.setAttribute(Qt::AA_UseHighDpiPixmaps);
     a.setProductName("Password Generator");
     a.setApplicationName("Password Generator");
     a.setOrganizationName("deepinenespañol");
     a.setApplicationVersion(DApplication::buildVersion("0.1"));
     a.setApplicationAcknowledgementPage("deepinenespañol.org");
     //a.setProductIcon(QIcon::fromTheme("passwordgenerator"));

    MainWindow w;
    w.show();

    Dtk::Widget::moveToCenter(&w);
    return a.exec();
}
