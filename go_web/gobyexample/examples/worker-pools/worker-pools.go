// In this example we'll look at how to implement
// a _worker pool_ using goroutines and channels.

package main

import "fmt"
import "time"

// Here's the worker, of which we'll run several
// concurrent instances. These workers will receive
// work on the `jobs` channel and send the corresponding
// results on `results`. We'll sleep a second per job to
// simulate an expensive task.
func worker(id int, jobs <-chan int, results chan<- int) {
	fmt.Println("worker", id, "start")
	for j := range jobs {
		fmt.Println("worker", id, "started  job", j, "time", time.Now())
		time.Sleep(time.Millisecond * 100)
		fmt.Println("worker", id, "finished job", j, "time", time.Now())
		results <- j * 2
	}
}

func main() {

	// In order to use our pool of workers we need to send
	// them work and collect their results. We make 2
	// channels for this.
	jobs := make(chan int, 100)
	results := make(chan int, 100)

	// This starts up 3 workers, initially blocked
	// because there are no jobs yet.
	for w := 1; w <= 3; w++ {
		go worker(w, jobs, results)
	}

	// Here we send 5 `jobs` and then `close` that
	// channel to indicate that's all the work we have.
	for j := 1; j <= 10; j++ {
		jobs <- j
	}
	close(jobs)

	// Finally we collect all the results of the work.
	for a := 1; a <= 10; a++ {
		<-results
	}
}
