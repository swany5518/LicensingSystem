# LicensingSystem
A system for setting up and managing client licenses for software products on Windows

### How it works
Server backend written in Java designed to communicate with the MySQL database                                                                                                             
Client frontend written in C++ that communicates with the server

### Features
Secure TCP connections using Winsock2 and Java sockets                                                                                                                            
RSA and AES encrypted packets using CryptoPP and default Java encryption libraries                                                                                                
Database can host N amount of products on the server and distribute them to clients                                                                                                
Products are runtime protected so they cannot be ran without a valid license

### Status
This project is not done yet. I am currently writing a C++ UI library that I will use to finish the client UI
