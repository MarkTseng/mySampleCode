EXEC = gpio_int gpio_str

GPIO_PIN_STR = \"GPIO_116\"

all:
	gcc -DGPIO_PIN=116.22 gpio.c -o gpio_int
	gcc -DGPIO_PIN_STR=${GPIO_PIN_STR} gpio.c -o gpio_str

clean:
	rm -rf $(EXEC) *.o

test:
	./gpio_int
	./gpio_str
