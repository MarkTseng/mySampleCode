package main

import "fmt"
import "reflect"

type A struct {
	Foo string
}

func (a *A) PrintFoo() {
	fmt.Println("Foo value is " + a.Foo)
}

func main() {
	a := &A{Foo: "afoo"}
	val := reflect.Indirect(reflect.ValueOf(a))
	fmt.Println(val.Field(0).Type().Name())
}
