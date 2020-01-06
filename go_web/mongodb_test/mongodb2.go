package main

import (
	"fmt"
	"gopkg.in/mgo.v2"
	"gopkg.in/mgo.v2/bson"
)

type Person struct {
	ID       bson.ObjectId `bson:"_id,omitempty"`
	Username string
	Password string
}

var (
	IsDrop = true
)

func main() {
	session, err := mgo.Dial("127.0.0.1")
	if err != nil {
		panic(err)
	}

	defer session.Close()

	session.SetMode(mgo.Monotonic, true)

	// Collection People
	c := session.DB("articles_demo_dev").C("users")

	// Insert Datas
	// err = c.Insert(&Person{Username: "user4", Password: "pass4"})

	// Query One
	result := Person{}
	err = c.Find(bson.M{"username": "user1"}).One(&result)
	if err != nil {
		panic(err)
	}
	fmt.Println("username", result)

	// Query All
	var results []Person
	err = c.Find(nil).Sort("-timestamp").All(&results)

	if err != nil {
		panic(err)
	}
	fmt.Println("Results All: ", results)

}
