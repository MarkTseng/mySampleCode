package main

import (
	"fmt"
	"github.com/night-codes/mgo-ai"
	"gopkg.in/mgo.v2"
	"gopkg.in/mgo.v2/bson"
	"log"
)

const (
	MongoDBUrl = "mongodb://localhost:27017/articles_demo_dev"
)

// Users model
type Users struct {
	Id       bson.ObjectId `json:"_id,omitempty" bson:"_id,omitempty"`
	Username string        `json:"username" form:"username" binding:"required" bson:"username"`
	Password string        `json:"password" form:"password" binding:"required" bson:"password"`
}

func main() {
	session, err := mgo.Dial(MongoDBUrl)
	if err != nil {
		panic(err)
	}

	defer session.Close()
	// Optional. Switch the session to a monotonic behavior.
	session.SetMode(mgo.Monotonic, true)
	// connect AutoIncrement to collection "counters"
	ai.Connect(session.DB("articles_demo_dev").C("counters"))

	c := session.DB("articles_demo_dev").C("users")

	// use AutoIncrement
	/*
		session.DB("example-db").C("users").Insert(bson.M{
			"_id":   ai.Next("users"),
			"login": "test",
			"age":   32,
		})
	*/

	err = c.Insert(bson.M{"username": "user9", "password": "pass9", "_id": ai.Next("users")})
	//	&Person{"Cla", "+55 53 8402 8510"})
	//if err != nil {
	//	log.Fatal(err)
	//}

	// Index
	index := mgo.Index{
		Key:        []string{"username", "password"},
		Unique:     true,
		DropDups:   true,
		Background: true,
		Sparse:     true,
	}

	err = c.EnsureIndex(index)
	if err != nil {
		panic(err)
	}

	result := Users{}
	err = c.Find(bson.M{"_id": bson.ObjectIdHex("5b7d08bdcb066113e12d4234")}).One(&result)
	//err = c.Find(nil).Sort("-updated_on").All(&result)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Println("username:", result.Username)
	fmt.Println("username:", result)
	fmt.Println("collections", c)
}
