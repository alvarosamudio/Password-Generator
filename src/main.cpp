#include "mainwindow.h"
#include <DApplication>
#include <DWidgetUtil>
#include <DAboutDialog>
#include <QLocale>
#include <QTranslator>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    DApplication a(argc, argv);
    a.setOrganizationName("deepinenespanol");
    a.setApplicationName("passwordgenerator");
    a.setApplicationVersion("0.2.0");

    // Los textos fuente estan en espanol; para otros idiomas se carga la
    // traduccion embebida (:/i18n). Antes de crear cualquier texto.
    a.loadTranslator();
    QTranslator traductor;
    if (traductor.load(QLocale(), QStringLiteral("passwordgenerator"), QStringLiteral("_"),
                       QStringLiteral(":/i18n")))
        a.installTranslator(&traductor);
    a.setProductName(QObject::tr("Generador de contraseñas"));
    a.setApplicationDisplayName(QObject::tr("Generador de contraseñas"));
    a.setProductIcon(QIcon(":/passwordgenerator.svg"));
    a.setWindowIcon(QIcon(":/passwordgenerator.svg"));

    DAboutDialog dialog;
    dialog.setWindowTitle(a.productName());
    dialog.setProductName(a.productName());
    dialog.setProductIcon(a.productIcon());
    dialog.setCompanyLogo(QPixmap(":/Logo-Racoon.png"));
    dialog.setVersion(a.applicationVersion());
    dialog.setDescription(
        QObject::tr("Genera contraseñas aleatorias y seguras eligiendo su longitud y "
                    "complejidad.") +
        "<br/><br/><b>Deepin en Español</b> · Deepin Latin Code - developers");
    dialog.setWebsiteName("deepinenespanol.org");
    dialog.setWebsiteLink("https://deepinenespanol.org");
    a.setAboutDialog(&dialog);

    MainWindow w;
    w.show();
    Dtk::Widget::moveToCenter(&w);
    return a.exec();
}
