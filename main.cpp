#include "mainwindow.h"
#include <DApplication>
#include <DWidgetUtil>
#include <DAboutDialog>

DWIDGET_USE_NAMESPACE
int main(int argc, char *argv[])
{
    DApplication a(argc, argv);
    DAboutDialog dialog;

     a.setProductName("Password Generator");
     a.setOrganizationName("deepinenespanol");
     a.setApplicationName("passwordgenerator");
     a.setApplicationVersion("0.2.0");
     a.setAboutDialog(&dialog);

     //Title
     dialog.setWindowTitle("Password Generator");
     //descritcion
     dialog.setProductName("<span>Generate random passwords by choosing the size and complexity</span>");
     //icons
     dialog.setCompanyLogo(QPixmap("://Logo-Racoon.png"));
     //about
     dialog.setDescription(
         "<span style=' font-size:8pt; font-weight:600;'>Deepin en Español </span>"
         "<a href='https://deepinenespanol.org'>https://deepinenespanol.org</a><br/>"
         "<span style=' font-size:8pt; font-weight:600;'>Deepin Latin Code - developers</span>");
     dialog.setVersion(a.applicationVersion());
     dialog.setWebsiteName("deepinenespanol.org");
     dialog.setWebsiteLink("https://deepinenespanol.org");


    MainWindow w;
    w.show();

    Dtk::Widget::moveToCenter(&w);
    return a.exec();
}
