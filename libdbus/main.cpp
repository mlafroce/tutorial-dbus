#include <dbus/dbus.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


DBusMessage* create_dbus_message(const char* message);
void send_message(const char* message);

int main(int argc, char** argv) {
    send_message("Hola!");
}


/**
 * Basado en tutorial de http://www.matthew.ath.cx/misc/dbus
 *
 * Otros links usados de referencia
 * https://www.softprayog.in/programming/d-bus-tutorial
 * Ver también https://www.exploit-db.com/exploits/7822
 */

void send_message(const char* message) {
    printf("Enviando %s\n", message);

    // Inicializa error value
    DBusError err;
    dbus_error_init(&err);

    DBusConnection* dbus_connection;
    // Me conecto a session bus y chequeo errores
    dbus_connection = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) { 
        fprintf(stderr, "Connection Error (%s)\n", err.message); 
        dbus_error_free(&err); 
    }
    if (NULL == dbus_connection) { 
        exit(1); 
    }

    // crea un method call
    DBusMessage* dbus_msg = create_dbus_message(message);

    DBusMessage* reply = dbus_connection_send_with_reply_and_block(dbus_connection, dbus_msg, -1, &err);

    if (!reply) {
        fprintf(stderr, "Out Of Memory!\n"); 
        exit(1);
    }
    
    char* reply_body;
    // Función variádica, recibe un mensaje a parsear, un lugar donde depositar errores ocurridos
    // y luego pares Tipo de dato, &variable para depositar los parámetros.
    // El último parámetro debe ser DBUS_TYPE_INVALID
    dbus_message_get_args(reply, &err, DBUS_TYPE_STRING, &reply_body, DBUS_TYPE_INVALID);
    printf("Recibido %s\n", reply_body);
    // Libero recursos
    dbus_message_unref(dbus_msg);
    dbus_connection_unref(dbus_connection);
    // Cierro todo el sistema de D-Bus (para que valgrind chille un poco menos)
    dbus_shutdown();
}

/**
 * Crea un mensaje de dbus para ser enviado
 */
DBusMessage* create_dbus_message(const char* message) {
    DBusMessage* dbus_msg = dbus_message_new_method_call("taller.hellodbus", // nombre del servicio 
                                    "/taller/greeter", // nombre del objeto
                                    "taller.DbusGreeter", // interfaz
                                    "printHello"); // nombre del método
    if (!dbus_msg) {
        fprintf(stderr, "Error al crear el mensaje\n"); 
        exit(1); 
    }

    DBusMessageIter args;
    dbus_message_iter_init_append(dbus_msg, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &message)) {
        fprintf(stderr, "Out Of Memory!\n"); 
        exit(1);
    }
    return dbus_msg;
}
