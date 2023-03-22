#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <vector>

#include "movables/Tank.h"

enum _GAMESTATE {
    GAMESTATE_NONE = 0,
    GAMESTATE_INITIALIZING,
    GAMESTATE_INPLAY,
    GAMESTATE_QUITGAME
};

class HGE;

//! The default angular velocity of the ship when it is in motion
static const float DEFAULT_ANGULAR_VELOCITY = 3.0f; 
//! The default acceleration of the ship when powered
static const float DEFAULT_ACCELERATION = 50.0f;

/**
* The application class is the main body of the program. It will
* create an instance of the graphics engine and execute the
* Update/Render cycle.
*
*/
class Application
{
private:
    int gamestate_;
    HGE* hge_; //!< Instance of the internal graphics engine

    Tank player;
    std::vector<Tank> clients;

    bool Init();
    static bool Loop();
    void Shutdown();

public:
    Application();
    ~Application() throw();

    void Start();
    bool Update();
    void Render();

    template <typename T1, typename T2>
    bool HasCollided( T1 &object, T2 &movable );

    template <typename Mov, typename Tgt>
    bool CheckCollision( Mov &object, Tgt &movable, float timedelta );

	void CreateMissile(float x, float y, float w, int id);
	void CreateMine(float x, float y, int id);

public:
    void SetGameState( int gamestate ) { gamestate_ = gamestate; }
    int  GetGameState( void          ) { return gamestate_; }

public:
    Tank& GetPlayer() { return player; }
    std::vector<Tank>& GetClients() { return clients; }
};

#endif
