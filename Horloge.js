function drawClock() {
    const canvas = document.getElementById('clock');
    const ctx = canvas.getContext('2d');
    const radius = canvas.height / 2;
    ctx.translate(radius, radius);
    const clockRadius = radius * 0.90;
    setInterval(() => {
        ctx.clearRect(-radius, -radius, canvas.width, canvas.height);
        drawFace(ctx, clockRadius);
        drawNumbers(ctx, clockRadius);
        drawTime(ctx, clockRadius);
    }, 1000);
}

function drawFace(ctx, radius) {
    ctx.save();
    ctx.beginPath();
    ctx.arc(0, 0, radius, 0, 2 * Math.PI);
    ctx.fillStyle = 'white';
    ctx.fill();

    const grad = ctx.createRadialGradient(0, 0, radius * 0.95, 0, 0, radius * 1.05);
    grad.addColorStop(0, '#333');
    grad.addColorStop(0.5, 'white');
    grad.addColorStop(1, '#333');
    ctx.strokeStyle = grad;
    ctx.lineWidth = radius * 0.1;
    ctx.stroke();

    ctx.beginPath();
    ctx.arc(0, 0, radius * 0.1, 0, 2 * Math.PI);
    ctx.fillStyle = '#333';
    ctx.fill();
    ctx.restore();
}

function drawNumbers(ctx, radius) {
    ctx.save();
    ctx.font = `${radius * 0.10}px arial`;
    ctx.textBaseline = 'middle';
    ctx.textAlign = 'center';
    for (let num = 1; num <= 24; num++) {
        const ang = num * Math.PI / 12;
        ctx.rotate(ang);
        ctx.translate(0, -radius * 0.85);
        ctx.rotate(-ang);
        ctx.fillText(num.toString(), 0, 0);
        ctx.rotate(ang);
        ctx.translate(0, radius * 0.85);
        ctx.rotate(-ang);
    }
    ctx.restore();
}

function drawTime(ctx, radius) {
    const now = new Date();
    const hour = now.getHours();
    const minute = now.getMinutes();
    const second = now.getSeconds();
    const hourAngle = (hour % 24) * Math.PI / 12 + (minute * Math.PI / (12 * 60)) + (second * Math.PI / (12 * 3600));
    drawHand(ctx, hourAngle, radius * 0.5, radius * 0.07);
    const minuteAngle = (minute * Math.PI / 30) + (second * Math.PI / (30 * 60));
    drawHand(ctx, minuteAngle, radius * 0.8, radius * 0.07);
    const secondAngle = second * Math.PI / 30;
    drawHand(ctx, secondAngle, radius * 0.9, radius * 0.02);
}

function drawHand(ctx, pos, length, width) {
    ctx.save();
    ctx.beginPath();
    ctx.lineWidth = width;
    ctx.lineCap = 'round';
    ctx.moveTo(0, 0);
    ctx.rotate(pos);
    ctx.lineTo(0, -length);
    ctx.stroke();
    ctx.restore();
}

function updateClock() {
    const canvas = document.getElementById('clock');
    const ctx = canvas.getContext('2d');
    const radius = canvas.height / 2;
    ctx.clearRect(0, 0, canvas.width, canvas.height); // Clear the canvas
    ctx.save(); // Save the default state
    ctx.translate(radius, radius); // Move the origin to the center
    const clockRadius = radius * 0.90;
    drawFace(ctx, clockRadius);
    drawNumbers(ctx, clockRadius);
    drawTime(ctx, clockRadius);
    ctx.restore(); // Restore the default state
}

setInterval(updateClock, 1000);
updateClock();// Initial call to display clock immediately

function startTime() {
    var today = new Date();
    var h = today.getHours();
    var m = today.getMinutes();
    m = checkTime(m);
    document.getElementById('time').innerHTML = h + ":" + m;
    var t = setTimeout(startTime, 500);
  }
  function checkTime(i) {
    if (i < 10) {i = "0" + i};  // add zero in front of numbers < 10
    return i;
  }
   
  startTime();