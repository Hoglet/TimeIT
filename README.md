


# TimeIT
TimeIT is a time tracker that works quietly with minimal/none interaction while 
registering the time you are spending on multiple projects.  
![Screenshot](src/doc/html/C/TimeIT-MainWindow.png)

It features:
* Manual time tracking.
* Automatic time tracking (through workspace tracking). 
* Multiple timers running at the same time. Example "I am at work" and "I work with X"   
* Idle detection.
* Unobtrusive
  * Runs in background.
  * Status Icon in the system tray.
* Distributed database
  * Synchronization with server See: [TimeIT-server](https://github.com/Hoglet/TimeIT-Server)
  * Run on several computers and see the times distributed and visible on all.
* Summary views grouped by day, week, month, and year.
* Show details of a task by day/week/month/year. 
  * When did you work?
  * When did you have a break and for how long?
* Editing of records.

Source is available on [GitHub](https://github.com/Hoglet/TimeIT)

Project homepage is on [GitHub pages](https://hoglet.github.io/TimeIT/)

## Build

NOTE: Running the application without having installed it will fail as it can not load resources.

### Prerequisites
1. Install cmake and prefered compiler
2. Install needed libaries:
   
Debian example:

        $ sudo apt install -y   \
               libgtkmm-3.0-dev \
               libxss-dev       \
               libsqlite3-dev   \
               gtkmm-3.0-dev    \
               libjansson-dev   \
               libcurl4-openssl-dev \
               libx11-dev       \
               gettext          \
               libfmt-dev       \
               libnotify-dev    \
               libgtest-dev  

   
### Generic
Build:

        $ cmake --preset ninja-multi
        $ cmake --build --preset ninja-multi-release

Install:

        $ cmake --build --preset ninja-multi-release --target install

### Making a debian package:
        $ debian/rules clean  
        $ debian/rules build  
        $ fakeroot debian/rules binary
deb file will be in parent directory.
Install with gdebi ( or dpkg ).



## Working environments

TimeIT should work in any X11 environment, theoretically even on Windows (not tested)

Reported to work with following window managers:

* xfwm4 (XFCE)
* compiz (Unity)
* kwin (KDE)
* metacity (Gnome2, Unity)
* mutter (Gnome3)
* muffin (Cinnamon)
