# D-Bus

**D-Bus** Es un sistema de comunicación entre procesos y llamadas a procedimientos remotos, que surgió a partir de la necesidad de estandarizar los servicios de escritorio ofrecidos principalmente por *KDE* y *GNOME*. Además de comunicar procesos de escritorio entre sí, también se puede comunicar con procesos del sistema operativo. El protocolo es parte del proyecto *freedesktop.org*.

El sistema *D-Bus* posee un servicio corriendo en segundo plano (*daemon*) que utiliza utiliza 2 canales (buses) para comunicarse con el resto de las aplicaciones. Estos buses son el **System bus** y **Session bus**, correspondiendo al canal global del sistema y al de la sesión del usuario respectivamente. Estos buses están implementados sobre sockets de tipo *unix* (o TCP en el caso de windows); en el caso de unix, la dirección del *System bus* es `/run/dbus/system_bus_socket`, mientras que la de sesión está en la variable `DBUS_SESSION_BUS_ADDRESS` de la sesión, por lo que podemos ejecutar en una terminal

~~~
echo $DBUS_SESSION_BUS_ADDRESS
~~~

Y nos devuelve `unix:path=/run/user/1000/bus`

Existe una biblioteca de bajo nivel, llamada **libdbus**, pero los mismos desarrolladores de la biblioteca recomiendan utilizar bibliotecas una capa más arriba de esta (como *GDBus* o *QDbus* para Glibc o Qt respectivamente), ya que sino el usuario se estaría "suscribiendo a algo de dolor". Sin embargo, en este tutorial nos centraremos más en el protocolo que utiliza *libdbus*.

## Casos de uso de DBus

Varias aplicaciones usan D-Bus para comunicarse con el sistema. Desde servicios como *networkmanager* para administrar las conexiones de red, o *pulseaudio*, el motor de sonido, a aplicaciones gráficas populares como *Sublime* o *Spotify*. Algunas aplicaciones usan dbus únicamente para consultar el estado de otros servicios (como en el caso de firefox), mientras que otras ofrecen una interfaz (API) para llamar métodos de la aplicación. Por ejemplo, *Spotify* implementa varios métodos de la interfaz [MediaPlayer2](https://specifications.freedesktop.org/mpris-spec/latest/Player_Interface.html), permitiendo controlar la reproducción de audio mediante la llamada a estas funciones.

Ejemplo:

~~~
dbus-send --print-reply --dest=org.mpris.MediaPlayer2.spotify /org/mpris/MediaPlayer2 org.mpris.MediaPlayer2.Player.PlayPause
dbus-send --print-reply --dest=org.mpris.MediaPlayer2.spotify /org/mpris/MediaPlayer2 org.mpris.MediaPlayer2.Player.Next
~~~

Además de llamar procedimientos remotos, los procesos pueden emitir "señales", es decir, mensajes notificando un evento (no confundir con las señales del sistema). Para ver el tráfico a través de *D-bus*, utilizamos el comando *dbus-monitor*.

## Ejemplo 1: Recibiendo mensajes con con QDBusSession

En este ejemplo, en C++, utilizaremos el módulo *QDbus* del **framework Qt5** para crear una aplicación que permite la llamada a procedimientos desde DBus.

El código del mismo se encuentra en la carpeta *qdbus*, y se compila utilizando *cmake*

El ejemplo contiene un archivo *main.cpp* donde se encuentra la función `main()` de las cuales extraemos las siguientes lineas relevantes:


### main.cpp del receptor

~~~
QCoreApplication app(argc, argv);
~~~
Al ser una aplicación de *Qt*, instanciamos `QCoreApplication` para inicializar el framework. Esta clase se encarga de manejar un loop de eventos, señales, dispositivos, etc.

~~~
DbusGreeter greeter(nullptr);
~~~
Este objeto DbusGreeter es de una clase que creamos que hereda de QObject. Es una clase común de C++ pero con anotaciones de Qt para extender su funcionalidad

~~~
QDBusConnection dbus = QDBusConnection::sessionBus();
~~~
Apertura del *session bus*. Qt se encarga de gestionar la conexión al mismo.

~~~
if (dbus.registerService("taller.hellodbus")) {
~~~
Registrar el servicio. Es el identificador de los proveedores de servicios conectados a D-Bus, y será utilizado por los clientes que quieran enviarle mensajes a esta aplicación.
   
~~~
if (dbus.registerObject("/taller/greeter", &greeter)
~~~
Nombre del objeto que estoy "publicando". Puedo tener varios objetos sobre un mismo servicio

~~~
return app.exec();
~~~
Inicia el loop principal de Qt. En este loop Qt escucha y procesa todo tipo de eventos que le llegan a la aplicación. 


### DBusGreeter

El servicio que exponemos está implementado en una clase DBusGreeter. Aunque la misma es sencilla, posee varios macros pertenecientes a Qt

Primero describimos algunas lineas del header:

~~~
    Q_OBJECT
~~~

Esta macro genera las implementaciones de algunos métodos que permiten instrospección y metadatos utilizados por Qt. Se utiliza frecuentemente en el framework con diversos fines.

~~~
Q_CLASSINFO("D-Bus Interface", "taller.DbusGreeter") // El adaptador generado se llamará DbusGreeterAdaptor
~~~

Metadatos para generar la interfaz Dbus. En particular, las herramientas que generan los archivos de interfaces toman el último nombre y le agregan el sufijo "Adaptor" para crear una clase que interactúe con DBus.

~~~
public slots: 
    QString printHello(const QString& name);
~~~

Son métodos que pueden conectarse a "señales", utilizados fuertemente en programación orientada a eventos.

Por otra parte, en el .cpp nos encontramos con esta linea:

~~~
#include "dbusgreeteradaptor.h"
~~~

Este archivo no está en el proyecto, es un archivo autogenerado por ciertas herramientas de Qt que se pueden ver con más detalle en el archivo de compilación.

### Compilación

Como dependencias tendremos que instalar

* cmake

* qtbase5-dev

* libdbus-1-dev

Qt tiene la particularidad de usar su propio script de compilación, haciendo transparente las invocaciones a herramientas, recursos, etc. Este tutorial utiliza *CMake*, ya que es más explicito (menos "mágico") e independiente de Qt y su compilador *qmake*.

En una terminal creamos una carpeta (por ejemplo "build"), nos movemos a esa carpeta y llamamos a cmake sobre el directorio donde se encuentra nuetro `CMakeLists.txt`

~~~
mkdir build
cd build
cmake ..
~~~

## Ejecución y monitoreo

La actividad en el bus se puede monitorear utilzando la aplicación `dbus-monitor`. Por defecto monitorea el *session bus*, que es que utilizaremos.

Una vez que compilamos la aplicación podemos ejecutarla desde la terminal. Una vez iniciada podemos usar la utilidad `dbus-send` para comunicarnos con el bus y preguntar que servicios están disponibles. Preguntamos por los servicios en el *session bus*

~~~
dbus-send --session            \
  --dest=org.freedesktop.DBus \
  --type=method_call          \
  --print-reply               \
  /org/freedesktop/DBus       \
  org.freedesktop.DBus.ListNames
~~~

Recibiremos un array de strings, y uno de sus items deberá ser `string "taller.hellodbus"`. Esto nos indica que el servicio "taller.hellodbus" está disponible.

También, si preferimos usar interfaz gráfica, podemos instalar la herramienta **d-feet**

Si queremos enviarle un mensaje a nuestro servicio, podemos ejecutar por terminal

~~~
dbus-send --print-reply --dest=taller.hellodbus /taller/greeter taller.DbusGreeter.printHello string:"Pepe"
~~~

Donde *taller.hellodbus* es el nombre del servicio con el que lo publicamos, */taller/greeter* es el nombre del objeto sobre el que queremos actuar, *taller.DbusGreeter* es la interfaz que configuramos con la macro `Q_CLASSINFO` en *DbusGreeter.h*, y *printHello* es el nombre del slot que quiero llamar.

Al ejecutar este comando veremos que la respuesta es "Hello pepe" y se informa del llamado en la aplicación "servidor".

Si utilizamos *d-feet*, seleccionamos el tab de *session bus*, buscamos "taller", y seleccionamos el servicio, el objeto */taller/greeter/* y nuestro método *printHello*. Al hacer doble click sobre el mismo se nos abre un popup donde podemos enviar los parámetros del método. Recordar que los strings van entre comillas.

## Ejemplo 2: llamando un método con dbus

Para este ejemplo utilizaremos la biblioteca libdbus de forma directa. En vez de usar CMake usamos un makefile directo. Para obtener las dependencias usamos `pkg-config`, que nos devuelve los flags necesarios para que gcc pueda encontrar los includes y las bibliotecas contra las que tiene que hacer link.

Este código es más sencillo, algunas de las lineas más relevantes son:

~~~
dbus_connection = dbus_bus_get(DBUS_BUS_SESSION, &err);
~~~
Abrimos una conexión con el bus

~~~
DBusMessage* dbus_msg = dbus_message_new_method_call(
~~~

Creamos un mensaje del tipo "method_call", para realizar una llamada a un procedimiento remoto (printHello). Existen otros tipos de mensajes, como Signal.

~~~
dbus_message_iter_init_append(dbus_msg, &args);
~~~
Me permite ir armando los objetos que voy a pasarle como parámetros al método que estoy llamando.

~~~
dbus_message_get_args
~~~
Función variádica que me permite deserializar el cuerpo de un mensaje (en particular el de la respuesta recibida al llamar `printHello`)

Esta es la parte 1 del tutorial, en la siguiente parte se describe a fondo como es el protocolo de comunicación que utiliza D-Bus

## Para seguir expandiendo

Para investigar un poco, se puede usar el comando `strace -e %network` para observar las llamadas a syscalls relacionadas a sockets

