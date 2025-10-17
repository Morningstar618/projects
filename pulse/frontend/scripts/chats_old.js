document.addEventListener("DOMContentLoaded", async () => {
    const chatList = document.getElementById("chat-list");
    const currentUserId = sessionStorage.getItem("pulse_user_id");

    if (!currentUserId) {
        alert("User session not found. Please log in again.");
        window.location.href = "index.html";
        return;
    }

    try {
        const res = await fetch(`http://localhost:8080/chats?currentUserId=${currentUserId}`);
        if (!res.ok) throw new Error("Failed to fetch chats");

        const chats = await res.json();
        chatList.innerHTML = ""; // Clear "Loading..."

        if (chats.length === 0) {
            chatList.innerHTML = "<li>No past chats found.</li>";
        } else {
            chats.forEach(chat => {
                const li = document.createElement("li");
                li.textContent = `To: ${chat.recipientUsername} — "${chat.text}"`;
                chatList.appendChild(li);
            });
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
