document.addEventListener("DOMContentLoaded", function() {
  const numbersContainer = document.getElementById("nombre");
  const radius = 175;
  for (let i = 0; i < 24; i++) {
      let angle = (i * 15) - 90;
      let x = 200 + radius * Math.cos(angle * Math.PI / 180);
      let y = 200 + radius * Math.sin(angle * Math.PI / 180);
      let numberElement = document.createElement("div");
      numberElement.style.position = "absolute";
      numberElement.style.top = `${y}px`;
      numberElement.style.left = `${x}px`;
      numberElement.style.transform = "translate(-50%, -50%)"; 
      numberElement.textContent = i;
      numbersContainer.appendChild(numberElement);
  }

  function updateClock() {
      const now = new Date();
      const hours = now.getHours();
      const minutes = now.getMinutes();
      const seconds = now.getSeconds();

      document.getElementById('clock').textContent = 
          `${String(hours).padStart(2, '0')}:${String(minutes).padStart(2, '0')}:${String(seconds).padStart(2, '0')}`;

      const hourDeg = (hours % 24) * 15 + (minutes / 4);
      const minuteDeg = (minutes * 6) + (seconds / 10);
      const secondDeg = seconds * 6;

      document.getElementById("heure").style.transform = `rotate(${hourDeg}deg)`;
      document.getElementById("min").style.transform = `rotate(${minuteDeg}deg)`;
      document.getElementById("sec").style.transform = `rotate(${secondDeg}deg)`;
  }
  setInterval(updateClock, 1000);
  updateClock();

  function pie(ctx, w, h, datalist, colist) {
      var radius = h / 2 - 5;
      var centerx = w / 2;
      var centery = h / 2;
      var total = datalist.reduce((a, b) => a + b, 0);
      var lastend = 0;
      var offset = Math.PI / 2;
      
      for (let x = 0; x < datalist.length; x++) {
          var thispart = datalist[x]; 
          var arcsector = Math.PI * (2 * thispart / total);
          ctx.beginPath();
          ctx.fillStyle = colist[x];
          ctx.moveTo(centerx, centery);
          ctx.arc(centerx, centery, radius, lastend - offset, lastend + arcsector - offset, false);
          ctx.lineTo(centerx, centery);
          ctx.fill();
          ctx.closePath();
          lastend += arcsector;
      }
  }

  var datalist = [35, 25, 20, 12, 7, 1]; 
  var colist = ['blue', 'red', 'green', 'orange', 'gray', 'yellow'];
  var canvas = document.getElementById("canvas3"); 
  if (canvas) {
      var ctx = canvas.getContext('2d');
      pie(ctx, canvas.width, canvas.height, datalist, colist);
  }
});