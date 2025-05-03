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
func (l *Logger) Info(format string, args ...interface{}) {
	message := fmt.Sprintf(format, args...)

	// Log only to syslog
	if l.syslog != nil {
		l.syslog.Info(message)
	}
}

// Error logs an error message
func (l *Logger) Error(format string, args ...interface{}) {
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
