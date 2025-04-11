# 1.4.0
### Major changes
- Added regex to urls (TODO REIMPL)
- Redo the UI code (TODO)
- Server integration support (TODO)
- Clean up includes (TODO) (REMOVE THIS)
- Reimpl URI support (TODO) (REMOVE THIS)
- Migrate to Prevter's label (TODO) (REMOVE THIS)
- Bonus settings (TODO) (REMOVE THIS)
# 1.3.0
### Add URI support
- Added URI Handling if you have [Geode URI Scheme](mod:camila314.geode-uri) by Camila314 (usage: `geode://gdps/add/[url];[name]`)
# 1.2.8
### Fix more save issues
- Fix a save issue with certain values set in the save file (found by [RighteousRyan](https://github.com/RighteousRyan1))
# 1.2.7
### CORRECTLY fix patched exes
- Previous change except without the save file issues
# 1.2.6
### Patched EXE fix
- Fix server automatically setting to RobTop's when on a url patched game
# 1.2.5
### Small compatibility changes
- Add api
- Disable while certain mods are enabled, **most notably fangame mods**
# 1.2.4
### Update for Server API Update
- Internal changes for Server API's update
# 1.2.3
### Update for Geode 4.0.0-beta.1
- Build for mac again
- Geode 4.0.0-beta.1
# 1.2.2
### Update for 2.2074
- Update to Geode 4.0.0-alpha.1 and GD 2.2074
# 1.2.1
### Fix save data issues
- Create a popup for save recovery - shows on servers with potentially lost data.
  - This does not merge data. It restores it.
- Delete the music library and SFX library on server switch to fix issues

# 1.2.0
### Internal Changes & bug fixes
- Move actual request handling to another mod (km7dev.server_api)
- Remove geode::web hooking (May cause new/returning problems with other mods)
- Fix some UI bugs and minorly improve some elements
# 1.1.4
### Fix a Name (sorry!)
- fix SorkoPiko's name (sorry about that!)
# 1.1.3
## IMPORTANT
### Mark "sorkopiko.noratelimit" incompatible
SorkoPiko's No Rate Limit causes a crash on startup. I've marked it as incompatible in this important update, so if possible, please update.
# 1.1.2
### Fixes and Event Levels
- Fix some other mods not connecting to private servers for stats (mods like Demons in Garage and Garage Plus),
- Event Levels (thank you Uproxide and TheSilyDoggo)
- Fix crash when importing invalid files
- Temporarily remove Globed warning
- Changed create server popup slightly
# 1.1.1
### Bug Fix
- Add warning about globed not working
- Change credits slightly to properly credit Brift
- Fix server lists not properly exporting as json
- Fix the Apply button on some aspect ratios
- Pin GD servers to the list
- Add warning about only 2.2 servers to add server prompt.
# 1.1.0
### Overhaul
- Redo the UI
- Importing/Exporting
# 1.0.1
### Bug Fix
- Fix mac building
# 1.0.0
### Full Release
- Add switching servers
- Windows/Android/Mac support
