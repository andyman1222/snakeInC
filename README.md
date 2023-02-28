# snakeInC
Not actually Snake in C

Multiplayer snake game written in C with WinSock API. Build with Visual Studio.

To run: Use the compiled binaries from building in VS (ex. x64/release/main.exe) or use the release? (It's been a while since I touched this)
* no args: run as a server on port 27015
* 1 arg: run as a server, specify the port number
* 2 args: run as a client, connect to server (arg 1) : port (arg 2)

Use WASD to move your snake (with the @ symbol). Collect food, marked as #. Don't run into other players or yourself. Note, there's a few oddities with the way the game tick goes that might result in bugs like unregistered collisions.
