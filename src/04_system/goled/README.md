# GoLED

A Go application for controlling an LED on a NanoPi NEO Plus2 board using push buttons.

## Features

- Controls the flashing frequency of the status LED
- Uses three push buttons for control:
  - K1: Increase frequency
  - K2: Reset frequency to initial value
  - K3: Decrease frequency
- Auto-repeat functionality when buttons are held down
- Logs all frequency changes to syslog

## Building

Make sure you have Go installed. You can use the `get-go.sh` script provided in the `toolchain` directory in the system. Then build the application using the following command:

```bash
go build -o goled
```