package scanner

import (
	"fmt"
	"net"
	"rattrap/lib/printer"
	"strconv"
	"strings"

	"github.com/shirou/gopsutil/process"
	pnet "github.com/shirou/gopsutil/v3/net"
)

func ScanConnections() {
	green := printer.PrintGreen()
	red := printer.PrintRed()
	cyan := printer.PrintCyan()

	numColumnWidth := 4
	ipColumnWidth := 44
	statusWidth := 15

	fmt.Printf("%s\n", green("Scanning for all public network connections"))
	fmt.Println("------------------------------------------------")

	connections, err := pnet.Connections("all")
	if err != nil {
		fmt.Printf("%s\n", red("[!] Unable to fetch connections"))
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

	fmt.Printf("%s\n", green("Scan complete"))
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
