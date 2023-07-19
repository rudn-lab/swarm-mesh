esp32: esp32_compile esp32_upload
esp32_compile:
	arduino-cli compile --profile esp32
esp32_upload:
	arduino-cli upload -p $p --profile esp32

esp8266: esp8266_compile esp8266_upload
esp8266_compile:
	arduino-cli compile --profile esp8266
esp8266_upload:
	arduino-cli upload -p $p --profile esp8266

monitor:
	arduino-cli monitor -p $p -c baudrate=115200
