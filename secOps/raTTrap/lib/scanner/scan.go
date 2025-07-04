package scanner

import (
	"fmt"
	"net"
	"os"
	"os/signal"
	"rattrap/lib/printer"
	"runtime"
	"strconv"
	"strings"
	"time"

	"github.com/shirou/gopsutil/process"
	pnet "github.com/shirou/gopsutil/v3/net"
)

const refreshInterval = 2 * time.Second

func DisplayConnections() {
	sigChan := make(chan os.Signal, 1)
	signal.Notify(sigChan, os.Interrupt)

	ticker := time.NewTicker(refreshInterval)
	defer ticker.Stop()

	getConnections()

	for {
		select {
		case <-ticker.C:
			getConnections()
		case <-sigChan:
			fmt.Printf("%s\n", printer.PrintGreen()("Exiting raTTrap... Goodbye!"))
			return
		}
	}
}

func getConnections() {
	clearScreen()

	cyan := printer.PrintCyan()

	numColumnWidth := 4
	ipColumnWidth := 44
	statusWidth := 15

	fmt.Printf("%s\n", printer.PrintGreen()("Scanning for all public network connections"))
	fmt.Println("------------------------------------------------")

	connections, err := pnet.Connections("all")
	if err != nil {
		fmt.Printf("%s\n", printer.PrintRed()("[!] Unable to fetch connections"))
	}

	for i, conn := range connections {
		if len(conn.Raddr.IP) == 0 {
			continue
		}

		remoteIP := net.ParseIP(conn.Raddr.IP)
		if remoteIP == nil || remoteIP.IsLoopback() || remoteIP.IsPrivate() {
			continue
		}

		procName := getProcessName(conn.Pid)

		process := fmt.Sprintf("Process: %s (PID: %d)", procName, conn.Pid)
		status := conn.Status
		conn_details := fmt.Sprintf("%s:%d -> %s:%d", conn.Laddr.IP, conn.Laddr.Port, conn.Raddr.IP, conn.Raddr.Port)

		numPadding := strings.Repeat(" ", numColumnWidth-len(strconv.Itoa(i)))
		ipPadding := strings.Repeat(" ", ipColumnWidth-len(conn_details))
		statusPadding := strings.Repeat(" ", statusWidth-len(status))

		fmt.Printf("#%d:%s %s%s\t%s%s\t%s\n", i, numPadding,
			cyan(conn_details), ipPadding,
			cyan(status), statusPadding,
			cyan(process))

	}

}

func clearScreen() {
	if runtime.GOOS == "windows" {
		// A simple way for modern Windows terminals.
		fmt.Print("\033[H\033[2J")
	} else {
		// Standard for Unix-like systems.
		fmt.Print("\033[2J\033[H")
	}
}

func getProcessName(pid int32) string {
	if pid == 0 {
		return "System"
	}
	p, err := process.NewProcess(pid)
	if err != nil {
		return fmt.Sprintf("PID %d (error getting name: %v)", pid, err)
	}
	name, err := p.Name()
	if err != nil {
		return fmt.Sprintf("PID %d (error getting name: %v)", pid, err)
	}
	return name
}
