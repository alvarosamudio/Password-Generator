#include "mainwindow.h"
#include <DApplication>
#include <DWidgetUtil>
#include <DAboutDialog>

DWIDGET_USE_NAMESPACE
int main(int argc, char *argv[])
{
    DApplication::loadDXcbPlugin();
    DApplication a(argc, argv);
    DAboutDialog dialog;

     a.setAttribute(Qt::AA_UseHighDpiPixmaps);
     a.setProductName("Password Generator");
     a.setApplicationName("Password Generator");
     a.setAboutDialog(&dialog);

     //Title
     dialog.setWindowTitle("Password Generator");
     //descritcion
     dialog.setProductName("<span>Generate random passwords by choosing the size and complexity</span>");
     //icons
     dialog.setProductIcon(QIcon(":/imege/passwordgenerator.svg"));
     dialog.setCompanyLogo(QPixmap(":/image/deepines.svg"));
     //about
     dialog.setDescription(
         "<span style=' font-size:8pt; font-weight:600;'>Deepin en Español </span>"
         "<a href='https://deepinenespañol.org'>https://deepinenespañol.org</a><br/>"
         "<span style=' font-size:8pt; font-weight:600;'>Deepin Latin Code - developers</span>");
     dialog.setVersion(DApplication::buildVersion("Version 0.1"));
     dialog.setWebsiteName("deepinenespañol.org");
     dialog.setWebsiteLink("https://deepinenespañol.org");


    MainWindow w;
    w.show();

    Dtk::Widget::moveToCenter(&w);
    return a.exec();
}
