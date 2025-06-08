# Real-Time Chat App (Multithreading)

A simple real-time chat application implemented in C, featuring a multithreaded server and client using POSIX threads and TCP sockets.

---


## Overview

This project demonstrates a basic yet robust chat system in C. The server handles multiple clients concurrently using POSIX threads and mutex locks to ensure thread-safe operations. The client connects to the server over TCP, allowing users to send and receive messages in real time.

## Features

- Multi-threaded server supporting 100+ concurrent TCP client connections
- Public broadcast and private messaging via a simple command-based protocol
- Real-time user presence notifications (join/leave)
- Thread-safe data structures for user management
- Robust buffer and memory handling to prevent overflows and leaks

## Prerequisites

- GCC (or any C compiler supporting C99)
- POSIX-compliant system (Linux, macOS)
- Make (optional but recommended)
