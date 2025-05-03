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
