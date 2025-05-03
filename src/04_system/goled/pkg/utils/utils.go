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

package utils

import (
	"fmt"
	"os"
	"strconv"
	"strings"
)

// FileExists checks if a file exists
func FileExists(path string) bool {
	_, err := os.Stat(path)
	return !os.IsNotExist(err)
}

// IsValidGPIO checks if a GPIO pin is valid and available
func IsValidGPIO(pin string) bool {
	// Check if the pin can be parsed as an integer
	_, err := strconv.Atoi(pin)
	if err != nil {
		return false
	}

	// Check if it could potentially be exported
	return FileExists("/sys/class/gpio/export")
}

// IsGPIOExported checks if a GPIO pin is already exported
func IsGPIOExported(pin string) bool {
	return FileExists(fmt.Sprintf("/sys/class/gpio/gpio%s", pin))
}

// GetPinValue reads the current value of a GPIO pin
func GetPinValue(pin string) (bool, error) {
	if !IsGPIOExported(pin) {
		return false, fmt.Errorf("pin %s is not exported", pin)
	}

	data, err := os.ReadFile(fmt.Sprintf("/sys/class/gpio/gpio%s/value", pin))
	if err != nil {
		return false, err
	}

	value := strings.TrimSpace(string(data))
	return value == "1", nil
}

// CalculateBlinkPeriod calculates the period in nanoseconds for a given frequency
func CalculateBlinkPeriod(freqHz float64) int64 {
	if freqHz <= 0 {
		return 0
	}
	return int64(1e9 / freqHz)
}

// ClampFrequency ensures a frequency is within the specified range
func ClampFrequency(freq, min, max float64) float64 {
	if freq < min {
		return min
	}
	if freq > max {
		return max
	}
	return freq
}

// FormatFrequency formats a frequency value nicely
func FormatFrequency(freq float64) string {
	return fmt.Sprintf("%.1f Hz", freq)
}
