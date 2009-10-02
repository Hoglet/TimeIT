<?xml version="1.0"?>
<anjuta>
    <plugin name="Macro Plugin" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-macro:MacroPlugin"/>
    </plugin>
    <plugin name="Valgrind Plugin" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-valgrind:AnjutaValgrindPlugin"/>
    </plugin>
    <plugin name="Profiler" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-profiler:Profiler"/>
    </plugin>
    <plugin name="Terminal" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-terminal:TerminalPlugin"/>
    </plugin>
    <plugin name="Git" mandatory="no">
        <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-git:Git"/>
    </plugin>
</anjuta>
