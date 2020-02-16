#ifndef DBUS_GREETER_H
#define DBUS_GREETER_H
#include <QObject>

class DbusGreeter : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "taller.DbusGreeter") // El adaptador generado se llamará DbusGreeterAdaptor
public:
    DbusGreeter(QObject *parent);
    virtual ~DbusGreeter();
public slots: // 
    QString printHello(const QString& name);
signals:
    void emitHello();
};

#endif