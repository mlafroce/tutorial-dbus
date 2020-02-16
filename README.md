Este tutorial muestra un parte del sistema de comunicación D-Bus. Comenzamos con una breve descripción del mismo, casos de uso y una implementación de una aplicación que permite llamadas a procedimientos remotos y otra que realiza estas llamadas. Esta parte del tutorial se encuentra en la [parte 1](tutorial-parte-1.md)
En la parte 2 se describe la especificación del protocolo.

Para la parte 1 se requiere tener instalado:

* `libdbus-1-dev` (para el "cliente")

* `qt5-default` (para usar bibliotecas de Qt5)

* `cmake` (para compilar el "servidor")

El "cliente" se encuentra en la carpeta *libdbus*, y está implementado en C, mientras que el "servidor" está implementado en C++ y se encuentra en *qdbus*.

