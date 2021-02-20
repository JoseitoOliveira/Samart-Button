# Samart Button
 Dispositivo de comunicação que ajuda idosos a solicitarem atendimento dos seus cuidadores e/ou responsáveis de maneira rápida, prática e eficiente.


![alt text](Imagens/montagem1.jpeg "Dispositivo apresentado em sala.")

O dispositivo acima se comunica por meio de TCP/IP com um computador que exibe as solicitações.

Foi desenvolvido um software em C# que exibe as notificações recebidas.

![alt text](Imagens/Dashboard.jpeg "Dispositivo apresentado em sala.")

Cada botão do dispositivo representa uma mensagem que pode ser customizada, junto da cor e o número de identificação. Estas e outras configurações são realizadas por meio de uma página web criada pelo dispositivo.

![alt text](Imagens/Painel_conf.jpeg "Tela de configuração.")

Um layout mais compacto da placa foi desenvolvido posteriormente, mas não chegou a ser produzido.

![alt text](Imagens/layout3.3.jpeg "Novo layout.")

As configurações são salvas em memória não volátil, que no caso do ESP8266 é flash. Essas configurações ficam armazenadas em memória da seguinte forma:

![alt text](Imagens/Memória.jpeg "Disposição em memória")


