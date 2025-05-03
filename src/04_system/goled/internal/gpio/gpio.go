package gpio

import (
	"fmt"
	"os"
	"time"
)

const (
	// GPIO paths
	gpioExport   = "/sys/class/gpio/export"
	gpioUnexport = "/sys/class/gpio/unexport"
)

// Setup configures a GPIO pin
func Setup(pin, direction string, edge string) error {
	// Unexport first to ensure clean state
	_ = os.WriteFile(gpioUnexport, []byte(pin), 0644)

	// Export the pin
	if err := os.WriteFile(gpioExport, []byte(pin), 0644); err != nil {
		return fmt.Errorf("failed to export GPIO pin %s: %v", pin, err)
	}

	// Wait a moment for sysfs entries to be created
	time.Sleep(100 * time.Millisecond)

	// Set direction
	dirPath := fmt.Sprintf("/sys/class/gpio/gpio%s/direction", pin)
	if err := os.WriteFile(dirPath, []byte(direction), 0644); err != nil {
		return fmt.Errorf("failed to set direction for GPIO pin %s: %v", pin, err)
	}

	// Set edge detection if provided and pin is input
	if edge != "" && direction == "in" {
		edgePath := fmt.Sprintf("/sys/class/gpio/gpio%s/edge", pin)
		if err := os.WriteFile(edgePath, []byte(edge), 0644); err != nil {
			return fmt.Errorf("failed to set edge for GPIO pin %s: %v", pin, err)
		}
	}

	return nil
}

// OpenValueFile opens the value file for a GPIO pin
func OpenValueFile(pin string) (*os.File, error) {
	valuePath := fmt.Sprintf("/sys/class/gpio/gpio%s/value", pin)
	file, err := os.Open(valuePath)
	if err != nil {
		return nil, fmt.Errorf("failed to open GPIO value file for pin %s: %v", pin, err)
	}
	return file, nil
}

// Unexport removes a GPIO pin from sysfs
func Unexport(pin string) error {
	return os.WriteFile(gpioUnexport, []byte(pin), 0644)
}
