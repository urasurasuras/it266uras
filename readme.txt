Uras Oran
IT266

Project description:
	This mod features enemies that drop random items from a list 
	(List includes: "weapon_shotgun", "weapon_rocketlauncher", "weapon_machinegun", "item_health_small"
	Random weapons that drop have random stats that show up on the objective tab once you pick them up.
	
Testable deliverables:
	Random weapon drops
	Random weapon stats
	
Instructions:
	Shortcut properties* will skip all intro cinematics, and start the game in the test map
	If not, use "map tools/mv2" in the console for the test map
	Open console with (crtl + alt + ~)
	Spawn command to spawn enemies (example:  spawn enemy_strogg_marine)
	Enemies will drop a random item that can be picked up by the player 
	Once the player pickes a weapon up, they will have initialized their random stats for that weapon
	Alternatively you can give weapons to players (example: give weapon_machinegun)
	Random stats will be shown on the objective tab by holding TAB key to display
	(The same stats will also be displayed on the console)
	Random stats for each weapon will be initialized on the weapon spawn
	If you pick up the same weapon while you already have it, you will only get additional ammo
	You cannot respawn a weapon unless you use all the ammo for that weapon
	(The blaster also has a random spread stat that only shows up in the console, and it will respawn if you switch back to it)
	
Shortcut properties:
	+set fs_game umDoomify 			(Launch mod)
	+disconnect 					(Skip intros)
	+set com_allowconsole 1 		(Allow console to open only using 
	+map tools/mv2.map				(Launches test map)