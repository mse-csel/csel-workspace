package gpio

import (
	"os"
	"strconv"

	"golang.org/x/sys/unix"
)

// Button represents a GPIO-controlled button
type Button struct {
	pin     string
	file    *os.File
	fd      int
	pressed bool
}

// NewButton creates a new button controller
func NewButton(pin string) (*Button, error) {
	// Setup GPIO
	if err := Setup(pin, "in", "both"); err != nil {
		return nil, err
	}

	// Open value file
	file, err := OpenValueFile(pin)
	if err != nil {
		return nil, err
	}

	return &Button{
		pin:     pin,
		file:    file,
		fd:      int(file.Fd()),
		pressed: false,
	}, nil
}

// GetFD returns the file descriptor
func (b *Button) GetFD() int {
	return b.fd
}

// ReadState reads the current button state
// It returns true if the button is pressed, false otherwise
// and an error if the read operation fails
// Note: It reads the state directly from the file by looking
// at the first byte which is expected to be '0' or '1'.
// If '1' is read, it means the button is pressed.
func (b *Button) ReadState() (bool, error) {
	var buf [8]byte

	if _, err := unix.Seek(b.fd, 0, 0); err != nil {
		return false, err
	}

	if _, err := unix.Read(b.fd, buf[:]); err != nil {
		return false, err
	}

	val, err := strconv.Atoi(string(buf[0]))
	if err != nil {
		return false, err
	}

	return val == 1, nil
}

// IsPressed returns whether the button is currently pressed
func (b *Button) IsPressed() bool {
	return b.pressed
}

// SetPressed sets the button's pressed state
func (b *Button) SetPressed(state bool) {
	b.pressed = state
}

// Close cleans up resources
func (b *Button) Close() error {
	if b.file != nil {
		b.file.Close()
	}
	return Unexport(b.pin)
}
