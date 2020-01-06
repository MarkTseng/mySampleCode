package main

import "fmt"

type Circle struct {
	x, y, r float64
}

func main() {
	c := Circle{1, 2, 3}
	c_point := &Circle{1, 2, 3}
	c_new := new(Circle)

	fmt.Printf("Circle: c %T(%p)\n", c, &c)
	c.x = 0
	fmt.Printf("Circle: c.x %f \n", c)

	fmt.Printf("Circle: c_point.x %f \n", c_point.x)
	fmt.Printf("Circle: c_point %T(%p)\n", c_point, c_point)
	fmt.Printf("Circle: c_point %f \n", c_point)

	fmt.Printf("Circle: c_new %T(%p)\n", c_new, c_new)
	fmt.Printf("Circle: c_new %f \n", c_new)
	c_new.x = 1
	fmt.Printf("Circle: c_new %f \n", c_new)
}
