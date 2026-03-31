<?php

header("Content-Type: application/json");

// lê JSON recebido
$data = json_decode(file_get_contents("php://input"), true);

// simulação (caso não venha nada)
if (!$data) {
    $data = [
        "device" => "teste",
        "distance" => rand(25, 80)
    ];
}

// salva em arquivo (simples)
file_put_contents("dados.json", json_encode($data));

// resposta
echo json_encode([
    "status" => "ok",
    "recebido" => $data
]);