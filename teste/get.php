<?php

header("Content-Type: application/json");

if (file_exists("dados.json")) {
    echo file_get_contents("dados.json");
} else {
    echo json_encode([
        "device" => "teste",
        "distance" => 50
    ]);
}