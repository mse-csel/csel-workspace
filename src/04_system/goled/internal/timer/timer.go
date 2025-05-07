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

package timer

import (
	"fmt"
	"time"
	"unsafe"

	"golang.org/x/sys/unix"
)

// Timer wraps the timerfd functionality
type Timer struct {
	fd int
}

// New creates a new timer
func New() (*Timer, error) {
	fd, err := unix.TimerfdCreate(unix.CLOCK_MONOTONIC, unix.TFD_NONBLOCK)
	if err != nil {
		return nil, fmt.Errorf("timerfd create error: %v", err)
	}

	return &Timer{
		fd: fd,
	}, nil
}

// GetFD returns the timer's file descriptor
func (t *Timer) GetFD() int {
	return t.fd
}

// SetPeriod sets the timer period
func (t *Timer) SetPeriod(period time.Duration) error {
	var timeSpec unix.ItimerSpec
	timeSpec.Value.Sec = int64(period / time.Second)
	timeSpec.Value.Nsec = int64(period % time.Second)
	timeSpec.Interval.Sec = timeSpec.Value.Sec
	timeSpec.Interval.Nsec = timeSpec.Value.Nsec

	if err := unix.TimerfdSettime(t.fd, 0, &timeSpec, nil); err != nil {
		return fmt.Errorf("timerfd settime error: %v", err)
	}

	return nil
}

// Read reads the timer expiration count
func (t *Timer) Read() (uint64, error) {
	var timeoutCount uint64
	_, err := unix.Read(t.fd, (*(*[8]byte)(unsafe.Pointer(&timeoutCount)))[:])
	if err != nil {
		return 0, fmt.Errorf("timerfd read error: %v", err)
	}

	return timeoutCount, nil
}

// Close closes the timer
func (t *Timer) Close() error {
	return unix.Close(t.fd)
}
