# V1.03 CHANGELOG:





**General**
- Disabled the use of the Escape key in lobby as it was the culprit of bugging out lobbies so that you may no longer click anything.








**BALANCE CHANGES**

- Added more trap spawns to the maps listed below:

- Black Lake Asylum
- Black Lake Woods
- Bright Hotel
- Factory
- Tyberg Hospital
- Serenity Asylum


*WAREHOUSE*

- This map originally had a poor distribution of traps (some traps being grouped up in clusters of up to 8 at a time) and as such been moderately reworked. Information below:

8 clocks replaced with non-trap clock variants (in the clock room)
5 timers removed, 4 traps added (in the oven timer/shelf room, AKA spawn point)
10 pianos replaced with 10 non-trap piano variants, 3 traps added (in the room with multiple safes stacked as a tower)
12 traps added to empty room (room before the clock room, which originally had 2 traps)
trap net change: -3<<



*TRAP CHANGES*
- Percentage modifier (percent of traps that activate on a map per shift) has been modified on all maps as a result of adding more traps, or not having enough.







**BUGFIXES**


☆ PogChamp Hotel ☆

- Corrected some spawnpoints
- Fixed issue with doorhandles clipping through wooden planks on doors



☆ Hund Hills Community Center (HHCC) ☆

- Covered staircase meshhole



☆ BRIGHT HOTEL ☆

- Adjusted the position of tables and chairs on the X-axis in the diner so that they no longer fatshame players. (impede movement from being too close)



------------------------------------------------------------------------------------------------------------------------------------------------------------------------------




# V1.02 CHANGELOG:





**GENERAL**

- Changed the menu background back to what it was on patch 1.00 (was the original damned menu)





**BUGFIXES**


☆ Hund Hills Community Center ☆

- Removed various test objects from the map
- Added boards at gameroom doubledoors to cover meshholes
- Adjusted projector objects to be properly aligned
- Fixed issue on 2nd floor where a table was not able to be set as a trap
- Miniature makeshift passage added between staircase and exit room so that lurker can properly fit through the doorframe
- Adjusted spawn points for both survivor and monster so they do not fall through the map when host starts game before players are fully loaded





----------------------------------------------------------------------------------------------------------------------------------------------------------------------------



# V1.01 CHANGELOG:







**GENERAL**

- Hosting a Lobby with Community Patch will now add a special NameTag to the Lobby's Name

- Added more variety to "Fake Trap Hallucination" Insanity Effect

- Soundfile monster_around.ogg modified for clarity purposes

- blind_hearing.ogg modified, smoothened looping and added an ease-in at the beginning



☼ Hund Hills Community Center ☼
- Added 3 new spotlights to the Game Room on the second floor.







**BALANCE CHANGES**

Lurker

- monster_close.ogg is now Global



Phantom

- New music plays (monster_around.ogg) when the Phantom is within 16m of the Player



Fallen:

- Phasing Time now 4s (was 3s)

- Max needed traps for Hunting form (Phase 2) now 16 (was 10)

- Max possible Statues is now 8 (16 on Woods) (was 5, 10 on Woods)

- Trapper form (Phase 1) Movement Speed Multiplier now 1.65 (was 1.0)

- Added trap progress counter in top left of screen




☆ Insanity System ☆

- Clicking on locked doors no longer increases Insanity


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





----------------------------------------------------------------------------------------------------------------------------------------------------------------------------




# V1.00 CHANGELOG:


**General**
- Added a new Community-made Map 'Pog Champ Hotel'
- Added a new Community-made Map 'Hund Hills Community Center'
- Debug Console now displays a list of Maps and their internal ID
- Changed background music when you are survivor and Phantom sees you from 'Run!' (monster_closer) to 'Kill!' (blind_hearing)




**BALANCE CHANGES**

Fallen
- Traps needed for Seeker Form (P2) = 10, instead of random between 9 and 12
- Seeker (P2) starts transforming to Destroyer (P3) when a Survivor is within 6m line-of-sight, instead of previous 10m
- Seeker Form (P2) can now see Humans
- Destroyer Form (P3) now lasts for 30s, up from 7s
- While in Destroyer Form, press E button to phase out of Destroyer into Trapper Form and revert back into Destroyer after 3s assuming your 30s Form Time didn't elapse yet.
This ability has a cooldown of 5s (8s if you count the 3s of phasing time).
