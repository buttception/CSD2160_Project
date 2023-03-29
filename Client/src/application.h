#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <vector>
#include <list>
#include <unordered_map>

#include "movables/Tank.h"
#include "movables/Missile.h"
#include "packets/packets_c2s.h"

enum _GAMESTATE
{
	GAMESTATE_NONE = 0,
	GAMESTATE_INITIALIZING,
	GAMESTATE_MENU,
	GAMESTATE_INPLAY,
	GAMESTATE_QUITGAME
};

class HGE;

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
	std::unordered_map<int, Missile> missiles;
	hgeSprite* button;
	hgeSprite* background;
	hgeFont* buttonFont;

	float missile_cooldown;

	// colour
	DWORD red, green, color, black;

	struct pos
	{
		float x, y;
		pos() : x{}, y{} {}
		pos(float _x, float _y) : x{_x}, y{_y} {}
	};

	pos menuPos[2];

	bool Init();
	static bool Loop();
	void Shutdown();

	bool ButtonCollision(float px, float py, float w, float h);

public:

	enum MECHANISMS
	{
		MCH_CLIENT_PREDICTION = 0,
		MCH_RECONCILIATION,
		MCH_INTERPOLATE,
		MCH_COUNT
	};

	std::list<PKT_C2S_TankMovement> QueuedPlayerMovements;	// Buffer of un-acknowledged player movement packets.
	std::list<PKT_C2S_TankTurret> QueuedPlayerTurret;		// Buffer of un-acknowledged player turret packets.
	bool isMechanism[MCH_COUNT];

	Application();
	~Application() throw();

	void Start();
	bool Update();
	void Render();

	template <typename T1, typename T2>
	bool HasCollided( T1 &object, T2 &movable );

	template <typename Mov, typename Tgt>
	bool CheckCollision( Mov &object, Tgt &movable, float timedelta );

	void SetGameState( int gamestate ) { gamestate_ = gamestate; }
	int  GetGameState( void          ) { return gamestate_; }

	Tank& GetPlayer() { return player; }
	std::vector<Tank>& GetClients() { return clients; }
	std::unordered_map<int, Missile>& GetMissiles() { return missiles; }
	void ClearMissiles() { missiles.clear(); }
	void EmplaceMissile(Missile& missile) { missiles.emplace(missile.missile_id, missile); }
	Missile& GetMissile(const int& missile_id) { return missiles.find(missile_id)->second; }
	bool IsExistingMissile(const int& missile_id)
	{
		if (missiles.find(missile_id) != missiles.end())
			return true;
		else
			return false;
	}
};

#endif
