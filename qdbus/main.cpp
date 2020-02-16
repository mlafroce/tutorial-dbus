#include <QCoreApplication>
#include <QDBusConnection>
#include "dbusGreeter.h"


int main(int argc, char **argv) {
    // Inicia event loop
    QCoreApplication app(argc, argv);
    // Instancia nuestro objeto "proxy"
    DbusGreeter greeter(nullptr);
    qInfo("Conectandose a session bus");
    QDBusConnection dbus = QDBusConnection::sessionBus();
    if (!QDBusConnection::sessionBus().isConnected()) {
        qWarning("No se puede conectar a D-Bus session bus.\n");
        return 1;
    } else {
        qInfo("sessionBus está conectado");
    }

    if (dbus.registerService("taller.hellodbus")) {
        qInfo("Registrado el servicio taller.hellodbus");
    } else {
        qInfo("Ya se encuentra registrado taller.hello");
    }
    if (dbus.registerObject("/taller/greeter", &greeter)) {
        qInfo("Registrado el objeto greeter");
    } else {
        qWarning("No se puede registrar a greeter.\n");
    }
    emit greeter.emitHello();
    return app.exec();
}
