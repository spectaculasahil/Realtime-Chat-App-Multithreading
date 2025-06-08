Real-Time Chat App (Multithreading)

A simple real-time chat application implemented in C, featuring a multithreaded server and client using POSIX threads and TCP sockets.

Table of Contents

Overview

Features

Prerequisites

Directory Structure

Build Instructions

Usage

Protocol

Contributing

License

Overview

This project demonstrates a basic yet robust chat system in C. The server handles multiple clients concurrently using POSIX threads and mutex locks to ensure thread-safe operations. The client connects to the server over TCP, allowing users to send and receive messages in real time.

Features

Multi-threaded server supporting 100+ concurrent TCP client connections

Public broadcast and private messaging via a simple command-based protocol

Real-time user presence notifications (join/leave)

Thread-safe data structures for user management

Robust buffer and memory handling to prevent overflows and leaks

Prerequisites

GCC (or any C compiler supporting C99)

POSIX-compliant system (Linux, macOS)

Make (optional but recommended)

Directory Structure

├── server.c         # Server application source code
├── client.c         # Client application source code
├── Makefile         # Build script (optional)
└── README.md        # Project documentation

Build Instructions

Clone the repository:

git clone https://github.com/your-username/realtime-chat-c.git
cd realtime-chat-c

Build using GCC:

gcc -pthread -o server server.c
gcc -pthread -o client client.c

(Optional) Use the provided Makefile:

make

Usage

Start the server on the desired port (default: 8080):

./server 8080

Run the client by specifying the server IP and port:

./client 127.0.0.1 8080

Chat commands available on the client:

Broadcast message: Type your message and press Enter.

Private message: /msg <username> <message>

List users: /list

Quit: /quit

Protocol

Messages follow a simple, delimiter-based format:

<COMMAND>:<ARG1>:<ARG2>\n

BROADCAST: MSG:<sender>:<text>

PRIVATE: PRV:<sender>:<recipient>:<text>

JOIN: JOIN:<username>

LEAVE: LEAVE:<username>

LIST_REQUEST: LIST_REQ:<username>

LIST_RESPONSE: LIST_RES:<user1>,<user2>,...
