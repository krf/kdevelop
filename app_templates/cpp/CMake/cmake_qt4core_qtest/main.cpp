#include <QCoreApplication>
#include "%{APPNAME}.h"


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    %{APPNAMEID} foo;
    return app.exec();
}
