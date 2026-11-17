# Tech Stack

## Language & Standard
- C++11 (`-std=c++0x`)
- Compiler: `g++` with `-Wall -O3 -Wno-write-strings -Wno-narrowing`

## Libraries
| Library | Purpose |
|---|---|
| **libmosquitto** | MQTT client (Eclipse Mosquitto C library) |
| **libjansson** | JSON encoding/decoding (`-ljansson`) |
| **Boost.Lockfree** | `boost::lockfree::spsc_queue` for the inter-thread message queue (`libboost-dev`) |
| **pthreads** | Threading (`-lpthread`); mix of `std::thread` and raw `pthread_create` |
| **termios / Linux serial** | Raw UART access, non-blocking, 8N1 |
| **Apache 2 + PHP** | Serves the local web dashboard and config form |

## Build System
GNU Make. All `*.cpp` files in the project root are compiled automatically.  
Auto-dependency tracking: each `.cpp` produces a `.d` file that is `include`d to handle header changes.

### Common Commands
```bash
make          # compile everything → produces ./pylonmonitor binary
make clean    # remove all .o, .d files and the binary
./prepare     # apt-get install all required system packages (run once on a new Pi)
```

### Linker flags
```
-pthread -lpthread -lm -lmosquitto -ljansson
```

## Runtime Dependencies
- Apache 2 with PHP enabled (serves `/var/www/html/`)
- `www-data` user/group must exist (config file ownership)
- Program must run as **root** to access `/dev/ttyAMA0`
- `iwconfig` must be available for SSID/RSSI reads
- Systemd unit or cron for autostart — see `autostart.txt`

## Compile-time Configuration
`config.h` contains two mutually exclusive `#define` profiles that select the `batteryEnergy[]` and `batteryCapacity[]` arrays:
- `DJ0ABR` — 8-battery system (7 × 70 Ah, 1 × 47 Ah)
- `DL1EV`  — 2-battery system (2 × 70 Ah)

Uncomment the appropriate define before building.

## No Test Suite
There are no automated tests in this project.
