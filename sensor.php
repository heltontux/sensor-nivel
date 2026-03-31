<?php

header("Content-Type: application/json");

// lê JSON recebido
$data = json_decode(file_get_contents("php://input"), true);

// simulação (caso não venha nada)
if (!$data) {
    $data = [
        "device" => "teste",
        "distance" => rand(25, 80),
    ];
}

// lê dados anteriores (se existir)
$dadosAnteriores = null;
if (file_exists("dados.json")) {
    $dadosAnteriores = json_decode(file_get_contents("dados.json"), true);
}

// pega distancia anterior
$distanciaAnterior = null;
if ($dadosAnteriores && isset($dadosAnteriores["distance"])) {
    $distanciaAnterior = $dadosAnteriores["distance"];
}

// adiciona ao JSON atual
$data["distanciaAnterior"] = $distanciaAnterior;

// salva em arquivo (simples)
file_put_contents("dados.json", json_encode($data));

// resposta
echo json_encode([
    "status" => "ok",
    "recebido" => $data
]);