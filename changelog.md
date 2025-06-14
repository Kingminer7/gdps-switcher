# 1.4.2
### Some minor fixes
- <cy>Fix saving from GDPS Hub and a deletion issue (found by tt_kybersport)</c>
- <cy>Make deleting a server delete the save file</c>
# 1.4.1
### API Fix
- <cy>Fix API for mods like GDPS Hub (found by tt_kybersport)</c>
# 1.4.0
### Major changes
- <cg>Added regex to urls</c>
- <cy>Redo the UI code</c>
- <cg>Some minor server customization options. Currently only MOTDs and logos are possible.</c>
- <cr>Remove URI support due to issues and complications</c>
- <cb>Secrets!</c>

# 1.3.0
### Add URI support
- <cg>Added URI Handling if you have [Geode URI Scheme](mod:camila314.geode-uri) by Camila314 (usage: `geode://gdps/add/[url];[name]`)</c>

# 1.2.8
### Fix more save issues
- <cy>Fix a save issue with certain values set in the save file (found by [RighteousRyan](https://github.com/RighteousRyan1))</c>

# 1.2.7
### CORRECTLY fix patched exes
- <cy>Previous change except without the save file issues</c>

# 1.2.6
### Patched EXE fix
- <cy>Fix server automatically setting to RobTop's when on a url patched game</c>

# 1.2.5
### Small compatibility changes
- <cg>Add api</c>
- <cg>Disable while certain mods are enabled, **most notably fangame mods**</c>

# 1.2.4
### Update for Server API Update
- <cy>Internal changes for Server API's update</c>

# 1.2.3
### Update for Geode 4.0.0-beta.1
- <cy>Build for mac again</c>
- <cy>Geode 4.0.0-beta.1</c>

# 1.2.2
### Update for 2.2074
- <cy>Update to Geode 4.0.0-alpha.1 and GD 2.2074</c>

# 1.2.1
### Fix save data issues
- <cg>Create a popup for save recovery - shows on servers with potentially lost data.</c>
  - <cy>This does not merge data. It restores it.</c>
- <cy>Delete the music library and SFX library on server switch to fix issues</c>

# 1.2.0
### Internal Changes & bug fixes
- <cy>Move actual request handling to another mod (km7dev.server_api)</c>
- <cr>Remove geode::web hooking (May cause new/returning problems with other mods)</c>
- <cy>Fix some UI bugs and minorly improve some elements</c>

# 1.1.4
### Fix a Name (sorry!)
- <cy>Fix SorkoPiko's name (sorry about that!)</c>

# 1.1.3
### Mark No Rate limit incompatible
- <cy>SorkoPiko's No Rate Limit causes a crash on startup. I've marked it as incompatible in this important update, so if possible, please update.</c>

# 1.1.2
### Fixes and Event Levels
- <cy>Fix some other mods not connecting to private servers for stats (mods like Demons in Garage and Garage Plus)</c>
- <cg>Event Levels (thank you Uproxide and TheSilyDoggo)</c>
- <cy>Fix crash when importing invalid files</c>
- <cr>Temporarily remove Globed warning</c>
- <cy>Changed create server popup slightly</c>

# 1.1.1
### Bug Fix
- <cg>Add warning about globed not working</c>
- <cy>Change credits slightly to properly credit Brift</c>
- <cy>Fix server lists not properly exporting as json</c>
- <cy>Fix the Apply button on some aspect ratios</c>
- <cg>Pin GD servers to the list</c>
- <cg>Add warning about only 2.2 servers to add server prompt.</c>

# 1.1.0
### Overhaul
- <cy>Redo the UI</c>
- <cg>Importing/Exporting</c>

# 1.0.1
### Bug Fix
- <cy>Fix mac building</c>

# 1.0.0
### Full Release
- <cg>Add switching servers</c>
- <cg>Windows/Android/Mac support</c>