package main

import (
	"database/sql"
	"fmt"
	"log"
	"net/http"
	"sort"
	"strconv"
	"strings"
	"time"

	"github.com/gin-gonic/gin"
	"github.com/golang-jwt/jwt/v5"
)

func getChats(c *gin.Context) {
	type Chat struct {
		RecipientID string
		Message     string
		Timestamp   int64
		MessageType string
	}

	userID := c.Query("currentUserId")
	if userID == "" {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Missing currentUserId"})
		return
	}

	// Fetching conversations ids for the current user
	var conversations []string
	var convo string

	sid_iter := scyllaSession.Query(`
		SELECT conversation_id FROM messages
		WHERE sender_id = ?
		ALLOW FILTERING
	`, userID).Iter()

	for sid_iter.Scan(&convo) {
		if !contains(conversations, convo) {
			conversations = append(conversations, convo)
		}
	}

	if err := sid_iter.Close(); err != nil {
		log.Printf("Error closing sid_iter in getChats: %v", err)
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to close iterator"})
		return
	}

	rid_iter := scyllaSession.Query(`
		SELECT conversation_id FROM messages
		WHERE recipient_id = ?
		ALLOW FILTERING
	`, userID).Iter()

	for rid_iter.Scan(&convo) {
		if !contains(conversations, convo) {
			conversations = append(conversations, convo)
		}
	}

	if err := rid_iter.Close(); err != nil {
		log.Printf("Error closing rid_iter in getChats: %v", err)
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to close iterator"})
		return
	}

	chatsMap := make(map[string][]Chat)

	var (
		recipientID, messageText, messageType string
		timestamp                             int64
	)

	for _, convo_id := range conversations {
		iter := scyllaSession.Query(`
			SELECT recipient_id, message_text, timestamp, message_type 
			FROM messages 
			WHERE conversation_id = ? 
			ALLOW FILTERING
		`, convo_id).Iter()

		for iter.Scan(&recipientID, &messageText, &timestamp, &messageType) {
			chatsMap[convo_id] = append(chatsMap[convo_id], Chat{
				RecipientID: recipientID,
				Message:     messageText,
				Timestamp:   timestamp,
				MessageType: messageType,
			})
		}

		if err := iter.Close(); err != nil {
			log.Printf("Error closing iterator inside loop in getChats: %v", err)
			c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to fetch chats"})
			return
		}
	}

	all_conversations := make(map[string][]Chat)

	for k, v := range chatsMap {
		parts := strings.Split(k, "_")
		sender := parts[0]
		recipient := parts[1]

		var key string
		// Let key be always the recipient
		if sender != userID {
			key = sender
		} else {
			key = recipient
		}

		for _, msg := range v {
			all_conversations[key] = append(all_conversations[key], Chat{
				RecipientID: msg.RecipientID,
				Message:     msg.Message,
				Timestamp:   msg.Timestamp,
				MessageType: msg.MessageType,
			})
		}
	}

	for _, v := range all_conversations {
		sort.Slice(v, func(i, j int) bool {
			return v[i].Timestamp < v[j].Timestamp
		})
	}

	c.JSON(http.StatusOK, all_conversations)
}

func getMessages(c *gin.Context) {
	// Extract conversationId from the query parameter
	conversationID := c.Query("conversationId")
	if conversationID == "" {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Missing conversationId"})
		return
	}

	// Determine both variations: 1_2 and 2_1
	parts := strings.Split(conversationID, "_")
	if len(parts) != 2 {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Invalid conversationId format"})
		return
	}
	reverseConversationID := fmt.Sprintf("%s_%s", parts[1], parts[0])

	conversationIDs := []string{conversationID, reverseConversationID}
	var messages []PulseDbMessage

	for _, convID := range conversationIDs {
		iter := scyllaSession.Query(`
			SELECT sender_id, recipient_id, message_text, timestamp, message_type 
			FROM messages 
			WHERE conversation_id = ? 
			ORDER BY timestamp ASC 
			ALLOW FILTERING
		`, convID).Iter()

		var msg PulseDbMessage
		for iter.Scan(&msg.SenderID, &msg.RecipientID, &msg.Text, &msg.Timestamp, &msg.MessageType) {
			messages = append(messages, PulseDbMessage{
				SenderID:    msg.SenderID,
				RecipientID: msg.RecipientID,
				Text:        msg.Text,
				Timestamp:   msg.Timestamp,
				MessageType: msg.MessageType,
			})
		}

		if err := iter.Close(); err != nil {
			log.Printf("Error closing iterator for %s: %v", convID, err)
			c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to fetch messages"})
			return
		}
	}

	// Sort all messages by timestamp in Go
	sort.Slice(messages, func(i, j int) bool {
		return messages[i].Timestamp < messages[j].Timestamp
	})

	c.JSON(http.StatusOK, messages)
}

func signUpHandler(c *gin.Context) {
	type SignUpInput struct {
		Email    string `json:"email" binding:"required,email"`
		Password string `json:"password" binding:"required,min=6"`
	}

	var input SignUpInput
	if err := c.ShouldBindJSON(&input); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	_, err := db.Exec(`INSERT INTO users (email, username, password) VALUES ($1, $2, $3)`, input.Email, "aj", input.Password)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "Email may already be registered"})
		return
	}

	c.JSON(http.StatusCreated, gin.H{"message": "Account Created Successfully"})
}

func loginHandler(c *gin.Context) {
	type LoginInput struct {
		Email    string `json:"email" binding:"required,email"`
		Password string `json:"password" binding:"required"`
	}

	var input LoginInput
	if err := c.ShouldBindJSON(&input); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	var storedPassword string
	var userID int
	var username string

	err := db.QueryRow(`SELECT id, username, password FROM users WHERE email = $1`, input.Email).Scan(&userID, &username, &storedPassword)
	if err == sql.ErrNoRows || input.Password != storedPassword {
		c.JSON(http.StatusUnauthorized, gin.H{"error": "Invalid email or password"})
		return
	} else if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Database error"})
		return
	}

	// Create JWT token
	claims := jwt.MapClaims{
		"user_id":  userID,
		"username": username,
		"exp":      time.Now().Add(8 * time.Hour).Unix(),
	}

	token := jwt.NewWithClaims(jwt.SigningMethodHS256, claims)
	signedToken, err := token.SignedString(jwtSecret)
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to create token"})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"token":    signedToken,
		"user_id":  userID,
		"username": username,
	})
}

// wsHandler is the Gin handler for the WebSocket route.
func wsHandler(hub *Hub, c *gin.Context) {
	// Use c.Query() to get URL parameters in Gin.
	userIDStr := c.Query("userId")
	username := c.Query("username")

	if userIDStr == "" || username == "" {
		log.Println("Missing user info in query parameters")
		c.String(http.StatusBadRequest, "Missing user info")
		return
	}

	userID, err := strconv.Atoi(userIDStr)
	if err != nil {
		log.Printf("Invalid user ID: %v", err)
		c.String(http.StatusBadRequest, "Invalid user ID")
		return
	}

	// Upgrade the HTTP connection to a WebSocket connection.
	// We use c.Writer and c.Request from the Gin context.
	conn, err := upgrader.Upgrade(c.Writer, c.Request, nil)
	if err != nil {
		log.Println("Failed to upgrade connection:", err)
		return
	}

	// Create and register the client.
	client := &Client{
		ID:       userID,
		Username: username,
		conn:     conn,
		hub:      hub,
		send:     make(chan []byte, 256),
	}
	client.hub.register <- client

	// Start the goroutines to handle reading from and writing to the connection.
	go client.writePump()
	go client.readPump()
}

func healthCheck(c *gin.Context) {
	c.JSON(http.StatusOK, gin.H{
		"message": "Dashboard Check",
	})
}
