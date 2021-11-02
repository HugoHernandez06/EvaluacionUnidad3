---------------------------------------------
Indicaciones para compilar y lanzar:
- Server: 
    1. gcc -Wall server.c -o server -lpthread -lrt
    2. ./server

- Client:
    1. gcc -Wall client.c -o client -lpthread -lrt
    2. ./client

Para establer la comunicion el cliente debe enviar un mensaje al server por medio de la consola
---------------------------------------------
 Problemas:
 - Comunicación desde el Server al Client (Problemas con trigger - funcSender)
 - Problemas con exit al enviar el mensaje con trigger

---------------------------------------------
 NOTA IMPORTANTE!!!

 Pese a nuestros esfuerzos el resultado no fue el esperado por ninguno de los dos, pero la logica y arquitectura del programa nos quedó muy clara con los diferentes conceptos, pero nos falló al momento de plasmarlo en código. 
