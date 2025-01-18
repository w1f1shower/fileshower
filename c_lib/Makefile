all: file_scanner.so metadata_injector.so

file_scanner.so: file_scanner.c
    gcc -shared -o file_scanner.so -fPIC file_scanner.c

metadata_injector.so: metadata_injector.c
    gcc -shared -o metadata_injector.so -fPIC metadata_injector.c
