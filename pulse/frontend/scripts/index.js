const API_BASE = "http://localhost:8888";

// --- Form Switching ---
function showSignUp() {
  document.getElementById("login-form").classList.add("hidden");
  document.getElementById("signup-form").classList.remove("hidden");
}

function showLogin() {
  document.getElementById("signup-form").classList.add("hidden");
  document.getElementById("login-form").classList.remove("hidden");
}

// --- Signup Logic ---
async function handleSignUp(event) {
  event.preventDefault();
  const email = document.getElementById("signup-email").value.trim();
  const password = document.getElementById("signup-password").value;
  const confirm = document.getElementById("verify-password").value;

  if (password !== confirm) {
    alert("Passwords do not match.");
    return;
  }
  if (!email || !password) {
    alert("Please fill out all fields.");
    return;
  }

  try {
    const response = await fetch(`${API_BASE}/signup`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ email, password }),
    });

    const data = await response.json();
    if (!response.ok) {
      throw new Error(data.error || "Signup failed.");
    }

    alert("Account created successfully. Please log in.");
    showLogin();
    document.getElementById("signup-form").reset(); // Clear form
  } catch (err) {
    console.error("Signup error:", err);
    alert(err.message);
  }
}

async function handleLogin(event) {
  event.preventDefault();
  const email = document.getElementById("login-email").value.trim();
  const password = document.getElementById("login-password").value;

  if (!email || !password) {
    alert("Please enter both email and password.");
    return;
  }

  try {
    const response = await fetch(`${API_BASE}/login`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ email, password }),
    });

    const data = await response.json();
    if (!response.ok) {
      throw new Error(data.error || "Invalid credentials.");
    }

    // Store token securely
    sessionStorage.setItem("pulse_token", data.token);
    sessionStorage.setItem("pulse_user_id", data.user_id);
    sessionStorage.setItem("pulse_username", data.username);

    // Redirect without leaking credentials
    window.location.href = "home.html";

  } catch (err) {
    console.error("Login error:", err);
    alert(err.message);
  }
}

function parseJwt(token) {
  const base64Url = token.split('.')[1];
  const base64 = decodeURIComponent(atob(base64Url).split('').map(c => {
    return `%${('00' + c.charCodeAt(0).toString(16)).slice(-2)}`
  }).join(''));
  return JSON.parse(base64);
}


//------------------------------------------------------------------------

const token = sessionStorage.getItem("pulse_token");
const userData = token ? parseJwt(token) : null;
