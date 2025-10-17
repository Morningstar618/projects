package main

type IncomingMessage struct {
	Type        string `json:"type"`
	Text        string `json:"text"`
	RecipientID int    `json:"recipientID"`
}

// Define Message struct with exported fields
type PulseDbMessage struct {
	SenderID    int    `json:"senderID"`
	RecipientID int    `json:"recipientID"`
	Text        string `json:"text"`
	Timestamp   int64  `json:"timestamp"`
	MessageType string `json:"type"`
}
