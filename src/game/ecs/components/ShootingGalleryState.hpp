#pragma once

#include "../Component.hpp"
#include "../Entity.hpp"
#include <string>

// Forward declaration
class Timer;

namespace game::ecs::components {
    
    /**
     * Game State enumeration for duck shooter game
     * 
     * Defines the three main game states with clear transitions:
     * - MENU: Initial state, showing instructions
     * - PLAYING: Active gameplay with timer and scoring
     * - GAME_OVER: End state with final score display
     */
    enum class GameState {
        MENU,        ///< Menu/start screen state
        PLAYING,     ///< Active gameplay state
        GAME_OVER    ///< Game over/results state
    };
    
    /**
     * ShootingGalleryState Component - Duck Shooter Game
     * 
     * Singleton component that manages the global game state including
     * scoring, timing, accuracy tracking, high score persistence, and
     * game state transitions. This is the central game management component.
     * 
     * Features:
     * - Singleton pattern for global state access
     * - 60-second game rounds with countdown timer
     * - Score tracking with regular/boss duck differentiation
     * - Accuracy calculation (shots fired vs shots hit)
     * - High score persistence across game sessions
     * - Game state management (menu → playing → game over)
     * - Duck spawning configuration (90% regular, 10% boss)
     * - Auto-start functionality for seamless gameplay
     */
    class ShootingGalleryState : public game::ecs::Component {
    private:
        static ShootingGalleryState* instance_;
        
    public:
        /**
         * Get the singleton instance of the game state
         * 
         * @return Reference to the singleton instance
         * @throws std::runtime_error if instance not created
         */
        static ShootingGalleryState& getInstance();
        
        /**
         * Create the singleton instance with an entity and Timer dependency.
         * @param entity The entity this component belongs to
         * @param timer Timer instance for hardware-independent timing (required)
         * @throws std::invalid_argument if timer is null
         */
        static void createInstance(const game::ecs::Entity& entity, const Timer* timer);
        
        /**
         * Check if the singleton instance exists
         * 
         * @return true if instance has been created
         */
        static bool hasInstance();
        
        /**
         * Destroy the singleton instance (for cleanup)
         */
        static void destroyInstance();
        
        /**
         * Copy constructor and assignment operator (deleted for singleton)
         */
        ShootingGalleryState(const ShootingGalleryState& other) = delete;
        ShootingGalleryState& operator=(const ShootingGalleryState& other) = delete;
        
        /**
         * Move constructor and assignment operator (deleted for singleton)
         */
        ShootingGalleryState(ShootingGalleryState&& other) = delete;
        ShootingGalleryState& operator=(ShootingGalleryState&& other) = delete;
        
        /**
         * Destructor
         */
        ~ShootingGalleryState() override = default;
        
        // Public member variables for direct access (ECS pattern)
        int score = 0;                          ///< Current game score
        float timeRemaining = GAME_DURATION;    ///< Time left in current round (seconds)
        GameState state = GameState::MENU;      ///< Current game state
        int targetsHit = 0;                     ///< Number of targets successfully hit
        int shotsFired = 0;                     ///< Total number of shots fired
        int highScore = 0;                      ///< Highest score achieved
        
        // Spawn timing variables (using game time, not system time)
        double gameStartTime = 0.0;             ///< Time when the game started (game time)
        double lastTargetSpawn = 0.0;           ///< Time when last target was spawned (game time)
        
        /**
         * Add points to the current score and update statistics
         * 
         * @param points Points to add to score
         */
        void addScore(int points);
        
        /**
         * Record a shot being fired (for accuracy calculation)
         */
        void recordShot();
        
        /**
         * Start a new game round
         */
        void startGame();
        
        /**
         * End the current game round
         */
        void endGame();
        
        /**
         * Calculate current accuracy percentage
         * 
         * @return Accuracy as percentage (0.0 to 100.0)
         */
        float getAccuracy() const;
        
        /**
         * Check if game is in playing state
         * 
         * @return true if state == PLAYING
         */
        bool isPlaying() const;
        
        /**
         * Check if game is in game over state
         * 
         * @return true if state == GAME_OVER
         */
        bool isGameOver() const;
        
        /**
         * Check if game is in menu state
         * 
         * @return true if state == MENU
         */
        bool isMenu() const;
        
        /**
         * Update the game timer (decreases time remaining)
         * 
         * @param deltaTime Time elapsed in seconds
         */
        void updateTimer(float deltaTime);
        
        /**
         * Check if time has run out
         * 
         * @return true if time remaining <= 0
         */
        bool isTimeUp() const;
        
        /**
         * Check if it's time to spawn a new target based on spawn rate
         * 
         * @return true if a target should be spawned, false otherwise
         */
        bool shouldSpawnTarget();
        
        /**
         * Reset all game statistics for a new round
         */
        void resetStats();
        
        /**
         * Get the game state as a string for debugging
         * 
         * @return String representation of current state
         */
        std::string getStateString() const;
        
        /**
         * Set the game state
         * 
         * @param newState The new game state
         */
        void setState(GameState newState);
        
        /**
         * String representation for debugging
         * 
         * @return String describing complete game state
         */
        std::string toString() const;
        
        // Static constants for duck shooter game (matching specification)
        static constexpr float GAME_DURATION = 60.0f;              ///< Game round duration (seconds)
        static constexpr float REGULAR_DUCK_SPAWN_WEIGHT = 0.9f;   ///< 90% regular duck spawn rate
        static constexpr float BOSS_DUCK_SPAWN_WEIGHT = 0.1f;      ///< 10% boss duck spawn rate
        static constexpr float DUCK_SPAWN_RATE = 2.0f;             ///< Ducks per second spawn rate
        // Note: Point values (regular: 10, boss: 50) are now loaded from GameData.json
        
    private:
        /**
         * Private constructor for singleton pattern with Timer dependency.
         * @param entity The entity this component belongs to
         * @param timer Timer instance for hardware-independent timing (required)
         */
        explicit ShootingGalleryState(const game::ecs::Entity& entity, const Timer* timer);
        
        const Timer* timer_;  ///< Timer reference for hardware-independent timing
        
        /**
         * Load high score from persistent storage
         */
        void loadHighScore();
        
        /**
         * Save high score to persistent storage
         */
        void saveHighScore();
        
        /**
         * Get the high score file path
         * 
         * @return Path to high score storage file
         */
        std::string getHighScoreFilePath() const;
    };
    
    /**
     * Utility function to convert GameState to string
     * 
     * @param state The game state to convert
     * @return String representation of the game state
     */
    std::string gameStateToString(GameState state);
    
    /**
     * Utility function to convert string to GameState
     * 
     * @param stateStr String representation ("menu", "playing", "game_over")
     * @return Corresponding GameState (defaults to MENU for invalid strings)
     */
    GameState stringToGameState(const std::string& stateStr);
    
} // namespace game::ecs::components 