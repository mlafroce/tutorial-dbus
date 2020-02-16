#include "dbusGreeter.h"
#include "dbusgreeteradaptor.h"

DbusGreeter::DbusGreeter(QObject *parent) : QObject(parent) {
    new DbusGreeterAdaptor(this);
    qInfo("Construyendo dbusGreeter");
}

QString DbusGreeter::printHello(const QString& name) {
    QString greet = QString("Hello %1").arg(name);
    qInfo("Llamado printHello! devolviendo %s", greet.toStdString().c_str());
    return greet;
}

DbusGreeter::~DbusGreeter() {};