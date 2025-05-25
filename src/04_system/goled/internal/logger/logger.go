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

package logger

import (
	"fmt"
	"log/syslog"
)

// Logger provides logging functionality
// Check at /var/log/syslog or /var/log/messages
type Logger struct {
	syslog *syslog.Writer
	tag    string
}

// New creates a new logger
func New(tag string) (*Logger, error) {
	// Try to connect to syslog
	sl, err := syslog.New(syslog.LOG_INFO|syslog.LOG_DAEMON, tag)
	if err != nil {
		// If syslog is not available, return error
		return nil, fmt.Errorf("could not connect to syslog: %v", err)
	}

	return &Logger{
		syslog: sl,
		tag:    tag,
	}, nil
}

// Info logs an informational message
func (l *Logger) Info(format string, args ...any) {
	message := fmt.Sprintf(format, args...)

	// Log only to syslog
	if l.syslog != nil {
		l.syslog.Info(message)
	}
}

// Error logs an error message
func (l *Logger) Error(format string, args ...any) {
	message := fmt.Sprintf(format, args...)

	// Log only to syslog
	if l.syslog != nil {
		l.syslog.Err(message)
	}
}

// Close closes the logger
func (l *Logger) Close() {
	if l.syslog != nil {
		l.syslog.Close()
	}
}
