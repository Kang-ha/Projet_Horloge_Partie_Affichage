document.addEventListener("DOMContentLoaded", function() {
    const numbersContainer = document.getElementById("nombre");
    for (let i = 0; i < 24; i++) {
        let angle = (i * 15) - 90;
        let x = 50 + 42 * Math.cos(angle * Math.PI / 180);
        let y = 50 + 42 * Math.sin(angle * Math.PI / 180);
        let numberElement = document.createElement("div");
        numberElement.style.top = `${y}%`;
        numberElement.style.left = `${x}%`;
        numberElement.textContent = i;
        numbersContainer.appendChild(numberElement);
    }

    function updateClock() {
        const now = new Date();
        const hours = now.getHours();
        const minutes = now.getMinutes();
        const seconds = now.getSeconds();

        const hourDeg = (hours * 15) + (minutes / 4);
        const minuteDeg = (minutes * 6) + (seconds / 10);
        const secondDeg = seconds * 6;

        document.getElementById("heure").style.transform = `translate(-50%) rotate(${hourDeg}deg)`;
        document.getElementById("min").style.transform = `translate(-50%) rotate(${minuteDeg}deg)`;
        document.getElementById("sec").style.transform = `translate(-50%) rotate(${secondDeg}deg)`;
    }
    setInterval(updateClock, 1000);
    updateClock();
});