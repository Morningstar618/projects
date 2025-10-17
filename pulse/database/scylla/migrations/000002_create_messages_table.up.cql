CREATE TABLE IF NOT EXISTS pulse.messages (
    conversation_id text,
    timestamp bigint,
    message_id uuid,
    sender_id int,
    recipient_id int,
    message_text text,
    message_type text,
    PRIMARY KEY ((conversation_id), timestamp)
) WITH CLUSTERING ORDER BY (timestamp ASC);
