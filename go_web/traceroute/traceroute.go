package main

import (
	"fmt"
	"github.com/lixiangzhong/traceroute"
)

func main() {
	t := traceroute.New("tw-traffic.servehttp.com")
	//t.MaxTTL=30
	//t.Timeout=3 * time.Second
	//t.LocalAddr="0.0.0.0"
	result, err := t.Do()
	if err != nil {
		fmt.Println(err)
		return
	}
	for _, v := range result {
		fmt.Println(v)
	}
}
