package main

import (
	"encoding/json"
	"log"

	"github.com/gorilla/websocket"
)

// Client represents a single user connection.
type Client struct {
	ID       int
	SenderID int
	Username string
	conn     *websocket.Conn
	hub      *Hub
	send     chan []byte
}

// Hub maintains the set of active clients and broadcasts messages.
type Hub struct {
	clients     map[*Client]bool
	clientsByID map[int]*Client
	incoming    chan []byte
	register    chan *Client
	unregister  chan *Client
}

func newHub() *Hub {
	return &Hub{
		incoming:    make(chan []byte),
		register:    make(chan *Client),
		unregister:  make(chan *Client),
		clients:     make(map[*Client]bool),
		clientsByID: make(map[int]*Client),
	}
}

func (h *Hub) run() {
	for {
		select {
		case client := <-h.register:
			h.clients[client] = true
			h.clientsByID[client.ID] = client
			log.Printf("Client registered: %s", client.Username)
			h.broadcastUserList()

		case client := <-h.unregister:
			if _, ok := h.clients[client]; ok {
				delete(h.clients, client)
				delete(h.clientsByID, client.ID)
				close(client.send)
				log.Printf("Client unregistered: %s", client.Username)
				h.broadcastUserList()
			}

		case message := <-h.incoming:
			var msg struct {
				Type        string `json:"type"`
				Text        string `json:"text"`
				RecipientID int    `json:"recipientID"`
				SenderID    int    `json:"senderID"`
			}

			if err := json.Unmarshal(message, &msg); err != nil {
				log.Printf("Error unmarshaling incoming message: %v", err)
				continue
			}

			recipient, recipientOnLine := h.clientsByID[msg.RecipientID]
			sender, senderOnline := h.clientsByID[msg.SenderID]

			outgoingMessage, _ := json.Marshal(msg)

			if recipientOnLine {
				select {
				case recipient.send <- outgoingMessage:
				default:
					log.Printf("Recipient's send channel full: %d", recipient.ID)
				}
			}

			if senderOnline {
				select {
				case sender.send <- outgoingMessage:
				default:
					log.Printf("Sender's send channel full: %d", sender.ID)
				}
			}
		}
	}
}

func (h *Hub) broadcastUserList() {
	var userList []map[string]interface{}
	for client := range h.clients {
		userList = append(userList, map[string]interface{}{
			"id":       client.ID,
			"username": client.Username,
		})
	}

	message := map[string]interface{}{
		"type":  "user_list",
		"users": userList,
	}

	payload, err := json.Marshal(message)
	if err != nil {
		log.Printf("Error marshaling user list: %v", err)
		return
	}
	// Broadcast the payload instead of the raw message map
	for client := range h.clients {
		select {
		case client.send <- payload:
		default:
			close(client.send)
			delete(h.clients, client)
		}
	}
}
