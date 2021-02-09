# LicensingSystem
A system for setting up and managing client licenses for software products on Windows

### How it works
Server backend written in Java designed to communicate with the MySQL database                                                                                                             
Client frontend written in C++ that communicates with the server

### Features
Secure TCP connections using Winsock2 and Java sockets                                                                                                                            
RSA key exchange and AES encrypted packets using CryptoPP and default Java encryption libraries                                                                                                
Database can host N amount of products on the server and distribute them to clients                                                                                                
Products are runtime protected so they cannot be ran without a valid license

### Status
This project is working, but more could be added.                                                                                                                               
I am writing an ImGui menu for a demo.    
I am currently writing a C++ UI library that I will add to this project for the UI once its complete

### Demos
client's point of view account creation, login, and product run: https://streamable.com/w6gq8k
