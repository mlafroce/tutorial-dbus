cmake_minimum_required(VERSION 3.1)

project(dbus-tutorial)

find_package(Qt5 COMPONENTS Core DBus)

# Permite llamar al "automoc", que es una herramienta que toma archivos.h con
# metadatos de Qt y expande sus macros
set(CMAKE_AUTOMOC ON)

# Este comando genera un archivo xml con la descripción de la interfaz de mi clase
# Esto se hace mediante el comando `qdbuscpp2xml`, que toma un header con la declaración
# de una clase (Un QObject), e itera sus slots y señales
qt5_generate_dbus_interface(dbusGreeter.h
  taller.hello.DbusGreeter.xml
  OPTIONS -A
)

# Este comando toma una descripción (en este caso, la generada en el paso anterior),
# una interfaz de una clase QObject y el nombre de la clase que queremos exponer en Dbus
# Se genera una clase *adaptor*, que envuelve a nuestra clase y la prepara para ser registrada en dBus
# Los archivos generados se guardan en la variable cuyo nombre es el primer parámetro de la función.
qt5_add_dbus_adaptor(dbusGreeterAdaptorSrcs
  ${CMAKE_CURRENT_BINARY_DIR}/taller.hello.DbusGreeter.xml
  dbusGreeter.h
  DbusGreeter
  )

# Imprimimos la variable con los nombres de los archivos generados
message(STATUS Gen adaptor: ${dbusGreeterAdaptorSrcs})

# Compilamos todas las fuentes en el ejecutable hello-dbus-service
add_executable(hello-dbus-service
  main.cpp
  dbusGreeter.cpp
  ${dbusGreeterAdaptorSrcs}
)

# Para este target además incluimos la carpeta donde estoy compilando y la carpeta raiz del proyecto como
# directorios de include (es decir, agrega `-I carpeta/del/proyecto` y `-I carpeta/de/compilacion` a los flags de g++)
target_include_directories(hello-dbus-service
  PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Linkea libQt5Widgets.so y libQt5DBus al ejecutable
target_link_libraries(hello-dbus-service Qt5::Core Qt5::DBus)