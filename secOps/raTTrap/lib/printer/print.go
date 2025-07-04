package printer

import "github.com/fatih/color"

func PrintRed() func(a ...interface{}) string {
	colour := color.New(color.FgRed).SprintFunc()
	return colour
}

func PrintGreen() func(a ...interface{}) string {
	colour := color.New(color.FgGreen).SprintFunc()
	return colour
}

func PrintCyan() func(a ...interface{}) string {
	colour := color.New(color.FgCyan).SprintFunc()
	return colour
}
