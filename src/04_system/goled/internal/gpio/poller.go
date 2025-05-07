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

	"golang.org/x/sys/unix"
)

// PollEvent represents an event from the poller
type PollEvent struct {
	FD    int
	Event uint32
}

// Poller wraps the epoll functionality
type Poller struct {
	epfd int
}

// NewPoller creates a new epoll instance
func NewPoller() (*Poller, error) {
	epfd, err := unix.EpollCreate1(0)
	if err != nil {
		return nil, fmt.Errorf("epoll create error: %v", err)
	}

	return &Poller{
		epfd: epfd,
	}, nil
}

// AddFD adds a file descriptor to be monitored
func (p *Poller) AddFD(fd int) error {
	var event unix.EpollEvent
	event.Events = unix.EPOLLIN | unix.EPOLLPRI | unix.EPOLLET
	event.Fd = int32(fd)

	if err := unix.EpollCtl(p.epfd, unix.EPOLL_CTL_ADD, fd, &event); err != nil {
		return fmt.Errorf("epoll add error for fd %d: %v", fd, err)
	}

	return nil
}

// RemoveFD removes a file descriptor from monitoring
func (p *Poller) RemoveFD(fd int) error {
	if err := unix.EpollCtl(p.epfd, unix.EPOLL_CTL_DEL, fd, nil); err != nil {
		return fmt.Errorf("epoll remove error for fd %d: %v", fd, err)
	}

	return nil
}

// Wait waits for events with a timeout in milliseconds
func (p *Poller) Wait(timeoutMs int) ([]PollEvent, error) {
	var events [10]unix.EpollEvent

	n, err := unix.EpollWait(p.epfd, events[:], timeoutMs)
	if err != nil {
		if err == unix.EINTR {
			return nil, nil
		}
		return nil, fmt.Errorf("epoll wait error: %v", err)
	}

	result := make([]PollEvent, n)
	for i := 0; i < n; i++ {
		result[i] = PollEvent{
			FD:    int(events[i].Fd),
			Event: events[i].Events,
		}
	}

	return result, nil
}

// Close closes the epoll instance
func (p *Poller) Close() error {
	return unix.Close(p.epfd)
}
