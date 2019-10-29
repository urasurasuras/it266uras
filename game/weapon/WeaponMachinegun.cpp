#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"
#include "../Weapon.h"

class rvWeaponMachinegun : public rvWeapon {
public:

	////////////////////////
	/*
	rvWeapon* A;
	rvWeaponMachinegun(){
	}*/
	CLASS_PROTOTYPE( rvWeaponMachinegun );

	rvWeaponMachinegun ( void );

	virtual void		Spawn(void);
	virtual void		Think				( void );
	void				Save				( idSaveGame *savefile ) const;
	void				Restore				( idRestoreGame *savefile );
	void					PreSave				( void );
	void					PostSave			( void );

	

protected:

	float				spreadZoom;
	bool				fireHeld;

	bool				UpdateFlashlight	( void );
	void				Flashlight			( bool on );

private:
	int randFireRate;
	int randAltFireRate;

	stateResult_t		State_Idle			( const stateParms_t& parms );
	stateResult_t		State_Fire			( const stateParms_t& parms );
	stateResult_t		State_Reload		( const stateParms_t& parms );
	stateResult_t		State_Flashlight	( const stateParms_t& parms );

	CLASS_STATES_PROTOTYPE ( rvWeaponMachinegun );
};

CLASS_DECLARATION( rvWeapon, rvWeaponMachinegun )
END_CLASS

/*
================
rvWeaponMachinegun::rvWeaponMachinegun
================
*/
rvWeaponMachinegun::rvWeaponMachinegun ( void ) {
}

/*
================
rvWeaponMachinegun::Spawn
================
*/
void rvWeaponMachinegun::Spawn(void) {
	//idUserInterface * hud = idPlayer::hud;
	//ui->SetStateInt("player_health", 10);
	
	//int randFireRates[4][20] = { 1, 10, 50, 100 };
	//int RandIndex = gameLocal.random.RandomInt();


	//initialize random seed
	gameLocal.random.SetSeed(gameLocal.time);
	gameLocal.random.SetSeed(gameLocal.random.RandomInt());

	fireRate = gameLocal.random.RandomInt(100);
	spread = gameLocal.random.RandomInt(20);
	spreadZoom = gameLocal.random.RandomInt(5);
	

	gameLocal.Printf("Spread: %.1f\n", spread);
	gameLocal.Printf("fireRate: %d\n", fireRate);
	//gameLocal.Printf("altFireRate: %.1f\n", randAltFireRate);
	gameLocal.Printf("spreadZoom: %.1f\n", spreadZoom);
	//_hud->SetStateInt("player_health", 50);
	
	idPlayer *player = gameLocal.GetLocalPlayer();
	player->GiveObjective("Spread: ", spread);
	player->passedVar = &fireRate;

	fireHeld   = false;
		
	SetState ( "Raise", 0 );	
	
	Flashlight ( owner->IsFlashlightOn() );
}

/*
================
rvWeaponMachinegun::Save
================
*/
void rvWeaponMachinegun::Save ( idSaveGame *savefile ) const {
	savefile->WriteFloat ( spreadZoom );
	savefile->WriteBool ( fireHeld );
}

/*
================
rvWeaponMachinegun::Restore
================
*/
void rvWeaponMachinegun::Restore ( idRestoreGame *savefile ) {
	savefile->ReadFloat ( spreadZoom );
	savefile->ReadBool ( fireHeld );
}

/*
================
rvWeaponMachinegun::PreSave
================
*/
void rvWeaponMachinegun::PreSave ( void ) {
}

/*
================
rvWeaponMachinegun::PostSave
================
*/
void rvWeaponMachinegun::PostSave ( void ) {
}


/*
================
rvWeaponMachinegun::Think
================
*/
void rvWeaponMachinegun::Think()
{
	rvWeapon::Think();
	if ( zoomGui && owner == gameLocal.GetLocalPlayer( ) ) {
		zoomGui->SetStateFloat( "playerYaw", playerViewAxis.ToAngles().yaw );
	}
}

/*
================
rvWeaponMachinegun::UpdateFlashlight
================
*/
bool rvWeaponMachinegun::UpdateFlashlight ( void ) {
	if ( !wsfl.flashlight ) {
		return false;
	}
	
	SetState ( "Flashlight", 0 );
	return true;		
}

/*
================
rvWeaponMachinegun::Flashlight
================
*/
void rvWeaponMachinegun::Flashlight ( bool on ) {
	owner->Flashlight ( on );
	
	if ( on ) {
		viewModel->ShowSurface ( "models/weapons/blaster/flare" );
		worldModel->ShowSurface ( "models/weapons/blaster/flare" );
	} else {
		viewModel->HideSurface ( "models/weapons/blaster/flare" );
		worldModel->HideSurface ( "models/weapons/blaster/flare" );
	}
}

/*
===============================================================================

	States 

===============================================================================
*/

CLASS_STATES_DECLARATION ( rvWeaponMachinegun )
	STATE ( "Idle",				rvWeaponMachinegun::State_Idle)
	STATE ( "Fire",				rvWeaponMachinegun::State_Fire )
	STATE ( "Reload",			rvWeaponMachinegun::State_Reload )
	STATE ( "Flashlight",		rvWeaponMachinegun::State_Flashlight )
END_CLASS_STATES

/*
================
rvWeaponMachinegun::State_Idle
================
*/
stateResult_t rvWeaponMachinegun::State_Idle( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};	
	switch ( parms.stage ) {
		case STAGE_INIT:
			if ( !AmmoAvailable ( ) ) {
				SetStatus ( WP_OUTOFAMMO );
			} else {
				SetStatus ( WP_READY );
			}
		
			PlayCycle( ANIMCHANNEL_ALL, "idle", parms.blendFrames );
			return SRESULT_STAGE ( STAGE_WAIT );
		
		case STAGE_WAIT:			
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}		
			if ( UpdateFlashlight ( ) ) {
				return SRESULT_DONE;
			}

			if ( fireHeld && !wsfl.attack ) {
				fireHeld = false;
			}
			if ( !clipSize ) {
				if ( !fireHeld && gameLocal.time > nextAttackTime && wsfl.attack && AmmoAvailable ( ) ) {
					SetState ( "Fire", 0 );
					return SRESULT_DONE;
				}
			} else {
				if ( !fireHeld && gameLocal.time > nextAttackTime && wsfl.attack && AmmoInClip ( ) ) {
					SetState ( "Fire", 0 );
					return SRESULT_DONE;
				}  
				if ( wsfl.attack && AutoReload() && !AmmoInClip ( ) && AmmoAvailable () ) {
					SetState ( "Reload", 4 );
					return SRESULT_DONE;			
				}
				if ( wsfl.netReload || (wsfl.reload && AmmoInClip() < ClipSize() && AmmoAvailable()>AmmoInClip()) ) {
					SetState ( "Reload", 4 );
					return SRESULT_DONE;			
				}				
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponMachinegun::State_Fire
================
*/
stateResult_t rvWeaponMachinegun::State_Fire ( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};	
	switch ( parms.stage ) {
		case STAGE_INIT:
			if ( wsfl.zoom ) {
				nextAttackTime = gameLocal.time + (/*randAltFireRate**/fireRate * owner->PowerUpModifier ( PMOD_FIRERATE ));
				Attack ( true, 1, spreadZoom, 0, 1.0f );
				fireHeld = true;
			} else {
				nextAttackTime = gameLocal.time + (/*randFireRate**/fireRate * owner->PowerUpModifier ( PMOD_FIRERATE ));
				Attack ( false, 1, spread, 0, 1.0f );
			}
			PlayAnim ( ANIMCHANNEL_ALL, "fire", 0 );	
			return SRESULT_STAGE ( STAGE_WAIT );
	
		case STAGE_WAIT:		
			if ( !fireHeld && wsfl.attack && gameLocal.time >= nextAttackTime && AmmoInClip() && !wsfl.lowerWeapon ) {
				SetState ( "Fire", 0 );
				return SRESULT_DONE;
			}
			if ( AnimDone ( ANIMCHANNEL_ALL, 0 ) ) {
				SetState ( "Idle", 0 );
				return SRESULT_DONE;
			}		
			if ( UpdateFlashlight ( ) ) {
				return SRESULT_DONE;
			}			
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponMachinegun::State_Reload
================
*/
stateResult_t rvWeaponMachinegun::State_Reload ( const stateParms_t& parms ) {
	enum {
		STAGE_INIT,
		STAGE_WAIT,
	};	
	switch ( parms.stage ) {
		case STAGE_INIT:
			if ( wsfl.netReload ) {
				wsfl.netReload = false;
			} else {
				NetReload ( );
			}
			
			SetStatus ( WP_RELOAD );
			PlayAnim ( ANIMCHANNEL_ALL, "reload", parms.blendFrames );
			return SRESULT_STAGE ( STAGE_WAIT );
			
		case STAGE_WAIT:
			if ( AnimDone ( ANIMCHANNEL_ALL, 4 ) ) {
				AddToClip ( ClipSize() );
				SetState ( "Idle", 4 );
				return SRESULT_DONE;
			}
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}
			

/*
================
rvWeaponMachinegun::State_Flashlight
================
*/
stateResult_t rvWeaponMachinegun::State_Flashlight ( const stateParms_t& parms ) {
	enum {
		FLASHLIGHT_INIT,
		FLASHLIGHT_WAIT,
	};	
	switch ( parms.stage ) {
		case FLASHLIGHT_INIT:			
			SetStatus ( WP_FLASHLIGHT );
			// Wait for the flashlight anim to play		
			PlayAnim( ANIMCHANNEL_ALL, "flashlight", 0 );
			return SRESULT_STAGE ( FLASHLIGHT_WAIT );
			
		case FLASHLIGHT_WAIT:
			if ( !AnimDone ( ANIMCHANNEL_ALL, 4 ) ) {
				return SRESULT_WAIT;
			}
			
			if ( owner->IsFlashlightOn() ) {
				Flashlight ( false );
			} else {
				Flashlight ( true );
			}
			
			SetState ( "Idle", 4 );
			return SRESULT_DONE;
	}
	return SRESULT_ERROR;
}
