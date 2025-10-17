package main

import (
	"context"
	"encoding/json"
	"log"

	"github.com/gocql/gocql"
	"github.com/rabbitmq/amqp091-go"
)

// Message defines the JSON structure expected from RabbitMQ
type Message struct {
	ConversationID string `json:"conversationID"`
	MessageID      string `json:"message_id"`
	SenderID       int    `json:"senderID"`
	RecipientID    int    `json:"recipientID"`
	Text           string `json:"text"`
	Timestamp      int64  `json:"timestamp"`
	Type           string `json:"type"`
}

func main() {
	//Connnect to Scylladb
	cluster := gocql.NewCluster("127.0.0.1")
	cluster.Keyspace = "pulse"
	cluster.Consistency = gocql.Quorum

	session, err := cluster.CreateSession()
	if err != nil {
		log.Fatalf("Failed to connect to ScyllaDB: %v", err)
	}
	defer session.Close()
	log.Println("Connected to ScyllaDB")

	//Connect to RabbitMQ
	conn, err := amqp091.Dial("amqp://chatadmin:chatpass@localhost:5672/")
	if err != nil {
		log.Fatalf("Failed to connect to RabbitMQ: %v", err)
	}
	defer conn.Close()

	ch, err := conn.Channel()
	if err != nil {
		log.Fatalf("Failed to open RabbitMQ channel: %v", err)
	}
	defer ch.Close()

	queueName := "chat_messages"
	msgs, err := ch.Consume(
		queueName, // queue
		"",        // consumer tag
		true,      // auto-ack
		false,     // exclusive
		false,     // no-local
		false,     // no-wait
		nil,       // args
	)
	if err != nil {
		log.Fatalf("Failed to register consumer: %v", err)
	}
	log.Println("Waiting for messages...")

	// Listen forever
	forever := make(chan bool)

	go func() {
		for d := range msgs {
			var msg Message
			if err := json.Unmarshal(d.Body, &msg); err != nil {
				log.Printf("Failed to parse message JSON: %v", err)
				continue
			}

			// Insert into ScyllaDB
			if err := insertMessage(session, msg); err != nil {
				log.Printf("Failed to insert message: %v", err)
			} else {
				log.Printf("Inserted message %s into conversation %s", msg.MessageID, msg.ConversationID)
			}
		}
	}()

	<-forever
}

func insertMessage(session *gocql.Session, msg Message) error {
	messageUUID, err := gocql.ParseUUID(msg.MessageID)
	if err != nil {
		return err
	}

	return session.Query(`
        INSERT INTO messages (conversation_id, timestamp, message_id, sender_id, recipient_id, message_text, message_type)
        VALUES (?, ?, ?, ?, ?, ?, ?)`,
		msg.ConversationID,
		msg.Timestamp,
		messageUUID,
		msg.SenderID,
		msg.RecipientID,
		msg.Text,
		msg.Type,
	).WithContext(context.Background()).Exec()
}
