# Damned
A repository for updating a game on Steam called Damned.


# What is this repository?



This repository is used to keep track of changes that the community has made to the game.
Tools will be written to streamline the process of adding in patches that contain new maps and balance changes into the game.



For more information, see this post on the Steam community forums:


https://steamcommunity.com/app/251170/discussions/0/1700542968204133958/


Do not download the zip file on the steam post because the latest changes will always be here. You can download the zip file from here.

# Communication

We communicate on Discord. The link is https://discord.gg/hEKaz5m

# TODO:
 
Here is a list of things that we have in mind to create so far:
	- Create a GUI that acts like a steam workshop for browsing and downloading maps and installing them into the game.
	- Figure out how to import new objects into the game.
	- Write a bot that can fetch public lobbies and post them into the Discord. This would be better than launching the game and seeing if there are any public lobbies.

	
	
	
	
	
	
** # V1.0 CHANGELOG: **


General

- Added a new Community-made Map 'Pog Champ Hotel'

- Added a new Community-made Map 'Hund Hills Community Center'

- Debug Console now displays a list of Maps and their internal ID



Fallen

- Traps needed for Seeker Form (P2) = 10, instead of random between 9 and 12

- Seeker (P2) starts transforming to Destroyer (P3) when a Survivor is within 6m line-of-sight, instead of previous 10m

- Seeker Form (P2) can now see Humans

- Destroyer Form (P3) now lasts for 30s, up from 7s

- While in Destroyer Form, press E button to phase out of Destroyer into Trapper Form and revert back into Destroyer after 3s assuming your 30s Form Time didn't elapse yet.

This ability has a cooldown of 5s (8s if you count the 3s of phasing time).



Phantom

- Changed background music when you are survivor and Phantom sees you from 'Run!' (monster_closer) to 'Kill!' (blind_hearing)


-----------------------------------------------------------------------------------------------------------------------------------------------------


# V1.01 CHANGELOG:




**GENERAL**

- Hosting a Lobby with Community Patch will now add a special NameTag to the Lobby's Name

- Added more variety to "Fake Trap Hallucination" Insanity Effect

- Soundfile monster_around.ogg modified for clarity purposes


☼ Hund Hills Community Center ☼
- Added 3 new splotlights to the Game Room on the second floor.





**BALANCE CHANGES**

Lurker

- monster_close.ogg is now Global




Phantom

- New music plays (monster_around.ogg) when the Phantom is within 16m of the Player

- blind_hearing.ogg modified, smoothened looping and added an ease-in at the beginning



Fallen:

- Phasing Time now 4s (was 3s)

- Max needed traps for Hunting form (Phase 2) now 16 (was 10)

- Max possible Statues is now 8 (16 on Woods) (was 5, 10 on Woods)

- Trapper form (Phase 1) Movement Speed Multiplier now 1.65 (was 1.0)




☆ Insanity System ☆

- Clicking on closed Objects(Doors) no longer increases Insanity




☼ Hund Hills Community Center changes ☼

- Added 3 new Mary spawnpoints into Asy_lob (white asylum lobby)

- Added 1 new Mary spawnpoint into the Bar

- Moved Mary spawnpoint from room 104 into the main hallway upstairs

- Removed both the Mary spawn in the cabin, in the barn, and added a replacement outside.



**BUGFIXES**

☆ Bodom Hotel ☆
- Fixed 1 Key Sequence that made the Map unsolvable unless you had people spawn on the other side of the map



☆ Rose Crimson Hotel ☆
- Unstuck the little cupboard door in the Spawn Room. Also fixes a Key Sequence that was previously unsolvable
- Fixed a single door leading out of the spawn room due to the possibility of it becoming stuck

☆ Hund Hills Community Center ☆
- Adjusted boxes in outdoor area on the Y axis to properly be seated on the ground
- Adjusted various boxes and picnic tables on the X axis so that players can properly fit through them
- Adjusted various chairs and tables in the bar on the X axis so that players can properly fit through them

