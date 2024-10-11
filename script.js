let websocket;
let randomInterval; // Variable pour stocker l'intervalle de randomisation

function initWebSocket() {
    websocket = new WebSocket('ws://10.1.224.107/ws'); // Remplace par l'IP de ton ESP32

    websocket.onopen = function() {
        console.log('WebSocket connecté');
    };

    websocket.onmessage = function(event) {
        console.log('Message reçu : ' + event.data);
        // Affiche la valeur du capteur de lumière ambiante
        if (event.data.startsWith("LDR:")) {
            const ldrValue = parseInt(event.data.split(":")[1], 10);
            document.getElementById("ldrValue").innerText = "Valeur LDR : " + ldrValue;

            // Mettre à jour l'icône de luminosité en fonction de la valeur LDR
            updateLdrIcon(ldrValue);
        }
    };

    websocket.onclose = function() {
        console.log('WebSocket déconnecté');
        initWebSocket();
    };
}

function sendColor() {
    const red = document.getElementById("red").value;
    const green = document.getElementById("green").value;
    const blue = document.getElementById("blue").value;
    const message = `${red},${green},${blue}`;
    if (websocket.readyState === WebSocket.OPEN) {
        websocket.send(message);
        // Mettre à jour la couleur de l'icône
        updateColorDisplay(red, green, blue);
    } else {
        console.log("WebSocket n'est pas connecté");
    }
}

function updateColorDisplay(red, green, blue) {
    const colorDisplay = document.getElementById("colorDisplay");
    colorDisplay.style.backgroundColor = `rgb(${red}, ${green}, ${blue})`;
}

function updateLdrIcon(ldrValue) {
    const ldrIcon = document.getElementById("ldrIcon");
    
    // Déterminer l'icône à afficher en fonction de la valeur LDR
    if (ldrValue < 5) {
        ldrIcon.innerHTML = "🌑"; // Très sombre (Lune noire)
    } else if ( ldrValue < 10) {
        ldrIcon.innerHTML = "🌒"; // Légèrement lumineux (croissant de lune)
    } else if (ldrValue < 20) {
        ldrIcon.innerHTML = "🌓"; // Moyennement lumineux (demi-lune)
    } else if ( ldrValue < 40) {
        ldrIcon.innerHTML = "🌔"; // Lumineux (croissant presque plein)
    } else if (ldrValue < 60) {
        ldrIcon.innerHTML = "🌕"; // Très lumineux (pleine lune)
    } else {
        ldrIcon.innerHTML = "☀️"; // Pleine lumière du jour (soleil)
    }
}

function startRandomColors() {
    // Arrêter toute randomisation précédente
    stopRandomColors();
    
    randomInterval = setInterval(() => {
        const red = Math.floor(Math.random() * 256);
        const green = Math.floor(Math.random() * 256);
        const blue = Math.floor(Math.random() * 256);
        
        document.getElementById("red").value = red;
        document.getElementById("green").value = green;
        document.getElementById("blue").value = blue;
        sendColor(); // Envoyer la couleur aléatoire
    }, 1000); // Changer les couleurs toutes les secondes
}

function stopRandomColors() {
    clearInterval(randomInterval); // Arrêter la randomisation
}

window.onload = function() {
    initWebSocket();
};
