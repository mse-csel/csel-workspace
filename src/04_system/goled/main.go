package main

import (
	"fmt"
	"log/syslog"
	"os"
	"os/signal"
	"strconv"
	"time"
	"unsafe"

	"golang.org/x/sys/unix"
)

const (
	// GPIO paths
	gpioExport   = "/sys/class/gpio/export"
	gpioUnexport = "/sys/class/gpio/unexport"

	// Pin assignments based on the schematic
	ledPin = "10" // Status LED
	k1Pin  = "0"  // K1 button - GPIOA0
	k2Pin  = "2"  // K2 button - GPIOA2
	k3Pin  = "3"  // K3 button - GPIOA3

	// Frequency settings
	initialFreq   = 2.0  // Initial frequency in Hz
	freqIncrement = 0.5  // Frequency change increment in Hz
	minFreq       = 0.5  // Minimum frequency in Hz
	maxFreq       = 10.0 // Maximum frequency in Hz
)

// Helper function to write to sysfs files
func writeFile(filename, data string) error {
	return os.WriteFile(filename, []byte(data), 0644)
}

// Helper function to read from sysfs files
func readFile(filename string) (string, error) {
	content, err := os.ReadFile(filename)
	if err != nil {
		return "", err
	}
	return string(content), nil
}

// Configure a GPIO pin
func setupGPIO(pin, direction string, edge string) error {
	// Unexport first to ensure clean state
	_ = writeFile(gpioUnexport, pin)

	// Export the pin
	if err := writeFile(gpioExport, pin); err != nil {
		return fmt.Errorf("failed to export GPIO pin %s: %v", pin, err)
	}

	// Wait a moment for sysfs entries to be created
	time.Sleep(100 * time.Millisecond)

	// Set direction
	dirPath := fmt.Sprintf("/sys/class/gpio/gpio%s/direction", pin)
	if err := writeFile(dirPath, direction); err != nil {
		return fmt.Errorf("failed to set direction for GPIO pin %s: %v", pin, err)
	}

	// Set edge detection if provided and pin is input
	if edge != "" && direction == "in" {
		edgePath := fmt.Sprintf("/sys/class/gpio/gpio%s/edge", pin)
		if err := writeFile(edgePath, edge); err != nil {
			return fmt.Errorf("failed to set edge for GPIO pin %s: %v", pin, err)
		}
	}

	return nil
}

// Set LED state
func setLED(pin string, state bool) error {
	value := "0"
	if state {
		value = "1"
	}

	return writeFile(fmt.Sprintf("/sys/class/gpio/gpio%s/value", pin), value)
}

func main() {
	// Setup syslog
	logger, err := syslog.New(syslog.LOG_INFO|syslog.LOG_DAEMON, "goled")
	if err != nil {
		fmt.Fprintf(os.Stderr, "Failed to connect to syslog: %v\n", err)
		os.Exit(1)
	}
	defer logger.Close()

	logger.Info("Starting goled application with debug logging")
	logger.Info(fmt.Sprintf("Using LED pin: %s", ledPin))
	logger.Info(fmt.Sprintf("Using button pins: K1=%s, K2=%s, K3=%s", k1Pin, k2Pin, k3Pin))

	// Setup signal handling for graceful shutdown
	sigChan := make(chan os.Signal, 1)
	signal.Notify(sigChan, os.Interrupt, unix.SIGTERM)

	// Setup GPIO for LED and buttons
	pins := []struct {
		pin       string
		direction string
		edge      string
	}{
		{ledPin, "out", ""},
		{k1Pin, "in", "both"},
		{k2Pin, "in", "both"},
		{k3Pin, "in", "both"},
	}

	// Configure all GPIO pins
	for _, p := range pins {
		if err := setupGPIO(p.pin, p.direction, p.edge); err != nil {
			logger.Err(err.Error())
			fmt.Fprintf(os.Stderr, "Error: %v\n", err)
			os.Exit(1)
		}
	}
	logger.Info("GPIO pins configured successfully")

	// Cleanup function to be called on exit
	cleanup := func() {
		logger.Info("Cleaning up resources")
		setLED(ledPin, false)
		for _, p := range pins {
			writeFile(gpioUnexport, p.pin)
		}
	}
	defer cleanup()

	// Open value files for buttons
	buttons := make(map[string]*os.File)
	buttonFds := make(map[string]int)

	for _, p := range []string{k1Pin, k2Pin, k3Pin} {
		path := fmt.Sprintf("/sys/class/gpio/gpio%s/value", p)
		f, err := os.Open(path)
		if err != nil {
			logger.Err(fmt.Sprintf("Failed to open GPIO value file for pin %s: %v", p, err))
			cleanup()
			os.Exit(1)
		}
		defer f.Close()
		buttons[p] = f
		buttonFds[p] = int(f.Fd())
	}
	logger.Info("Button files opened successfully")

	// Create epoll instance
	epfd, err := unix.EpollCreate1(0)
	if err != nil {
		logger.Err(fmt.Sprintf("epoll create error: %v", err))
		cleanup()
		os.Exit(1)
	}
	defer unix.Close(epfd)
	logger.Info("Epoll created successfully")

	// Add buttons to epoll
	var event unix.EpollEvent
	event.Events = unix.EPOLLIN | unix.EPOLLPRI | unix.EPOLLET

	// Register all button file descriptors with epoll
	for pin, fd := range buttonFds {
		event.Fd = int32(fd)
		if err = unix.EpollCtl(epfd, unix.EPOLL_CTL_ADD, fd, &event); err != nil {
			logger.Err(fmt.Sprintf("epoll add error for pin %s: %v", pin, err))
			cleanup()
			os.Exit(1)
		}
	}
	logger.Info("All buttons registered with epoll")

	// Create timer for LED blinking
	timerFd, err := unix.TimerfdCreate(unix.CLOCK_MONOTONIC, unix.TFD_NONBLOCK)
	if err != nil {
		logger.Err(fmt.Sprintf("timerfd create error: %v", err))
		cleanup()
		os.Exit(1)
	}
	defer unix.Close(timerFd)
	logger.Info("Timer created successfully")

	// Add timer to epoll
	event.Fd = int32(timerFd)
	if err = unix.EpollCtl(epfd, unix.EPOLL_CTL_ADD, timerFd, &event); err != nil {
		logger.Err(fmt.Sprintf("epoll add error for timer: %v", err))
		cleanup()
		os.Exit(1)
	}

	// Initial frequency and LED state
	freq := initialFreq
	ledState := true
	setLED(ledPin, ledState)
	logger.Info(fmt.Sprintf("Initial LED frequency: %.1f Hz", freq))

	// Function to set the timer period based on frequency
	setPeriod := func(frequency float64) {
		period := time.Duration(1e9 / frequency)

		var timeSpec unix.ItimerSpec
		timeSpec.Value.Sec = int64(period / time.Second)
		timeSpec.Value.Nsec = int64(period % time.Second)
		timeSpec.Interval.Sec = timeSpec.Value.Sec
		timeSpec.Interval.Nsec = timeSpec.Value.Nsec

		if err := unix.TimerfdSettime(timerFd, 0, &timeSpec, nil); err != nil {
			logger.Err(fmt.Sprintf("timerfd settime error: %v", err))
		}
	}

	// Set initial timer
	setPeriod(freq)
	logger.Info(fmt.Sprintf("Timer set to initial frequency: %.1f Hz", freq))

	// Button state tracking
	buttonState := make(map[string]bool)

	// Auto-repeat variables
	const (
		autoRepeatDelay    = 500 * time.Millisecond
		autoRepeatInterval = 200 * time.Millisecond
	)

	var k1Timer, k3Timer *time.Timer
	var k1Ticker, k3Ticker *time.Ticker

	// Function to clean up timers
	cleanupTimers := func() {
		if k1Timer != nil {
			k1Timer.Stop()
		}
		if k3Timer != nil {
			k3Timer.Stop()
		}
		if k1Ticker != nil {
			k1Ticker.Stop()
		}
		if k3Ticker != nil {
			k3Ticker.Stop()
		}
	}

	// Button handler functions
	increaseFreq := func() {
		if freq < maxFreq {
			freq += freqIncrement
			if freq > maxFreq {
				freq = maxFreq
			}
			logger.Info(fmt.Sprintf("Increased LED frequency to %.1f Hz", freq))
			setPeriod(freq)
		} else {
			logger.Info(fmt.Sprintf("LED frequency at maximum: %.1f Hz", freq))
		}
	}

	decreaseFreq := func() {
		if freq > minFreq {
			freq -= freqIncrement
			if freq < minFreq {
				freq = minFreq
			}
			logger.Info(fmt.Sprintf("Decreased LED frequency to %.1f Hz", freq))
			setPeriod(freq)
		} else {
			logger.Info(fmt.Sprintf("LED frequency at minimum: %.1f Hz", freq))
		}
	}

	resetFreq := func() {
		freq = initialFreq
		logger.Info(fmt.Sprintf("Reset LED frequency to %.1f Hz", freq))
		setPeriod(freq)
	}

	// Main event loop
	var events [10]unix.EpollEvent
	for {
		select {
		case sig := <-sigChan:
			// Handle termination signals
			logger.Info(fmt.Sprintf("Received signal %v, shutting down", sig))
			cleanupTimers()
			cleanup()
			return

		default:
			// Wait for events with a timeout
			nevents, err := unix.EpollWait(epfd, events[:], 100)
			if err != nil {
				if err == unix.EINTR {
					continue
				}
				logger.Err(fmt.Sprintf("epoll wait error: %v", err))
				break
			}

			for ev := 0; ev < nevents; ev++ {
				fd := int(events[ev].Fd)

				if fd == timerFd {
					// Handle timer event - toggle LED
					var timeoutCount uint64
					unix.Read(timerFd, (*(*[8]byte)(unsafe.Pointer(&timeoutCount)))[:])

					ledState = !ledState
					setLED(ledPin, ledState)

				} else {
					// Determine which button was pressed
					var buttonPin string
					for pin, buttonFd := range buttonFds {
						if buttonFd == fd {
							buttonPin = pin
							break
						}
					}

					if buttonPin != "" {
						// Read button state
						var buf [8]byte
						unix.Seek(fd, 0, 0)
						unix.Read(fd, buf[:])

						val, _ := strconv.Atoi(string(buf[0]))
						newState := val == 1
						oldState := buttonState[buttonPin]
						buttonState[buttonPin] = newState

						// Debug output
						logger.Info(fmt.Sprintf("Button %s state change: %v -> %v", buttonPin, oldState, newState))

						if newState && !oldState {
							// Button pressed
							switch buttonPin {
							case k1Pin:
								// Increase frequency
								logger.Info("K1 button pressed - increasing frequency")
								increaseFreq()

								// Start auto-repeat timer
								k1Timer = time.AfterFunc(autoRepeatDelay, func() {
									k1Ticker = time.NewTicker(autoRepeatInterval)
									go func() {
										for range k1Ticker.C {
											if buttonState[k1Pin] {
												increaseFreq()
											} else {
												k1Ticker.Stop()
												break
											}
										}
									}()
								})

							case k2Pin:
								// Reset frequency
								logger.Info("K2 button pressed - resetting frequency")
								resetFreq()

							case k3Pin:
								// Decrease frequency
								logger.Info("K3 button pressed - decreasing frequency")
								decreaseFreq()

								// Start auto-repeat timer
								k3Timer = time.AfterFunc(autoRepeatDelay, func() {
									k3Ticker = time.NewTicker(autoRepeatInterval)
									go func() {
										for range k3Ticker.C {
											if buttonState[k3Pin] {
												decreaseFreq()
											} else {
												k3Ticker.Stop()
												break
											}
										}
									}()
								})
							}

						} else if !newState && oldState {
							// Button released
							logger.Info(fmt.Sprintf("Button %s released", buttonPin))
							switch buttonPin {
							case k1Pin:
								if k1Timer != nil {
									k1Timer.Stop()
								}
								if k1Ticker != nil {
									k1Ticker.Stop()
								}

							case k3Pin:
								if k3Timer != nil {
									k3Timer.Stop()
								}
								if k3Ticker != nil {
									k3Ticker.Stop()
								}
							}
						}
					}
				}
			}
		}
	}
}
