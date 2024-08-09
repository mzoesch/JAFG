# C# Scripting

To get this plugin's C# files to show up in the Visual Studio project, use the `-dotnet` argument when generating
project files.
You must use the command line approach to use this argument.

Full example command. Replace the paths with your own paths. Only for Windows CMD
(not PowerShell or Bash) as formatting is different.

```
"C:\UnrealPath\Engine\Build\BatchFiles\Build.bat" -projectfiles -project="C:\ProjectPath\JAFG.uproject" -game -rocket -progress -dotnet
```


# Disclaimer

This plugins C# files are not located here in the `Source` directory (Full Path: `.\Plugins\ModificationUBTEditorTool\Source\ModificationUBTEditorTool.Automation\`)
as they should be.


FAQ:


Q: Why? <br />
A: Apperently the [documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/create-an-automation-project-in-unreal-engine)
is kinda wrong, outdated or missing importend steps that are needed when working with a Unreal Project that is not
native but forgein to the Engine. So there is now no way to have the scripts here in the plugin subdirectory without
having to spend an absurd amount of time to figure out how to get this bascially unimportend feature to work.


Q: Where are they? <br />
A: They are now directly located in the forked [Unreal Engine repository](https://github.com/mzoesch/ue-rel) that is
also for JAFG. The scripts are temporarly located under the Automation Script Programs:

```
.\Engine\Source\Programs\AutomationTool\Scripts\
```


# The Scripts

Two scripts were created for this plugin. One for packaging a plugin
([PackagePlugin.cs](https://github.com/mzoesch/ue-rel/blob/release/Engine/Source/Programs/AutomationTool/Scripts/PackagePlugin.cs))
and one for launching the game, usually after a plugin was packaged and moved to a binary directory of JAFG
([LaunchGame.cs](https://github.com/mzoesch/ue-rel/blob/release/Engine/Source/Programs/AutomationTool/Scripts/LaunchGame.cs)).
