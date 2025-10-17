package main

import (
	"database/sql"
	"log"
	"net/http"

	"github.com/gin-contrib/cors"
	"github.com/gin-gonic/gin"
	"github.com/gocql/gocql"
	"github.com/gorilla/websocket"
	_ "github.com/lib/pq" // for initiating postgres driver
	"github.com/rabbitmq/amqp091-go"
)

// backend connection pools
var db *sql.DB
var scyllaCluster *gocql.ClusterConfig
var scyllaSession *gocql.Session
var amqpConn *amqp091.Connection
var amqpCh *amqp091.Channel

var jwtSecret = []byte("supermario123") // Move to config/env in production

// The upgrader is used to upgrade an HTTP connection to a WebSocket connection.
var upgrader = websocket.Upgrader{
	ReadBufferSize:  1024,
	WriteBufferSize: 1024,
	// Note: You'll want a proper CheckOrigin function in production
	CheckOrigin: func(r *http.Request) bool { return true },
}

func main() {
	var err error

	// rabbitmq conn
	log.Println("Connecting to RabbitMQ...")
	amqpConn, err = amqp091.Dial("amqp://chatadmin:chatpass@localhost:5672/")
	if err != nil {
		log.Fatalf("Failed to connect to RabbitMQ: %v", err)
	}
	defer amqpConn.Close()

	amqpCh, err = amqpConn.Channel()
	if err != nil {
		log.Fatalf("Failed to open a channel: %v", err)
	}
	defer amqpCh.Close()
	log.Println("Connected to RabbitMQ")

	// pg conn
	log.Println("Connecting with Postgres...")
	db, err = sql.Open("postgres", "postgres://pulseuser:supersecret@localhost:5432/pulse?sslmode=disable")
	if err != nil {
		log.Fatal("Database connection failed: ", err)
	}
	log.Println("Session created with Postgres")

	// ----- Scylla conn -----
	log.Println("Connecting with ScyllaDB...")
	scyllaCluster = gocql.NewCluster("127.0.0.1")
	scyllaCluster.Keyspace = "pulse"
	scyllaCluster.Consistency = gocql.Quorum

	scyllaSession, err = scyllaCluster.CreateSession()
	if err != nil {
		log.Printf("Failed to connect to ScyllaDB: %v", err)
		return
	}
	defer scyllaSession.Close()
	log.Println("Session created with ScyllaDB...")
	// ----- END -----

	hub := newHub()
	go hub.run()

	r := gin.Default()

	// CORS
	config := cors.DefaultConfig()
	config.AllowOrigins = []string{"*"}
	config.AllowMethods = []string{"GET", "POST", "PUT", "DELETE", "OPTIONS"}
	config.AllowHeaders = []string{"Origin", "Content-Type", "Authorization"}

	r.Use(cors.New(config))

	r.GET("/health-check", healthCheck)
	r.POST("/signup", signUpHandler)
	r.POST("/login", loginHandler)

	// utility endpoints
	r.GET("/ws", func(c *gin.Context) {
		wsHandler(hub, c)
	})

	r.GET("/verify_token", func(c *gin.Context) {
		_, claims, err := verifyToken(c)
		if err != nil {
			c.JSON(http.StatusUnauthorized, gin.H{"error": err.Error()})
			return
		}

		c.JSON(http.StatusOK, gin.H{
			"user_id":  claims["user_id"],
			"username": claims["username"],
			"exp":      claims["exp"],
		})
	})

	// Secured endpoints
	authGroup := r.Group("/")
	authGroup.Use(AuthMiddleware(string(jwtSecret)))

	authGroup.GET("/chats", getChats)
	authGroup.GET("/messages", getMessages)

	r.Run(":8888")
}
