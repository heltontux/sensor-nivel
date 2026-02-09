# Sensor de nivel
Este projeto é um sensor de nivel de caixa dágua feito com o microcontrolador ESP32 e o sensor ultrassônico JSN-SR04T, utilizando a linguagem C++ e framework arduino.
## Princípios
O sensor fica posicionado na parte de cima da caixa e mede a distância da água até ele. Quanto maior a distância medida, menor será o nível da caixa e vice-versa. Deve ser observada a distância mínima (caixa cheia), a distância média (meio) e a distância máxima (caixa vazia) para que possa ser feita a calibração dos valores e atualização no código do microcontrolador. 
## Observações
Caso não tenha a placa ainda, poderá ser utilizado o site [wokwi](https://wokwi.com) para fazer a simulação do sensor. Se você já tem o microcontrolador ESP32, então deve usar o [Arduino IDE](https://www.arduino.cc/en/software/) para trasferir o código para a placa, tornando o equipamento operacional.
## Requisitos
✔️ Microcontrolador ESP32 Devkit <p>
✔️ Sensor ultrassônico JSN-SR04T <p>
✔️ Jumper de conexão macho/fêmea <p>
✔️ Cabo USB tipo C <p>
✔️ Fonte de energia 5V (pode ser um carregador de celular) <p>
## Montagem
~~~bash
Desenhar o esquema de montagem
~~~
