# Protocol

The communication between the server and client obay a simple request-response protocol. The program works as a single-channel TCP server, serving only one client conversation by turn.

## The conversation between server and client 

The conversation begins when the client connects to the server and it finishes when one of the following events occurs:

- The client closes the connection
- The server closes the connection
- A timeout occurs

During a conversation, the client can send an undertermined number of requests to the server. The client must set the keep-alive byte if it wants to make more than one succesive request. The position of the keep-alive byte and other package fields are discussed below.

## When server closes the connection?

The server closes the connection after respond to a request with keep-alive other then 48.

## The packt format

The packat has the following format:

![image](https://user-images.githubusercontent.com/9665358/130887559-d8a3c0d9-7783-484e-a306-86e3189974f7.png)

The three first fields, namely **status**, **keep-alive** and **data size**, have predefined sizes in bytes (4, 1 and 4, respectivelly). The **data** field is the only one with an undetermined number of bytes. In a well-formed packat, the size of the data segment is set in the **data-size** field.

Obs.: for several practical reasons, the server assumes that **data-size** is bounded to a max positive value.
