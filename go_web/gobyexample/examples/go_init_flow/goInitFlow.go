package main

import (
	"fmt"
)

var number = GetNumber()

func GetNumber() int {
	//fmt.Println("Number:", number) //Why this error?
	fmt.Println("GetNumber() is called")
	return 999
}

func init() {
	number = 0
	fmt.Println("init")
}

func deinit() {
	number = 99
	fmt.Println("deinit")
}

func main() {
	defer deinit()
	if number == 0 {
		fmt.Println("yo~ this is main()")
	}
}
