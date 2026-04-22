let distancia = null;
let distanciaAnterior = null;
const tolerancia = 0.3; // cm

const water = document.getElementById("water");
const percentEl = document.getElementById("percent");
const volumeEl = document.getElementById("volume");
const statusEl = document.getElementById("status");
const volEl = document.getElementById("vol");

const ctx = document.getElementById("chart").getContext("2d");

const chart = new Chart(ctx, {
  type: "line",
  data: {
    labels: [],
    datasets: [
      {
        label: "Volume (L)",
        data: [],
        borderWidth: 2,
      },
    ],
  },
  options: {
    scales: {
      y: { beginAtZero: true },
    },
  },
});

function getColor(nivel) {
  if (nivel > 60) return "#22c55e";
  if (nivel > 30) return "#facc15";
  return "#ef4444";
}

function status() {
  let s = "parado";
  if (distancia < distanciaAnterior - tolerancia) {
    s = "enchendo";
  } else if (distancia > distanciaAnterior + tolerancia) {
    s = "esvaziando";
  } else {
    s = "estavel";
  }
  return s;
}

async function atualizar() {
  try {
    const res = await fetch("https://api-sensor-33x4.onrender.com/dados");
    const data = await res.json();
    distancia = data.distance;
    distanciaAnterior = data.distanciaAnterior;
    const alturaMax = 80; // cm
    const alturaMin = 26.5; // cm
    const nivel = ((alturaMax - distancia) / (alturaMax - alturaMin)) * 100;
    const nivelClamp = Math.max(0, Math.min(100, nivel));
    const volume = Math.round((nivelClamp / 100) * 1000);

    water.style.height = nivelClamp + "%";
    water.style.background = getColor(nivelClamp);

    percentEl.innerText = nivelClamp.toFixed(0) + "%";
    volumeEl.innerText = volume + " L";

    const s = status();
    statusEl.innerText = s;
    statusEl.style.color =
      s === "enchendo" ? "green" : s === "esvaziando" ? "red" : "gray";
    volEl.innerText = volume + " L";

    const time = new Date().toLocaleTimeString();

    chart.data.labels.push(time);
    chart.data.datasets[0].data.push(volume);

    if (chart.data.labels.length > 20) {
      chart.data.labels.shift();
      chart.data.datasets[0].data.shift();
    }

    chart.update();
  } catch (e) {
    console.error(e);
  }
}

setInterval(atualizar, 3000);
