// Copyright 2025 University of Applied Sciences Western Switzerland / Fribourg
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Project: HEIA-FR / HES-SO MSE - MA-CSEL1 Laboratory
//
// Abstract: System programming - file system
//
// Purpose: NanoPi advanced status led control system
//
// Author:  Bastien Veuthey
// Date:    03.05.2025

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
