package main

import (
	"github.com/gin-gonic/contrib/sessions"
	"github.com/gin-gonic/gin"
	"github.com/utrack/gin-csrf"
)

func main() {
	r := gin.Default()
	store := sessions.NewCookieStore([]byte("secret"))
	r.Use(sessions.Sessions("mysession", store))
	r.Use(csrf.Middleware(csrf.Options{
		Secret: "secret123",
		ErrorFunc: func(c *gin.Context) {
			c.String(400, "CSRF token mismatch")
			c.Abort()
		},
	}))

	r.GET("/protected", func(c *gin.Context) {
		c.String(200, csrf.GetToken(c))
	})

	r.POST("/protected", func(c *gin.Context) {
		c.String(200, "CSRF token is valid")
	})

	r.Run(":8000")
}
