const express = require("express");
const fs = require("fs");

const app = express();

// permite receber POST do ESP32
app.use(express.urlencoded({ extended: true }));
app.use(express.json());


// 🔹 ENDPOINT PRINCIPAL (ESP32 envia aqui)
app.post("/sensor", (req, res) => {

  const { device, distance } = req.body;

  if (!distance) {
    return res.json({
      status: "erro",
      msg: "distance não recebido"
    });
  }

  let dados = {
    device: device || "desconhecido",
    distance: parseFloat(distance),
    timestamp: new Date()
  };

  // salva em arquivo
  fs.writeFileSync("dados.json", JSON.stringify(dados, null, 2));

  console.log("Recebido:", dados);

  res.json({
    status: "ok",
    recebido: dados
  });
});


// 🔹 ENDPOINT PARA DASHBOARD
app.get("/dados", (req, res) => {

  if (fs.existsSync("dados.json")) {
    res.sendFile(__dirname + "/dados.json");
  } else {
    res.json({
      device: "caixa01",
      distance: 50
    });
  }

});


app.listen(3000, () => {
  console.log("Servidor rodando na porta 3000");
});