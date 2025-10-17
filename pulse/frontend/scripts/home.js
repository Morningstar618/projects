document.addEventListener("DOMContentLoaded", () => {
    // --- State Management ---
    let selectedUserId = null; // ID of the user we are currently chatting with
    const chatHistories = {}; // Stores message arrays, e.g., { "userId1": [msg1, msg2], "userId2": [...] }
    const backendPort = 8888

    // --- Get User Info & Validate ---
    const currentUserId = parseInt(sessionStorage.getItem("pulse_user_id"), 10);
    const currentUsername = sessionStorage.getItem("pulse_username");
    const jwtToken = sessionStorage.getItem("pulse_token");

    // ... validation logic ...

    // --- DOM Elements ---
    const userList = document.getElementById("user-list");
    const chatBox = document.getElementById("chat-box");
    const messageInput = document.getElementById("message-input");
    const sendButton = document.getElementById("send-btn");
    const callButton = document.getElementById("call-btn");

    // --- WebSocket Connection ---
    const socket = new WebSocket(`ws://localhost:${backendPort}/ws?userId=${currentUserId}&username=${currentUsername}`);

    socket.onmessage = function(event) {
        const msg = JSON.parse(event.data);

        if (msg.type === 'user_list') {
            updateUserList(msg.users);
        } else if (msg.type === 'private_message') {
            handleIncomingMessage(msg);
        }
    };
    // ... other socket handlers ...

    // --- Event Listeners ---
    userList.addEventListener("click", handleUserSelection);
    sendButton.addEventListener("click", sendMessage);
    callButton.addEventListener("click", startCall);
    messageInput.addEventListener("keyup", (event) => {
        if (event.key === "Enter") {
            sendMessage();
        }
    });

    // --- Core Functions ---

    async function handleUserSelection(event) {
        const li = event.target.closest('li');
        if (!li || !li.dataset.userid) return;

        selectedUserId = parseInt(li.dataset.userid, 10);
        if (selectedUserId === currentUserId) return;

        // ✅ Fetch past messages only if not already cached
        if (!chatHistories[selectedUserId]) {
            const convId = `${currentUserId}_${selectedUserId}`;
            try {
                const res = await fetch(`http://localhost:${backendPort}/messages?conversationId=${convId}`, {
                    method: 'GET',
                    headers: {
                        'Authorization': `Bearer ${jwtToken}`,
                        'Content-Type': 'application/json'
                    }
                });
                const history = await res.json();
                chatHistories[selectedUserId] = history;
            } catch (err) {
                console.error("Failed to fetch chat history:", err);
                chatHistories[selectedUserId] = []; // fallback
            }
        }

        renderChatWindow();
    }


    function sendMessage() {
        const text = messageInput.value.trim();
        if (!text || !selectedUserId) {
            return;
        }

        const message = {
            type: "private_message",
            text: text,
            recipientID: selectedUserId,
        };

        socket.send(JSON.stringify(message));
        messageInput.value = "";
    }
    
    function startCall() {
        if (!selectedUserId) {
            alert("Please select a user to call.");
            return;
        }

        // Placeholder logic — will later initiate WebRTC offer
        console.log(`Calling user ID: ${selectedUserId}`);
        
        // Example: Send signaling message over WebSocket
        const message = {
            type: "call_request",
            from: currentUserId,
            to: selectedUserId
        };
        socket.send(JSON.stringify(message));
    }

    function handleIncomingMessage(msg) {
        // Determine the chat partner's ID (the person who is NOT us)
        const chatPartnerId = msg.senderID === currentUserId ? msg.recipientID : msg.senderID;

        // Initialize history if it doesn't exist
        if (!chatHistories[chatPartnerId]) {
            chatHistories[chatPartnerId] = [];
        }
        chatHistories[chatPartnerId].push(msg);

        // If we are currently viewing the chat with this person, render it immediately
        if (chatPartnerId === selectedUserId) {
            renderChatWindow();
        }
    }

    function renderChatWindow() {
        // Update active class on user list
        document.querySelectorAll("#user-list li").forEach(li => {
            li.classList.toggle('active-chat', parseInt(li.dataset.userid, 10) === selectedUserId);
        });

        // Clear chat box and render messages for the selected user
        chatBox.innerHTML = "";
        const history = chatHistories[selectedUserId] || [];

        history.forEach(msg => {
            const messageDiv = document.createElement("div");
            messageDiv.className = "message";

            // Align our messages to the right, others to the left
            const isMyMessage = msg.senderID === currentUserId;
            messageDiv.style.alignItems = isMyMessage ? 'flex-end' : 'flex-start';

            const bubble = document.createElement('div');
            bubble.className = "message-bubble";

            // ✅ KEY FIX: The 'message-sender' div has been completely removed.
            bubble.innerHTML = `<p class="message-text">${msg.text}</p>`;

            // Style the bubble differently for sent vs. received
            bubble.style.backgroundColor = isMyMessage ? '#e0f2ff' : '#f1f5f9';
            bubble.style.borderTopLeftRadius = isMyMessage ? '12px' : '0';
            bubble.style.borderTopRightRadius = isMyMessage ? '0' : '12px';

            messageDiv.appendChild(bubble);
            chatBox.appendChild(messageDiv);
        });
        chatBox.scrollTop = chatBox.scrollHeight;
    }    

    function updateUserList(users) {
        userList.innerHTML = "";
        users.forEach(user => {
            const li = document.createElement("li");
            li.dataset.userid = user.id;
            li.dataset.username = user.username;
            li.textContent = user.username;
            if (user.id === currentUserId) {
                li.textContent += " (You)";
            }
            userList.appendChild(li);
        });
        // After updating the list, re-apply the active class if a user is selected
        if(selectedUserId) {
            const selectedLi = userList.querySelector(`li[data-userid='${selectedUserId}']`);
            if (selectedLi) {
                selectedLi.classList.add('active-chat');
            }
        }
    }

    // Logout functionality
    document.querySelector(".logout-btn").addEventListener("click", () => {
	sessionStorage.clear();
	window.location.href = "index.html";
    });

    // Chats functionality
    document.querySelector(".chats-btn").addEventListener("click", () => {
	window.location.href = "chats.html";
    });
});