# Battlestar-Dreamcast Multiplayer Plan

This document outlines the plan for implementing a two-player online multiplayer mode for Battlestar-Dreamcast.

## High-Level Architecture

The multiplayer mode will use a **client-server architecture**. One player will act as the **host** (server) and the other will be the **client**. This model is simpler to implement and manage than a peer-to-peer architecture for a two-player game.

- **Host**: The host is responsible for managing the game state, including enemy spawning, AI, and the positions of all game objects. The host will receive input from the client and send regular game state updates back to the client.
- **Client**: The client is responsible for sending its input to the host and rendering the game state received from the host.

## Networking Protocol

We will use a combination of TCP and UDP for networking:

-   **TCP**: TCP will be used for the initial **lobby system** to establish a reliable connection between the host and the client.
-   **UDP**: UDP will be used for **in-game communication** to send game state updates and player input. UDP is connectionless and has lower overhead than TCP, which is ideal for real-time games. However, since UDP is unreliable, we will need to implement mechanisms to handle packet loss for critical game events.

## Implementation Phases

The implementation will be broken down into the following phases:

### Phase 1: Basic Network Initialization and Lobby System

1.  **Enable Networking**:
    -   Add the `INIT_NET` flag to `KOS_INIT_FLAGS` in `main.cpp` to initialize the KallistiOS network stack.
2.  **Create Lobby System**:
    -   Add a "Multiplayer" option to the main menu in `menu_system.cpp`.
    -   This will lead to a new screen with two options: "Host Game" and "Join Game".
3.  **Host Game Functionality**:
    -   Create a new class or set of functions to handle the hosting process.
    -   When a player chooses to host, the game will:
        -   Create a TCP listening socket on a predefined port (e.g., 1337).
        -   Retrieve and display the host's IP address on the screen.
        -   Enter a "waiting for player" state.
4.  **Join Game Functionality**:
    -   Create a new class or set of functions to handle the client connection process.
    -   When a player chooses to join, the game will:
        -   Display a simple UI to allow the player to enter the host's IP address.
        -   Attempt to connect to the host's IP address and port.
5.  **Connection Establishment**:
    -   Once the client connects to the host, both games will transition from the lobby to the gameplay state.

### Phase 2: Game State Synchronization

1.  **Define Network Packets**:
    -   Define the data structures for the network packets that will be sent between the host and client.
    -   **Client-to-Host Packet**: Will contain the client's controller input.
    -   **Host-to-Client Packet**: Will contain the game state (player positions, enemy positions, bullet positions, score, etc.).
2.  **Implement UDP Communication**:
    -   Create UDP sockets on both the host and the client for sending and receiving game state packets.
3.  **Synchronize Game State**:
    -   **Host**:
        -   In the main game loop, receive the client's input packet.
        -   Update the game state based on the local player's input and the client's input.
        -   Send the updated game state to the client in a UDP packet.
    -   **Client**:
        -   In the main game loop, send the local player's input to the host in a UDP packet.
        -   Receive the game state packet from the host.
        -   Update the local game world based on the received data.

### Phase 3: Gameplay Implementation and Latency Handling

1.  **Integrate Networking with Game Logic**:
    -   Modify the `GameEngine` class to handle the multiplayer game loop.
    -   Ensure that both players' ships are rendered correctly.
    -   The host will be responsible for all game logic (collisions, scoring, etc.).
2.  **Handle Latency and Packet Loss**:
    -   **Client-Side Prediction**: Implement client-side prediction for the client's ship to make it feel more responsive. The client will predict its own movement based on its input and correct its position when it receives an update from the host.
    -   **Interpolation**: Implement interpolation for the other player's ship and enemies on the client to smooth out their movement.
    -   **Packet Loss**: For critical game events (e.g., firing a weapon, taking damage), we can send the event in multiple packets to increase the chance of it being received.

## Future Enhancements

-   **Peer-to-Peer with NAT Traversal**: For a more robust solution, we could explore a peer-to-peer architecture with NAT traversal techniques (e.g., STUN).
-   **Hostname Support**: Allow players to connect using hostnames instead of IP addresses by integrating the DNS client example.
-   **Time Synchronization**: Use the NTP example to synchronize the clocks of both players to ensure a consistent game state.
