## Game Prototype README

### General Information
- **Game Title:** Beat 'Em Up Dimensions
- **Developer:** I-Tung Hsieh
- **Course:** FIT2096 - Games Programming

### Core Mechanics
- **Portal System**: The core mechanic involves a portal system where players can transport between different dimensions temporarily. The player can activate entry and exit portals, which drastically affect gameplay strategy and navigation. This system allows for evasion and strategic positioning against enemies.

### Physics Interactions
1. **Grappling Hook**: Implements impulse physics, allowing the player to either pull themselves towards a point or pull objects towards themselves. This mechanic uses physics forces to create dynamic interactions with both static and dynamic environment elements.
2. **Wrecking Ball**: Demonstrates the use of physics constraints, specifically using a hinge constraint system. The ball is attached to a cable that swings based on physics simulations, creating a destructible force within the environment that can be used against enemies or to alter the surroundings.

### User Interface
- **Main Menu**: Includes a basic navigation menu with game start and exit options, set against a themed background that matches the game's aesthetic.
- **In-Game UI**: Features a mini-map in the bottom right corner and dynamic enemy status indicators. Health bars and awareness icons (exclamation for direct sight and question mark for investigation mode) are displayed above enemies when relevant.

### Artificial Intelligence
- **Custom AI Communication**: Enemies are equipped with a communication system where seeing the player can alert other nearby enemies. Alerted enemies investigate the last known player position and engage in combat if the player is spotted. The AI utilizes a navigation mesh with custom pathways and jump points, enhancing the complexity of enemy movements and interactions.

### Debugging and Settings
- **Blueprint Debugging**: A debug mode can be enabled in the BP_ThirdPersonCharacter under Development Settings. This mode is off by default but can be activated to display additional diagnostic information during gameplay testing.

### Important Files and Code Locations
- **Portal System Code**: Located in `PortalSystem.cpp` and `Portal.cpp`, these files contain all logic related to the creation and management of portals.
- **Physics Mechanics Code**: The grappling hook is implemented in `GrapplingHook.cpp`, and the wrecking ball setup is detailed in `WreckingBall.cpp`.
- **AI and Behavior Trees**: AI logic is primarily located in `EnemyAIController.cpp` and associated Behavior Tree assets within the Unreal project.

### Additional Notes
- Please refer to the included documentation for a detailed description of all game mechanics and their expected interactions within the game environment.
- For any issues or questions about the prototype, please contact `ihsi0002@student.monash.edu`.