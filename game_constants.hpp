#ifndef GAME_CONSTANTS_HPP
#define GAME_CONSTANTS_HPP

// Screen dimensions
const float SCREEN_WIDTH = 640.0f;
const float SCREEN_HEIGHT = 480.0f;

// Player movement
const float BASE_PLAYER_SPEED = 2.5f;
const float DIAGONAL_COMPENSATION = 0.707f;  // 1/sqrt(2)
const float BOOST_MULTIPLIER = 2.0f;

// Enemy movement
const float ENEMY_BASE_SPEED = 2.0f;
const float ENEMY_AMPLITUDE = 10.0f;
const float ENEMY_FREQUENCY = 0.05f;
const float MAX_X_THRESHOLD = 320.0f;

// Game mechanics
const int MAX_NUM_BULLETS = 3;
const int MAX_NUM_ENEMY_BULLETS = 1;
const int MAX_NUM_ENEMIES = 10;
const int MAX_DAMAGE_STATES = 6;
const int ENEMIES_PER_WAVE = 5;

// UI Elements
const float UI_CHAR_WIDTH = 16.0f;
const float UI_ARROW_WIDTH = 16.0f;
const float UI_CONTROL_WIDTH = 48.0f;
const float UI_NORMAL_SPACING = 8.0f;
const float UI_CONTROL_SPACING = 32.0f;
const float UI_ROW_SPACING = 30.0f;
const float UI_NAME_START_Y = 160.0f;
const float UI_GRID_OFFSET_Y = 60.0f;

#endif // GAME_CONSTANTS_HPP