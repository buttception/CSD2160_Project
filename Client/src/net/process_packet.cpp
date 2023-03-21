#include <string>
#include <thread>

#include "ClientNetwork.h"
#include "application.h"
#include "packets\packets_s2c.h"
#include "process_packet.h"
#include "send_packet.h"
#include "movables\mine.h"
#include "movables\TimeBomb.h"
#include "movables\SpeedUp.h"


// NetLib Step 2. Client network object to use network library.
CatNet::ClientNetwork NetObj;

#ifdef _DEBUG
extern void log( char *szFormat, ... );
#endif


namespace Net {
    bool InitNetwork( void )
    {
        //return NetObj.InitNet( CatNet::APP_TYPE::APPTYPE_CLIENT, CatNet::PROTOCOL_TYPE::PROTOCOL_TCP, "127.0.0.1", 3456 );
		return NetObj.InitNet(2, 1, "127.0.0.1", 3456);
    }

    //-------------------------------------------------------------------------
    void ProcessPacket( Application *thisapp )
    {
        // Check any message from network! You can try to make this message processing with another thread but becareful with synchronization.
        // NetLib Step 6. Prepare the pointer of _ProcessSession and buffer structure of _PacketMessage.
        //         _ProcessSession pointer will give you the session information if there is any network communication from any client.
        //         _PacketMessage is for fetch the each of the actual data inside the packet buffer.
        struct CatNet::ProcessSession *ToProcessSessoin;
        // NetLib Step 7. Message Loop.
        //         Check any message from server and process.
        while( nullptr != ( ToProcessSessoin = NetObj.GetProcessList()->GetFirstSession() ) )
        { // Something recevied from network.
            int PacketID;
			CatNet::PacketMessage temp;
			
            ToProcessSessoin->m_PacketMessage >> PacketID;
            switch( thisapp->GetGameState() )
            {
                case GAMESTATE_INITIALIZING:
                {
                    switch( PacketID )
                    {
                        case PACKET_ID_S2C_WELCOMEMESSAGE:
                            WelcomeMessage( thisapp, ToProcessSessoin );
                            break;

                        case PACKET_ID_S2C_ENTERGAMEOK:
                            thisapp->SetGameState( GAMESTATE_INPLAY );
                            break;

						case PACKET_ID_S2C_FULLGAME:
							std::cout << "\nGame is full\n";
							break;
                    }
                }
                break;

                case GAMESTATE_INPLAY:
                {
                    switch( PacketID )
                    {
                        case PACKET_ID_S2C_ENEMYSHIP:
                        case PACKET_ID_S2C_NEWENEMYSHIP:
                            NewEnemyShip( thisapp, ToProcessSessoin );
                            break;

                        case PACKET_ID_S2C_NEWASTEROID:
                            NewAstreroid( thisapp, ToProcessSessoin );
                            break;

                        case PACKET_ID_S2C_DISCONNECTENEMYSHIP:
                            DisconnectEnemyShip( thisapp, ToProcessSessoin );
                            break;

                        case PACKET_ID_S2C_MOVEMENT:
                            ShipMovement( thisapp, ToProcessSessoin );
                            break;

                        case PACKET_ID_S2C_COLLIDED:
                            ShipCollided( thisapp, ToProcessSessoin );
                            break;

                        case PACKET_ID_S2C_ASTEROIDMOVEMENT:
                            AsteroidMovement( thisapp, ToProcessSessoin );
                            break;

						case PACKET_ID_S2C_ASTEROIDCOLLIDED:
							AsteroidCollided(thisapp, ToProcessSessoin);
							break;

						case PACKET_ID_S2C_NEWMISSILE:
							NewMissile(thisapp, ToProcessSessoin);
							break;
							
						case PACKET_ID_S2C_DELETEMISSILE:
							DeleteMissile(thisapp, ToProcessSessoin);
							break;

						case PACKET_ID_S2C_NEWBOOM:
							NewBoom(thisapp, ToProcessSessoin);
							break;

						case PACKET_ID_S2C_HITSHIP:
							HitShip(thisapp, ToProcessSessoin);
							break;

						case PACKET_ID_S2C_RESPAWN:
							Respawn(thisapp, ToProcessSessoin);
							break;
						
						case PACKET_ID_S2C_NEWMINE:
							NewMine(thisapp, ToProcessSessoin);
							break;

						case PACKET_ID_S2C_DELETEMINE:
							DeleteMine(thisapp, ToProcessSessoin);
							break;

						case PACKET_ID_S2C_NEWTIMEBOMB:
							NewTimeBomb(thisapp, ToProcessSessoin);
							break;

						case PACKET_ID_S2C_DELETETIMEBOMB:
							DeleteTimeBomb(thisapp, ToProcessSessoin);
							break;

						case PACKET_ID_S2C_NEWSPEEDUP:
							NewSpeedUp(thisapp, ToProcessSessoin);
							break;

						case PACKET_ID_S2C_DELETESPEEDUP:
							DeleteSpeedUp(thisapp, ToProcessSessoin);
							break;
                    }

                }
                break;

            }

            // Step 8. After finish the process with packet, You should delete the session message.
            //         If you forget to delete, it will cause memory leak!
            NetObj.GetProcessList()->DeleteFirstSession();
        }
    }

    //-------------------------------------------------------------------------
    void WelcomeMessage( Application *thisapp, struct CatNet::ProcessSession *ToProcessSessoin )
    {
        struct PKT_S2C_WelcomeMessage PacketData;
        ToProcessSessoin->m_PacketMessage >> PacketData;
        thisapp->GetMyShip()->SetShipID( PacketData.ShipID );
#ifdef _DEBUG
        log( "\nReceived: PACKET_ID_S2C_WELCOMEMESSAGE. ShipID:%d", PacketData.ShipID );
#endif
        // Send my spaceship info to server for synchronization.
        Net::send_packet_enter_game( thisapp->GetMyShip() );
    }

    //-------------------------------------------------------------------------
    void NewEnemyShip( Application *thisapp, struct CatNet::ProcessSession *ToProcessSessoin )
    {
        //struct PKT_S2C_EnemyShip EnemyshipPacketData;
        struct PKT_S2C_NewEnemyShip NewEnemyShipPacketData;

        ToProcessSessoin->m_PacketMessage >> NewEnemyShipPacketData;

        std::string EnemyShipName = "Enemy" + std::to_string( NewEnemyShipPacketData.ShipID );
        Ship *EnemyShip = new Ship( NewEnemyShipPacketData.ShipType, EnemyShipName, NewEnemyShipPacketData.x, NewEnemyShipPacketData.y );

        EnemyShip->SetShipID( NewEnemyShipPacketData.ShipID );
        EnemyShip->SetShipName( EnemyShipName );
        EnemyShip->SetShipType( NewEnemyShipPacketData.ShipType );
        EnemyShip->set_x( NewEnemyShipPacketData.x );
        EnemyShip->set_y( NewEnemyShipPacketData.y );
        EnemyShip->set_w( NewEnemyShipPacketData.w );
        EnemyShip->set_velocity_x( NewEnemyShipPacketData.velocity_x );
        EnemyShip->set_velocity_y( NewEnemyShipPacketData.velocity_y );
        EnemyShip->set_angular_velocity( NewEnemyShipPacketData.angular_velocity );

        thisapp->GetEnemyShipList()->push_back( EnemyShip );
#ifdef _DEBUG
        log( "\nReceived: PACKET_ID_S2C_ENEMYSHIP or PACKET_ID_S2C_NEWENEMYSHIP. ShipID:%d", EnemyShip->GetShipID() );
        log( "\n        - ShipName:%s, Type:%d, x:%0.0f, y:%0.0f, w:%0.0f",
             EnemyShip->GetShipName().c_str(), EnemyShip->GetShipType(), EnemyShip->get_x(), EnemyShip->get_y(), EnemyShip->get_w() );
        log( "\nNow I have: " );
        for( auto Enemy : *( thisapp->GetEnemyShipList() ) )
        {
            log( "%d ", Enemy->GetShipID() );
        }
#endif
    }

    //-------------------------------------------------------------------------
    void NewAstreroid( Application *thisapp, struct CatNet::ProcessSession *ToProcessSessoin )
    {
        struct PKT_S2C_NewAsteroid AsteroidData;
        ToProcessSessoin->m_PacketMessage >> AsteroidData;

#ifdef _DEBUG
        log( "\nReceived PACKET_ID_S2C_NEWASTEROID. ID:%d, x:%0.0f, y:%0.0f, vx:%0.0f, vy:%0.0f, av:%0.0f",
             AsteroidData.AsteroidID, AsteroidData.x, AsteroidData.y, AsteroidData.velocity_x, AsteroidData.velocity_y, AsteroidData.angular_velocity );
#endif

        Asteroid *asteroid = new Asteroid( "asteroid.png", AsteroidData.x, AsteroidData.y, AsteroidData.angular_velocity );
        asteroid->id_ = AsteroidData.AsteroidID;
        asteroid->set_velocity_x( AsteroidData.velocity_x );
        asteroid->set_velocity_y( AsteroidData.velocity_y );
        thisapp->GetAsteroidList()->push_back( asteroid );
    }

    //-------------------------------------------------------------------------
    void DisconnectEnemyShip( Application *thisapp, struct CatNet::ProcessSession *ToProcessSessoin )
    {
        struct PKT_S2C_EnemyShipDisconnect EnemyShipData;
        ToProcessSessoin->m_PacketMessage >> EnemyShipData;

        for( unsigned int i = 0; i < thisapp->GetEnemyShipList()->size(); ++i )
        {
            if( thisapp->GetEnemyShipList()->at( i )->GetShipID() == EnemyShipData.ShipID )
            {
                thisapp->GetEnemyShipList()->erase( thisapp->GetEnemyShipList()->begin() + i );

				// If there is exsiting missile with the ShipID, delete it.
				for (unsigned int i = 0; i < thisapp->GetEnemyMissileList()->size(); ++i)
				{
					if (thisapp->GetEnemyMissileList()->at(i)->get_ownerid() == EnemyShipData.ShipID)
					{
						thisapp->GetEnemyMissileList()->erase(thisapp->GetEnemyMissileList()->begin() + i);
						break;
					}
				}
                break;
            }
        }
#ifdef _DEBUG
        log( "\nReceived: PACKET_ID_S2C_DISCONNECT. ShipID: %d", EnemyShipData.ShipID );
        log( "\nNow I have: " );
        for( auto Enemy : *( thisapp->GetEnemyShipList() ) )
        {
            log( "%d ", Enemy->GetShipID() );
        }
#endif
    }

    //-------------------------------------------------------------------------
    void ShipMovement( Application *thisapp, struct CatNet::ProcessSession *ToProcessSessoin )
    {
        struct PKT_S2C_Movement MovementData;
        ToProcessSessoin->m_PacketMessage >> MovementData;
//#ifdef _DEBUG
//        log( "\nReceived: PACKET_ID_S2C_MOVEMENT. ShipID:%d, x:%0.0f, y:%0.0f, w:%0.0f, svx:%0.2f, svy:%0.2f, av:%0.0f",
//             MovementData.ShipID, MovementData.server_x, MovementData.server_y, MovementData.server_w,
//             MovementData.velocity_x, MovementData.velocity_y, MovementData.angular_velocity );
//#endif

        Ship *MoveShip;
        if( thisapp->GetMyShip()->GetShipID() == MovementData.ShipID )
        { // Update my ship.
//            MoveShip = thisapp->GetMyShip();
            return;
        }
        else
        { // Update enemyship.
            MoveShip = thisapp->FindEnemyShip( MovementData.ShipID );
        }
        MoveShip->set_server_x( MovementData.server_x );
        MoveShip->set_server_y( MovementData.server_y );
        MoveShip->set_server_w( MovementData.server_w );
        MoveShip->set_server_velocity_x( MovementData.velocity_x );
        MoveShip->set_server_velocity_y( MovementData.velocity_y );
        MoveShip->set_angular_velocity( MovementData.angular_velocity );
        MoveShip->do_interpolate_update();
    }

    //-------------------------------------------------------------------------
    void ShipCollided( Application *thisapp, struct CatNet::ProcessSession *ToProcessSessoin )
    {
        struct PKT_S2C_Collided CollidedData;
        ToProcessSessoin->m_PacketMessage >> CollidedData;

        Ship *CollidedShip;
        if( thisapp->GetMyShip()->GetShipID() == CollidedData.ShipID )
        { // Update my ship.
            CollidedShip = thisapp->GetMyShip();
        }
        else
        {
            CollidedShip = thisapp->FindEnemyShip( CollidedData.ShipID );
        }
        CollidedShip->set_server_x( CollidedData.server_x );
        CollidedShip->set_server_y( CollidedData.server_y );
        CollidedShip->set_server_w( CollidedData.server_w );
        CollidedShip->set_velocity_x( CollidedData.velocity_x );
        CollidedShip->set_velocity_y( CollidedData.velocity_y );
        CollidedShip->set_angular_velocity( CollidedData.angular_velocity );
    }

    //-------------------------------------------------------------------------
    void AsteroidMovement( Application *thisapp, struct CatNet::ProcessSession *ToProcessSessoin )
    {
        struct PKT_S2C_AsteroidMovement AsteroidMovementData;
        ToProcessSessoin->m_PacketMessage >> AsteroidMovementData;

        Asteroid *MovingAsteroid = nullptr;
        for( auto itr_asteroid : *( thisapp->GetAsteroidList() ) )
        {
            if( itr_asteroid->get_id() == AsteroidMovementData.AsteroidID )
            {
                MovingAsteroid = itr_asteroid;
            }
        }
        if( nullptr == MovingAsteroid )
        { // ID error. I cannot find this asteroid.
            return;
        }

        // Now, let's update the data.
        MovingAsteroid->set_server_x( AsteroidMovementData.server_x );
        MovingAsteroid->set_server_y( AsteroidMovementData.server_y );
        MovingAsteroid->set_server_velocity_x( AsteroidMovementData.velocity_x );
        MovingAsteroid->set_server_velocity_y( AsteroidMovementData.velocity_y );
        MovingAsteroid->set_angular_velocity( AsteroidMovementData.angular_velocity );
        MovingAsteroid->do_interpolate_update();
    }
	void AsteroidCollided(Application * thisapp, CatNet::ProcessSession * ToProcessSessoin)
	{
		struct PKT_S2C_AsteroidCollided AsteroidCollidedData;
		ToProcessSessoin->m_PacketMessage >> AsteroidCollidedData;
		Asteroid *CollidedAsteroid = NULL;
		for (auto itr_asteroid : *(thisapp->GetAsteroidList()))
		{
			if (itr_asteroid->get_id() == AsteroidCollidedData.AsteroidID)
			{
				CollidedAsteroid = itr_asteroid;
			}
		}
		if (NULL == CollidedAsteroid)
		{ // ID error. I cannot find this asteroid.
			return;
		}
		// Now, let's update the data.
		CollidedAsteroid->set_server_x(AsteroidCollidedData.server_x);
		CollidedAsteroid->set_server_y(AsteroidCollidedData.server_y);
		CollidedAsteroid->set_server_velocity_x(AsteroidCollidedData.velocity_x);
		CollidedAsteroid->set_server_velocity_y(AsteroidCollidedData.velocity_y);
		CollidedAsteroid->set_angular_velocity(AsteroidCollidedData.angular_velocity);
		CollidedAsteroid->do_interpolate_update();

	}
	void NewMissile(Application * thisapp, CatNet::ProcessSession * ToProcessSessoin)
	{
		struct PKT_S2C_NewMissile NewMissileData;
		ToProcessSessoin->m_PacketMessage >> NewMissileData;
		// If the missile is my own missile, then ignore the packet.
		if (thisapp->GetMyShip()->GetShipID() == NewMissileData.OwnerShipID) return;
		// If there is exsiting missile with the ShipID, delete it first.
		for (unsigned int i = 0; i < thisapp->GetEnemyMissileList()->size(); ++i)
		{
			if (thisapp->GetEnemyMissileList()->at(i)->get_ownerid() == NewMissileData.OwnerShipID)
			{
				thisapp->GetEnemyMissileList()->erase(thisapp->GetEnemyMissileList()->begin() + i);
				break;
			}
		}
		Missile *missile = new Missile("missile.png", NewMissileData.x, NewMissileData.y,
			NewMissileData.w, NewMissileData.OwnerShipID);
		missile->set_velocity_x(NewMissileData.velocity_x);
		missile->set_velocity_y(NewMissileData.velocity_y);
		missile->set_server_velocity_x(NewMissileData.velocity_x);
		missile->set_server_velocity_y(NewMissileData.velocity_y);
		thisapp->GetEnemyMissileList()->push_back(missile);
	}
	void DeleteMissile(Application * thisapp, CatNet::ProcessSession * ToProcessSession)
	{
		struct PKT_S2C_DeleteMissile DeleteMissileData;
		ToProcessSession->m_PacketMessage >> DeleteMissileData;
		//if it is my own missile, no need delete as i delete alr
		if (thisapp->GetMyShip()->GetShipID() == DeleteMissileData.OwnerShipID) return;
		//find the missile
		for (unsigned int i = 0; i < thisapp->GetEnemyMissileList()->size(); ++i)
		{
			if (thisapp->GetEnemyMissileList()->at(i)->get_ownerid() == DeleteMissileData.OwnerShipID)
			{
				//delete the missile
				delete thisapp->GetEnemyMissileList()->at(i);
				thisapp->GetEnemyMissileList()->erase(thisapp->GetEnemyMissileList()->begin() + i);
				break;
			}
		}
	}
	void NewBoom(Application * thisapp, CatNet::ProcessSession * ToProcessSession)
	{
		struct PKT_S2C_NewBoom NewBoomData;
		ToProcessSession->m_PacketMessage >> NewBoomData;
		//create the boom
		Boom* boom = new Boom("boom.png", NewBoomData.x, NewBoomData.y, NewBoomData.w);
		thisapp->GetParticleList()->push_back(boom);
	}
	void HitShip(Application * thisapp, CatNet::ProcessSession * ToProcessSession)
	{
		struct PKT_S2C_HitShip HitData;
		ToProcessSession->m_PacketMessage >> HitData;
		//set the ship it hit to inactive
		for (auto it : *thisapp->GetEnemyShipList())
		{
			if (it->GetShipID() == HitData.ShipHitID)
				it->active = false;
		}
		if (thisapp->GetMyShip()->GetShipID() == HitData.ShipHitID)
			thisapp->GetMyShip()->active = false;
	}
	void Respawn(Application * thisapp, CatNet::ProcessSession * ToProcessSession)
	{
		struct PKT_S2C_Respawn ShipData;
		ToProcessSession->m_PacketMessage >> ShipData;
		if (thisapp->GetMyShip()->GetShipID() == ShipData.ShipID)
			return;
		//find the ship to respawn
		for (auto it : *thisapp->GetEnemyShipList())
		{
			if (it->GetShipID() == ShipData.ShipID)
				it->active = true;
		}
	}
	void NewMine(Application * thisapp, CatNet::ProcessSession * ToPrcoessSession)
	{
		struct PKT_S2C_NewMine MineData;
		ToPrcoessSession->m_PacketMessage >> MineData;
		if (thisapp->GetMyShip()->GetShipID() == MineData.OwnerID)
			return;
		//check mine exist
		for (std::vector<Mine*>::iterator it = thisapp->GetEnemyMineList()->begin(); it != thisapp->GetEnemyMineList()->end(); ++it)
		{
			if ((*it)->ownerid_ == MineData.OwnerID)
			{
				Mine* temp = (*it);
				thisapp->GetEnemyMineList()->erase(it);
				delete temp;
				break;
			}
		}
		//create the thing
		thisapp->GetEnemyMineList()->push_back(new Mine("mine.png", MineData.x, MineData.y, MineData.OwnerID));
	}
	void DeleteMine(Application * thisapp, CatNet::ProcessSession * ToProcessSession)
	{
		struct PKT_S2C_DeleteMine mineData;
		ToProcessSession->m_PacketMessage >> mineData;
		for (std::vector<Mine*>::iterator it = thisapp->GetEnemyMineList()->begin(); it != thisapp->GetEnemyMineList()->end(); ++it)
		{
			if ((*it)->ownerid_ == mineData.ownerId)
			{
				Mine* temp = (*it);
				thisapp->GetEnemyMineList()->erase(it);
				delete temp;
				temp = NULL;
				return;
			}
		}
	}
	void NewTimeBomb(Application * thisapp, CatNet::ProcessSession * ToProcessSession)
	{
		struct PKT_S2C_NewTimeBomb bombData;
		ToProcessSession->m_PacketMessage >> bombData;

		if (thisapp->GetMyShip()->GetShipID() == bombData.ownerId)
			return;

		for (std::vector<TimeBomb*>::iterator it = thisapp->GetEnemyBombList()->begin(); it != thisapp->GetEnemyBombList()->end(); ++it)
		{
			if ((*it)->ownerid_ == bombData.ownerId)
			{
				TimeBomb* temp = (*it);
				thisapp->GetEnemyBombList()->erase(it);
				delete temp;
				break;
			}
		}
		TimeBomb* newBomb = new TimeBomb("timebomb.png", bombData.x, bombData.y, 0);
		newBomb->ownerid_ = bombData.ownerId;
		thisapp->GetEnemyBombList()->push_back(newBomb);
	}
	void DeleteTimeBomb(Application * thisapp, CatNet::ProcessSession * ToProcessSession)
	{
		struct PKT_S2C_DeleteTimeBomb bombData;
		ToProcessSession->m_PacketMessage >> bombData;
		for (std::vector<TimeBomb*>::iterator it = thisapp->GetEnemyBombList()->begin(); it != thisapp->GetEnemyBombList()->end(); ++it)
		{
			if ((*it)->ownerid_ == bombData.ownerId)
			{
				TimeBomb* temp = (*it);
				thisapp->GetEnemyBombList()->erase(it);
				delete temp;
				temp = NULL;
				return;
			}
		}
	}
	void NewSpeedUp(Application * thisapp, CatNet::ProcessSession * ToProcessSession)
	{
		struct PKT_S2C_NewSpeedUp speedupdata;
		ToProcessSession->m_PacketMessage >> speedupdata;
		//spawn pick up
		for (auto it : *thisapp->GetSpeedUps())
		{
			if (it->id == speedupdata.SpeedUpID)
				return;
		}
		SpeedUp* newSU = new SpeedUp("speedup.png", speedupdata.server_x, speedupdata.server_y, 0);
		newSU->id = speedupdata.SpeedUpID;
		thisapp->GetSpeedUps()->push_back(newSU);
		printf("\npower up packet was received");
	}
	void DeleteSpeedUp(Application * thisapp, CatNet::ProcessSession * ToProcessSession)
	{
		struct PKT_S2C_DeleteSpeedUp speedupdata;
		ToProcessSession->m_PacketMessage >> speedupdata;
		for (std::vector<SpeedUp*>::iterator it = thisapp->GetSpeedUps()->begin(); it != thisapp->GetSpeedUps()->end(); ++it)
		{
			SpeedUp* temp = (*it);
			if (temp->id == speedupdata.SpeedUpID)
			{
				delete temp;
				temp = NULL;
				thisapp->GetSpeedUps()->erase(it);
				break;
				printf("\nPower up was deleted");
			}
		}
	}
}

