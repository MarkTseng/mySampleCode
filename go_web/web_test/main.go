package main

import (
	"flag"
	"fmt"
	"net/http"
	"os"
	"path"

	"github.com/gin-gonic/gin"
	"github.com/zalando/gin-oauth2/google"
)

var redirectURL, credFile string

func init() {
	bin := path.Base(os.Args[0])
	flag.Usage = func() {
		fmt.Fprintf(os.Stderr, `
		Usage of %s
		================
		`, bin)
		flag.PrintDefaults()
	}
	flag.StringVar(&redirectURL, "redirect", "http://192.168.10.20:3000/auth/", "URL to be redirected to after authorization.")
	flag.StringVar(&credFile, "cred-file", "./example/google/test-clientid.google.json", "Credential JSON file")
}
func main() {
	flag.Parse()

	scopes := []string{
		"https://www.googleapis.com/auth/userinfo.email",
		// You have to select your own scope from here -> https://developers.google.com/identity/protocols/googlescopes#google_sign-in
	}
	secret := []byte("secret")
	sessionName := "goquestsession"

	router := gin.Default()
	// init settings for google auth
	google.Setup(redirectURL, credFile, scopes, secret)
	router.Use(google.Session(sessionName))

	router.GET("/login", google.LoginHandler)

	// protected url group
	private := router.Group("/auth")
	private.Use(google.Auth())
	private.GET("/", UserInfoHandler)
	private.GET("/api", func(ctx *gin.Context) {
		ctx.JSON(200, gin.H{"message": "Hello from private for groups"})
	})

	router.Run(":3000")
}

func UserInfoHandler(ctx *gin.Context) {
	ctx.JSON(http.StatusOK, gin.H{"Hello": "from private", "user": ctx.MustGet("user").(google.User)})
}
