# LicensingSystem
A system for setting up and managing client licenses for software products on Windows

## The problem
Allowing people to run software that I have written without them sharing, cracking, or using without a valid license                                                               
Simple hwid locking does not allow for auto updates, and is harder to control the license times                                                                                     

## The solution
A server based licensing system                                                                                                                                                     

## How it works
Server backend written in Java designed to communicate with the MySQL database                                                                                                     
Client frontend written in C++ that communicates with the server
Products are stored on server and streamed to client when first using, or when updates are required

## Features
Secure TCP connections using Winsock2 and Java sockets                                                                                                                            
RSA key exchange and AES encrypted packets using CryptoPP and default Java encryption libraries                                                                                                
Database can host N amount of products on the server and distribute them to clients                                                                                                
Products are runtime protected so they cannot be ran without a valid license

## Status
This project is working, but more could be added.                                                                                                                               
There is currently a very simple ImGui UI for the client                                                                                                                           
I am currently writing a C++ UI library that I will add to this project for the UI once its complete

## Demos
Client's point of view account creation, login, and product run: https://streamable.com/w6gq8k
