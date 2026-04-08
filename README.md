# ESP32 GPIO Control API

HTTP API для управления GPIO пинами на ESP32 через WiFi.

## Возможности

- Управление 8 GPIO пинами (0-8)
- Встроенный веб-интерфейс
- REST API для интеграции
- Поддержка WiFi

## Требования

- ESP32 DevKit (или аналог)
- USB кабель для прошивки
- WiFi сеть

## Аппаратное подключение

### Распиновка ESP32

```
GPIO 0  → Кнопка/реле 0
GPIO 1  → Кнопка/реле 1
GPIO 2  → Кнопка/реле 2
GPIO 3  → Кнопка/реле 3
GPIO 4  → Кнопка/реле 4
GPIO 5  → Кнопка/реле 5
GPIO 6  → Кнопка/реле 6
GPIO 7  → Кнопка/реле 7
GPIO 8  → Кнопка/реле 8
GND     → GND
```

### Подключение реле

```
ESP32 GPIO → (+) обмотка реле
ESP32 GND  → (-) обмотка реле
```

## Установка

### 1. Установка PlatformIO

**Windows (choco):**
```powershell
choco install platformio
```

**Windows (ручная установка):**
1. Скачайте и установите Python: https://www.python.org/downloads/
2. Установите PlatformIO:
```powershell
pip install platformio
```

**Linux/Mac:**
```bash
pip install platformio
```

### 2. Настройка проекта

Отредактируйте файл `src/main.cpp`, указав свои данные WiFi:

```cpp
const char* ssid = "ВАШ_SSID";
const char* password = "ВАШ_ПАРОЛЬ";
```

### 3. Сборка и прошивка

Подключите ESP32 к компьютеру через USB.

Определите COM порт:
```powershell
# Windows
pio device list

# Linux/Mac
pio device list
```

Прошейте устройство:
```powershell
pio run --target upload --environment esp32dev --upload-port COM3
```
(замените COM3 на ваш порт)

Мониторинг:
```powershell
pio device monitor --baud 115200
```

## Использование

### Веб-интерфейс

После успешной прошивки откройте в браузере:
```
http://<IP_ESP32>
```

IP адрес появится в мониторе последовательного порта при загрузке.

### REST API

**Включить пин:**
```bash
curl -X POST http://<IP>/gpio/4/on
```

**Выключить пин:**
```bash
curl -X POST http://<IP>/gpio/4/off
```

**Переключить пин:**
```bash
curl -X POST http://<IP>/gpio/4/toggle
```

**Получить состояние пина:**
```bash
curl http://<IP>/gpio/4/state
```
Ответ: `{"gpio":4,"state":0}`

**Получить состояние всех пинов:**
```bash
curl http://<IP>/state
```
Ответ: `{"gpio0":0,"gpio1":0,"gpio2":0,...}`

### Примеры интеграции

**Python:**
```python
import requests

def gpio_on(pin):
    requests.post(f"http://<IP>/gpio/{pin}/on")

def gpio_off(pin):
    requests.post(f"http://<IP>/gpio/{pin}/off")
```

**JavaScript:**
```javascript
async function setGpio(pin, state) {
  await fetch(`/gpio/${pin}/${state ? 'on' : 'off'}`, { method: 'POST' });
}
```

## Устранение проблем

### Не подключается к WiFi

1. Проверьте SSID и пароль в `main.cpp`
2. Убедитесь, что WiFi работает на 2.4 GHz (ESP32 не поддерживает 5 GHz)
3. Проверьте последовательный монитор на наличие ошибок

### Не прошивается

1. Убедитесь, что драйверы CH340/CP2102 установлены
2. Нажмите кнопку BOOT на ESP32 при прошивке
3. Проверьте COM порт

### Драйверы Windows

- CH340: https://www.wch.cn/download/CH341SER_ZIP.html
- CP2102: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers

## Структура проекта

```
raspberri/
├── src/
│   └── main.cpp      # Основной код
├── platformio.ini    # Конфигурация PlatformIO
└── README.md         # Этот файл
```