package gpio

import (
	"fmt"
	"os"
)

// LED represents a GPIO-controlled LED
type LED struct {
	pin string
}

// NewLED creates a new LED controller
func NewLED(pin string) (*LED, error) {
	// Setup GPIO
	if err := Setup(pin, "out", ""); err != nil {
		return nil, err
	}

	return &LED{
		pin: pin,
	}, nil
}

// SetState sets the LED state (on/off)
func (l *LED) SetState(state bool) error {
	value := "0"
	if state {
		value = "1"
	}

	valuePath := fmt.Sprintf("/sys/class/gpio/gpio%s/value", l.pin)
	return os.WriteFile(valuePath, []byte(value), 0644)
}

// Close cleans up resources
func (l *LED) Close() error {
	return Unexport(l.pin)
}
