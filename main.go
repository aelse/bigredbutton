package main

import (
	"flag"
	"log"
	"strings"
	"time"

	"github.com/tarm/serial"
)

func read(s *serial.Port, ch chan<- string) []byte {
	var buf strings.Builder
	for {
		b := make([]byte, 128)
		n, _ := s.Read(b)
		if n > 0 {
			buf.Write(b[:n])
		} else if buf.Len() > 0 {
			ch <- string(buf.String())
			buf.Reset()
		}
	}
	panic("not reached")
}

func main() {
	device := flag.String("dev", "/dev/tty.BigRedButton-DevB-1", "tty device to attach")
	once := flag.Bool("once", false, "Terminate after one button press")
	flag.Parse()

	var ch = make(chan string, 10)
	c := &serial.Config{
		Name:        *device,
		Baud:        38400,
		ReadTimeout: 10 * time.Millisecond,
	}
	s, err := serial.OpenPort(c)
	if err != nil {
		log.Fatal(err)
	}

	go read(s, ch)

	if *once {
		log.Println("Will terminate after first press")
	}
	for {
		msg := <-ch
		log.Printf("%q", strings.Trim(msg, "\r\n"))
		if *once && strings.HasPrefix(msg, "pressed") {
			log.Println("Terminating")
			break
		}
	}
}
