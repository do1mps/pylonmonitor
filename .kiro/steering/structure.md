# Project Structure

## Repository Root

All C++ source files live flat in the project root. There are no subdirectories for source code.

```
pylonmonitor/
├── pylonmonitor.cpp / .h   # Entry point, main loop, startup/shutdown
├── readbatt.cpp / .h       # Battery polling engine: Pylontech protocol state machine,
│                           #   data parsing, JSON serialization, MQTT publish calls
├── mqttthread.cpp / .h     # MQTT client lifecycle: connect, callbacks, publish drain
├── fifo.cpp / .h           # Inter-thread bridge: boost::lockfree::spsc_queue wrapping
│                           #   send_to_mqtt() / read_from_readbatt()
├── config.cpp / .h         # JSON config persistence (/var/www/html/wxdata/configData.json),
│                           #   change detection, battery capacity tables
├── helper.cpp / .h         # System utilities: local IP, SSID/RSSI, CPU temp,
│                           #   signal handling, duplicate-process check
├── serial.cpp / .h         # Primary UART abstraction (open/read/write wrappers
│                           #   for /dev/ttyAMA0 at 115200 baud)
├── serial_helper.cpp / .h  # Serial port manager: spawns one pthread per port,
│                           #   termios setup, non-blocking byte read/write loops
├── identifySerUSB.cpp / .h # Maps serial/vendor ID strings to /dev/ttyU* device nodes
│                           #   via udevadm (used for USB-serial ports)
├── kmfifo.cpp / .h         # Generic mutex-guarded FIFO; used internally by serial layer
├── html/                   # Web UI files deployed to Apache
│   ├── index.html          # Local dashboard SPA (polls batteryinfo.json every 1 s)
│   ├── setup.html          # MQTT config form
│   ├── savedata.php        # Receives POST from setup.html, writes configData.json
│   └── defaultdata.php     # Returns configData.json as JSON for the config form
├── pics/                   # Documentation images
├── Makefile                # Build system
├── prepare                 # One-time apt-get install script
└── autostart.txt           # Systemd unit instructions for boot autostart
```

## Runtime File Paths (outside repo)

| Path | Purpose |
|---|---|
| `/var/www/html/index.html` | Dashboard, copied from `html/index.html` on each startup |
| `/var/www/html/wxdata/configData.json` | MQTT and topic config (created by daemon, edited by PHP) |
| `/var/www/html/wxdata/batteryinfo.json` | Live battery data written every read cycle |

## Threading Model

```
main thread
  ├── MQTTTHREAD (mosquitto internal network thread, started via mosquitto_loop_start)
  └── READBATT → std::thread (battery state machine, detached)
                   └── serial_helper → pthread (one per serial port)
```

Inter-thread data flows in one direction only: battery thread → `boost::lockfree::spsc_queue` → main loop → MQTT publish.

## Key Architectural Constraints

- The lock-free SPSC queue in `fifo.cpp` has exactly one producer (battery thread) and one consumer (main thread). Do not add additional producers or consumers without replacing it with a suitable multi-producer/consumer structure.
- Configuration is read from disk by `readConfigFromJson()` on each MQTT topic build, not cached at startup. Changes are detected by comparing broker credentials against static locals; a detected change sets the global `mqtt_changed` flag.
- The compile-time `#define` in `config.h` (`DJ0ABR` or `DL1EV`) selects which battery capacity table is compiled in. This must be set before building.
- The program must run as **root** (`/dev/ttyAMA0` requires it); Apache files are owned by `www-data`.
