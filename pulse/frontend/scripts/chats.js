document.addEventListener("DOMContentLoaded", async () => {
    const backendPort = 8888

    const chatList = document.getElementById("chat-list");
    const currentUserId = sessionStorage.getItem("pulse_user_id");
    const jwtToken = sessionStorage.getItem("pulse_token");

    if (!currentUserId) {
        alert("User session not found. Please log in again.");
        window.location.href = "index.html";
        return;
    }

    try {
        const res = await fetch(`http://localhost:${backendPort}/chats?currentUserId=${currentUserId}`, {
            method: 'GET',
            headers: {
                'Authorization': `Bearer ${jwtToken}`,
                'Content-Type': 'application/json'
            }
        });
        if (!res.ok) throw new Error("Failed to fetch chats");

        const chatsMap = await res.json();
        chatList.innerHTML = ""; // Clear "Loading..."

        let hasMessages = false;

        for (const recipientId in chatsMap) {
            const messages = chatsMap[recipientId];

            messages.forEach(chat => {
                // Only show messages where recipient is NOT the current user
                if (chat.RecipientID !== currentUserId) {
                    const li = document.createElement("li");
                    li.textContent = `To User ${chat.RecipientID}: "${chat.Message}"`;
                    chatList.appendChild(li);
                    hasMessages = true;
                }
            });
        }

        if (!hasMessages) {
            chatList.innerHTML = "<li>No past chats found.</li>";
        }
    } catch (err) {
        console.error("Error fetching chat history:", err);
        chatList.innerHTML = "<li>Error loading chat history.</li>";
    }

    document.querySelector(".logout-btn").addEventListener("click", () => {
        sessionStorage.clear();
        window.location.href = "index.html";
    });

    document.getElementById("back-btn").addEventListener("click", () => {
        window.location.href = "home.html";
    });
});
