package main

import (
	"fmt"
	"os"
	"os/signal"
	"time"

	"csel1/goled/internal/gpio"
	"csel1/goled/internal/logger"
	"csel1/goled/internal/timer"
)

const (
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

	// Auto-repeat settings
	autoRepeatDelay    = 500 * time.Millisecond
	autoRepeatInterval = 200 * time.Millisecond
)

func main() {
	// Initialize logger
	log, err := logger.New("goled")
	if err != nil {
		fmt.Fprintf(os.Stderr, "Failed to initialize logger: %v\n", err)
		os.Exit(1)
	}
	defer log.Close()

	log.Info("Starting goled application")
	log.Info("Using LED pin: %s", ledPin)
	log.Info("Using button pins: K1=%s, K2=%s, K3=%s", k1Pin, k2Pin, k3Pin)

	// Setup signal handling for graceful shutdown
	sigChan := make(chan os.Signal, 1)
	signal.Notify(sigChan, os.Interrupt)

	// Initialize GPIO devices
	led, err := gpio.NewLED(ledPin)
	if err != nil {
		log.Error("Failed to initialize LED: %v", err)
		os.Exit(1)
	}
	defer led.Close()

	// Initialize buttons
	k1, err := gpio.NewButton(k1Pin)
	if err != nil {
		log.Error("Failed to initialize K1 button: %v", err)
		os.Exit(1)
	}
	defer k1.Close()

	k2, err := gpio.NewButton(k2Pin)
	if err != nil {
		log.Error("Failed to initialize K2 button: %v", err)
		os.Exit(1)
	}
	defer k2.Close()

	k3, err := gpio.NewButton(k3Pin)
	if err != nil {
		log.Error("Failed to initialize K3 button: %v", err)
		os.Exit(1)
	}
	defer k3.Close()

	// Initialize timer for LED blinking
	ledTimer, err := timer.New()
	if err != nil {
		log.Error("Failed to create timer: %v", err)
		os.Exit(1)
	}
	defer ledTimer.Close()

	// Create a controller to manage the devices
	controller := NewController(log, led, ledTimer, k1, k2, k3, initialFreq, freqIncrement, minFreq, maxFreq)
	defer controller.Cleanup()

	// Set initial frequency
	log.Info("Setting initial frequency to %.1f Hz", initialFreq)
	controller.SetFrequency(initialFreq)

	// Run the main event loop
	controller.Run(sigChan, autoRepeatDelay, autoRepeatInterval)
}

// Controller manages the LED and buttons
type Controller struct {
	log           *logger.Logger
	led           *gpio.LED
	timer         *timer.Timer
	k1            *gpio.Button
	k2            *gpio.Button
	k3            *gpio.Button
	currentFreq   float64
	freqIncrement float64
	minFreq       float64
	maxFreq       float64
	poller        *gpio.Poller
	k1Timer       *time.Timer
	k3Timer       *time.Timer
	k1Ticker      *time.Ticker
	k3Ticker      *time.Ticker
}

// NewController creates a new controller
func NewController(log *logger.Logger, led *gpio.LED, timer *timer.Timer, k1, k2, k3 *gpio.Button,
	initialFreq, freqIncrement, minFreq, maxFreq float64) *Controller {

	poller, err := gpio.NewPoller()
	if err != nil {
		log.Error("Failed to create poller: %v", err)
		os.Exit(1)
	}

	// Register the timer with the poller
	if err := poller.AddFD(timer.GetFD()); err != nil {
		log.Error("Failed to add timer to poller: %v", err)
		os.Exit(1)
	}

	// Register buttons with the poller
	if err := poller.AddFD(k1.GetFD()); err != nil {
		log.Error("Failed to add K1 button to poller: %v", err)
		os.Exit(1)
	}

	if err := poller.AddFD(k2.GetFD()); err != nil {
		log.Error("Failed to add K2 button to poller: %v", err)
		os.Exit(1)
	}

	if err := poller.AddFD(k3.GetFD()); err != nil {
		log.Error("Failed to add K3 button to poller: %v", err)
		os.Exit(1)
	}

	return &Controller{
		log:           log,
		led:           led,
		timer:         timer,
		k1:            k1,
		k2:            k2,
		k3:            k3,
		currentFreq:   initialFreq,
		freqIncrement: freqIncrement,
		minFreq:       minFreq,
		maxFreq:       maxFreq,
		poller:        poller,
	}
}

// SetFrequency sets the LED blinking frequency
func (c *Controller) SetFrequency(freq float64) {
	c.currentFreq = freq
	c.timer.SetPeriod(time.Duration(1e9 / freq))
}

// Cleanup releases all resources
func (c *Controller) Cleanup() {
	// Stop any timers
	if c.k1Timer != nil {
		c.k1Timer.Stop()
	}
	if c.k3Timer != nil {
		c.k3Timer.Stop()
	}
	if c.k1Ticker != nil {
		c.k1Ticker.Stop()
	}
	if c.k3Ticker != nil {
		c.k3Ticker.Stop()
	}

	// Close the poller
	c.poller.Close()
}

// Run runs the main event loop
func (c *Controller) Run(sigChan chan os.Signal, autoRepeatDelay, autoRepeatInterval time.Duration) {
	// LED state
	ledState := true
	c.led.SetState(ledState)

	// Button handler functions
	increaseFreq := func() {
		if c.currentFreq < c.maxFreq {
			newFreq := c.currentFreq + c.freqIncrement
			if newFreq > c.maxFreq {
				newFreq = c.maxFreq
			}
			c.log.Info("Increased LED frequency to %.1f Hz", newFreq)
			c.SetFrequency(newFreq)
		} else {
			c.log.Info("LED frequency at maximum: %.1f Hz", c.currentFreq)
		}
	}

	decreaseFreq := func() {
		if c.currentFreq > c.minFreq {
			newFreq := c.currentFreq - c.freqIncrement
			if newFreq < c.minFreq {
				newFreq = c.minFreq
			}
			c.log.Info("Decreased LED frequency to %.1f Hz", newFreq)
			c.SetFrequency(newFreq)
		} else {
			c.log.Info("LED frequency at minimum: %.1f Hz", c.currentFreq)
		}
	}

	resetFreq := func() {
		c.log.Info("Reset LED frequency to %.1f Hz", initialFreq)
		c.SetFrequency(initialFreq)
	}

	// Main event loop
	for {
		select {
		case sig := <-sigChan:
			// Handle termination signals
			c.log.Info("Received signal %v, shutting down", sig)
			return

		default:
			// Wait for events
			events, err := c.poller.Wait(100) // 100ms timeout
			if err != nil {
				c.log.Error("Poll error: %v", err)
				continue
			}

			for _, event := range events {
				fd := event.FD

				if fd == c.timer.GetFD() {
					// Handle timer event - toggle LED
					c.timer.Read()
					ledState = !ledState
					c.led.SetState(ledState)

				} else if fd == c.k1.GetFD() {
					// K1 button - increase frequency
					newState, err := c.k1.ReadState()
					if err != nil {
						c.log.Error("Failed to read K1 state: %v", err)
						continue
					}

					oldState := c.k1.IsPressed()
					c.k1.SetPressed(newState)

					c.log.Info("K1 button state change: %v -> %v", oldState, newState)

					if newState && !oldState {
						// Button pressed
						c.log.Info("K1 button pressed - increasing frequency")
						increaseFreq()

						// Start auto-repeat timer
						c.k1Timer = time.AfterFunc(autoRepeatDelay, func() {
							c.k1Ticker = time.NewTicker(autoRepeatInterval)
							go func() {
								for range c.k1Ticker.C {
									if c.k1.IsPressed() {
										increaseFreq()
									} else {
										c.k1Ticker.Stop()
										break
									}
								}
							}()
						})

					} else if !newState && oldState {
						// Button released
						c.log.Info("K1 button released")
						if c.k1Timer != nil {
							c.k1Timer.Stop()
						}
						if c.k1Ticker != nil {
							c.k1Ticker.Stop()
						}
					}

				} else if fd == c.k2.GetFD() {
					// K2 button - reset frequency
					newState, err := c.k2.ReadState()
					if err != nil {
						c.log.Error("Failed to read K2 state: %v", err)
						continue
					}

					oldState := c.k2.IsPressed()
					c.k2.SetPressed(newState)

					c.log.Info("K2 button state change: %v -> %v", oldState, newState)

					if newState && !oldState {
						c.log.Info("K2 button pressed - resetting frequency")
						resetFreq()
					}

				} else if fd == c.k3.GetFD() {
					// K3 button - decrease frequency
					newState, err := c.k3.ReadState()
					if err != nil {
						c.log.Error("Failed to read K3 state: %v", err)
						continue
					}

					oldState := c.k3.IsPressed()
					c.k3.SetPressed(newState)

					c.log.Info("K3 button state change: %v -> %v", oldState, newState)

					if newState && !oldState {
						// Button pressed
						c.log.Info("K3 button pressed - decreasing frequency")
						decreaseFreq()

						// Start auto-repeat timer
						c.k3Timer = time.AfterFunc(autoRepeatDelay, func() {
							c.k3Ticker = time.NewTicker(autoRepeatInterval)
							go func() {
								for range c.k3Ticker.C {
									if c.k3.IsPressed() {
										decreaseFreq()
									} else {
										c.k3Ticker.Stop()
										break
									}
								}
							}()
						})

					} else if !newState && oldState {
						// Button released
						c.log.Info("K3 button released")
						if c.k3Timer != nil {
							c.k3Timer.Stop()
						}
						if c.k3Ticker != nil {
							c.k3Ticker.Stop()
						}
					}
				}
			}
		}
	}
}
