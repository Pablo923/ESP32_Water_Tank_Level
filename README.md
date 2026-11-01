# ESP32 Water Tank Level

Firmware para ESP32 basado en ESP-IDF. El proyecto está configurado para el chip
`esp32` y su `sdkconfig` fue generado con ESP-IDF 5.1.1.

## Preparar ESP-IDF

Instala ESP-IDF 5.1.1 siguiendo la guía oficial de instalación para tu sistema:
[Espressif ESP-IDF Get Started](https://docs.espressif.com/projects/esp-idf/en/v5.1.1/esp32/get-started/index.html).
En macOS/Linux, los comandos de `make` cargan automáticamente el entorno desde
`$HOME/esp/esp-idf`. Si instalas ESP-IDF en otra ruta, indícala al ejecutar
`make`, por ejemplo `make build IDF_PATH=/ruta/a/esp-idf`. Para invocar `idf.py`
directamente, carga primero el entorno con el script `export.sh`.

En macOS/Linux, si ESP-IDF está instalado en `~/esp/esp-idf`:

```sh
. "$HOME/esp/esp-idf/export.sh"
```

En Windows, usa el acceso directo **ESP-IDF 5.1.1 PowerShell** creado por el
instalador. Se necesita Python, CMake, Ninja y el toolchain para ESP32; el
instalador oficial los administra.

## Compilar

Desde la raíz del repositorio:

```sh
make build
```

El primer build también genera el directorio `build/`. Si la configuración de
objetivo no coincide, ejecuta una vez `make set-target`.

También puedes invocar ESP-IDF directamente: `idf.py build`.

## Grabar y ver la salida serie

Conecta la placa por USB y localiza el puerto serie. Por ejemplo:

- macOS: `/dev/cu.usbserial-XXXX` o `/dev/cu.usbmodemXXXX`
- Linux: `/dev/ttyUSB0` o `/dev/ttyACM0`
- Windows: `COM3`

Luego ejecuta:

```sh
make flash PORT=/dev/cu.usbserial-XXXX
make monitor PORT=/dev/cu.usbserial-XXXX
```

Para grabar y dejar abierto el monitor en una sola operación:

```sh
make flash-monitor PORT=/dev/cu.usbserial-XXXX
```

La velocidad serie predeterminada es 115200 baudios; se puede cambiar con
`BAUD=...`. Sal del monitor con `Ctrl+]`. Si la placa no entra automáticamente
en modo de descarga, mantén presionado **BOOT** mientras comienza la grabación.

`make erase-flash PORT=...` borra toda la memoria flash de la placa.

## VS Code

Las tareas de VS Code usan `make`; el Makefile carga automáticamente ESP-IDF
desde `$HOME/esp/esp-idf`. La tarea de grabación solicita el puerto serie.
También se puede usar `make` desde la terminal integrada.

- **Compilar:** `Cmd+Shift+B` en macOS (`Ctrl+Shift+B` en Windows/Linux).
- **Flashear:** abre **Preferences: Open Keyboard Shortcuts (JSON)** y agrega
  el contenido de `.vscode/keybindings.example.json`. El atajo es `Cmd+Alt+F`
  en macOS; ajusta `key` a `ctrl+alt+f` en Windows/Linux. Al ejecutarlo, VS Code
  pedirá el puerto serie.

VS Code solo lee atajos desde el archivo de teclado del usuario; por eso el
repositorio incluye un ejemplo para copiar y no una configuración que la
aplicación ignoraría.

## Configuración de la aplicación

Antes de usar el firmware en la red real, configura las credenciales Wi-Fi y el
broker MQTT según tu instalación. No guardes contraseñas ni otros secretos en
el repositorio. El archivo `sdkconfig` fija actualmente el objetivo ESP32 y
parámetros de flash de 2 MB; confirma que coincidan con tu placa.
