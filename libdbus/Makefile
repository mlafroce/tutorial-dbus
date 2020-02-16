CFLAGS += `pkg-config --cflags --libs dbus-1`

hello-sender: main.cpp
	gcc -g main.cpp $(CFLAGS) -o hello-sender

clean:
	$(RM) hello-sender

.PHONY: clean
