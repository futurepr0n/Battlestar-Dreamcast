# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System

This is a Dreamcast homebrew game using the KallistiOS (KOS) development environment.

### Build Commands
- `make` - Build the game executable (battlestar.elf)
- `make clean` - Clean build artifacts
- `make run` - Build and run on Dreamcast via dc-tool
- `make dist` - Create distribution build (stripped)
- `makedc` - Custom build script that performs clean, make, and opens Flycast emulator

### Deployment
- `dc-tool-ip -x battlestar.elf -t 192.168.1.19` - Send to Dreamcast via IP tool

## Architecture Overview

### Core Game Structure
- **Main Loop**: Single game loop in `main.cpp` with nested game session loop
- **Game States**: Managed through singleton `GameState` class for player data persistence
- **Object System**: All game entities use the `obj` struct defined in `loadobj.hpp`
- **Wave-based Gameplay**: Enemy waves spawn progressively with configurable parameters

### Key Components

#### Object Management (`loadobj.hpp/cpp`)
- Central `obj` struct handles all game entities (player, enemies, bullets, background)
- Texture management with damage state support via `damage_textures[]` array
- Fixed-size arrays: `ABSOLUTE_MAX_ENEMIES` (50), `ABSOLUTE_MAX_ENEMY_BULLETS` (5)
- Wave system with `current_wave`, `enemies_defeated`, `wave_in_progress` tracking

#### Rendering (`renderer.hpp/cpp`)
- PowerVR-based rendering using KOS PVR functions
- Scrolling background and starfield effects
- Font rendering system with `load_font_texture()`
- Scene composition in `draw_scene()`

#### Audio System (`audio_system.h/cpp`)
- ModPlug-based tracker music playback
- Stream-based audio with callback system
- Global `g_audioSystem` instance
- Supports XM/MOD/S3M formats

#### Movement & Physics (`movement.hpp/cpp`)
- Collision detection between game objects
- Bullet firing system (`shootChain()`, `shootEnemyChain()`)
- Player and enemy movement logic
- Diagonal movement compensation (0.707 factor)

#### Menu Systems
- `menu_system.hpp/cpp` - Main menu navigation
- `name_selection.hpp/cpp` - Player name input
- `options_menu.hpp/cpp` - Game options
- `initstorysplash.hpp/cpp` - Story intro screens
- `endgamesplash.hpp/cpp` - Victory screens

### Game Constants (`game_constants.hpp`)
Fixed gameplay parameters including:
- Screen dimensions (640x480)
- Player speeds and movement multipliers
- Maximum entity counts and array sizes
- UI layout constants

### Development Environment
- KallistiOS toolchain required (`/opt/toolchains/dc/kos/`)
- Uses custom terminal profile "Dreamcast-Shell" that sources KOS environment
- Links against: `-lpng -lz -lmp3 -lmodplug -lm`

### Asset Management
- `romdisk/` directory contains all game assets (textures, audio, fonts)
- PNG textures with damage state variants (e.g., `battlestar-damage-1.png` through `battlestar-damage-5.png`)
- Compressed font file (`wfont.bin`) converted to object file via `bin2o`
- Music files: XM tracker modules and WAV sound effects

### Code Conventions
- Mixed C/C++ codebase with `extern "C"` blocks for C compatibility
- Header guards using `#ifndef _FILENAME_HPP_` pattern
- Global objects and arrays defined in `loadobj.hpp`
- Texture loading/unloading paired functions for memory management