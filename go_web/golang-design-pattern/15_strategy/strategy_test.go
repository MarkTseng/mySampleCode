package strategy

func ExamplePayByCash() {
	ctx := NewPaymentContext("Ada", "", 123, &Cash{})
	ctx.Pay()
	// Output:
	// Pay $123 to Ada by cash
}

func ExamplePayByBank() {
	ctx := NewPaymentContext("Bob", "0002", 888, &Bank{})
	ctx.Pay()
	// Output:
	// Pay $888 to Bob by bank account 0002
}
