RAT-Trap 🐀
A simple yet powerful command-line tool written in Go to monitor all active network connections on your PC. It helps you expose potentially unauthorized remote connections by showing the status and process owner for every connection.

✨ Features
Comprehensive Scanning: Lists all active TCP and UDP connections on the system.

Detailed Connection Info: Shows local/remote addresses, ports, and the current connection status (e.g., LISTEN, ESTABLISHED, TIME_WAIT).

Process Identification: Shows the name and Process ID (PID) for each connection, helping you identify which application is responsible.

Cross-Platform: Works on Windows, macOS, and Linux.

📋 Prerequisites
Before you begin, ensure you have Go installed on your system. You can download it from the official website: golang.org.

To verify your installation, open a terminal and run:

go version

🚀 Usage
Clone or download this project's code.

Navigate to the project directory in your terminal:

cd /path/to/rat-trap

Run the application:
Go will automatically handle downloading the necessary dependencies (gopsutil) and run the program.

go run .

Important: Run as Administrator/root
For the most complete and accurate results, it's highly recommended to run this tool with elevated privileges. This allows it to inspect connections from all users and system processes.

On Linux or macOS:

sudo go run .

On Windows:
Open PowerShell or Command Prompt as an Administrator and then run the command.

📝 Example Output
The tool will print a detailed, single-line summary for each network connection it finds:

Scanning for all public network connections
------------------------------------------------
#0: 0.0.0.0:135 -> 0.0.0.0:0       LISTEN        Process: svchost.exe (PID: 1448)
#1: 0.0.0.0:445 -> 0.0.0.0:0       LISTEN        Process: System (PID: 4)
...
#10: 192.168.1.6:51023 -> 172.188.215.153:443   ESTABLISHED   Process: msedge.exe (PID: 3944)
#11: 192.168.1.6:51030 -> 208.103.161.2:443     ESTABLISHED   Process: Notion.exe (PID: 10812)
#15: 192.168.1.6:51642 -> 20.189.173.27:443     ESTABLISHED   Process: Code.exe (PID: 2084)
#16: 192.168.1.6:51708 -> 34.110.164.207:443     TIME_WAIT     Process: System (PID: 0)
#21: 192.168.1.6:51720 -> 13.107.246.58:443     CLOSE_WAIT    Process: Code.exe (PID: 2084)
...
#23: :::135 -> :::0              LISTEN        Process: svchost.exe (PID: 1448)
Scan complete

📜 License
This project is licensed under the MIT License.