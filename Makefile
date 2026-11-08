IDF_TARGET ?= esp32
IDF_PATH ?= $(HOME)/esp/esp-idf
IDF_COMMAND = . "$(IDF_PATH)/export.sh" >/dev/null && idf.py
PORT ?=
BAUD ?= 115200

.PHONY: help set-target build flash monitor flash-monitor menuconfig erase-flash clean

help:
	@printf '%s\n' \
	  'Comandos disponibles:' \
	  '  make set-target  Configura ESP-IDF para $(IDF_TARGET)' \
	  '  make build       Compila el firmware' \
	  '  make flash       Compila y graba (PORT=/dev/ttyUSB0)' \
	  '  make monitor     Abre el monitor serie' \
	  '  make flash-monitor  Graba y abre el monitor' \
	  '  make menuconfig  Configura opciones de ESP-IDF' \
	  '  make erase-flash Borra toda la memoria flash' \
	  '  make clean       Elimina los artefactos de compilación'

set-target:
	$(IDF_COMMAND) set-target $(IDF_TARGET)

build:
	$(IDF_COMMAND) build

flash:
	@test -n "$(PORT)" || (echo 'Indica el puerto serie: make flash PORT=/dev/ttyUSB0' >&2; exit 2)
	$(IDF_COMMAND) -p "$(PORT)" -b $(BAUD) flash

monitor:
	@test -n "$(PORT)" || (echo 'Indica el puerto serie: make monitor PORT=/dev/ttyUSB0' >&2; exit 2)
	$(IDF_COMMAND) -p "$(PORT)" -b $(BAUD) monitor

flash-monitor:
	@test -n "$(PORT)" || (echo 'Indica el puerto serie: make flash-monitor PORT=/dev/ttyUSB0' >&2; exit 2)
	$(IDF_COMMAND) -p "$(PORT)" -b $(BAUD) flash monitor

menuconfig:
	$(IDF_COMMAND) menuconfig

erase-flash:
	@test -n "$(PORT)" || (echo 'Indica el puerto serie: make erase-flash PORT=/dev/ttyUSB0' >&2; exit 2)
	$(IDF_COMMAND) -p "$(PORT)" erase-flash

clean:
	$(IDF_COMMAND) fullclean
