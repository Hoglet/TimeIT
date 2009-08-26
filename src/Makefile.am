## Process this file with automake to produce Makefile.in

## Created by Anjuta

ACLOCAL_AMFLAGR = -I ../m4

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_VERSION=\"${PACKAGE_VERSION}\" \
	-DGETTEXT_PACKAGE=\"${GETTEXT_PACKAGE}\" \
	-DPROGRAMNAME_LOCALEDIR=\"${PROGRAMNAME_LOCALEDIR}\" \
 	-DPROGRAMNAME_DATADIR=\""$(datadir)/@PACKAGE@"\"  \
	$(TIMEIT_CFLAGS) \
	$(BOOST_CPPFLAGS)
	
AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = timeit

timeit_SOURCES = \
	DB/Task.cpp \
	DB/Task.h \
	DB/DBAbstraction/CSQL.cpp \
	DB/DBAbstraction/CSQL.h \
	DB/DBAbstraction/DataCell.cpp \
	DB/DBAbstraction/DataCell.h \
	DB/DBAbstraction/dbexception.cpp \
	DB/DBAbstraction/dbexception.h \
	Logic/AutoTracker.cpp \
	Logic/AutoTracker.h \
	Logic/Timekeeper.cpp \
	Logic/Timekeeper.h \
	Misc/ApplicationLock.cpp \
	Misc/ApplicationLock.h \
	Misc/OSAbstraction.cpp \
	Misc/OSAbstraction.h \
	Misc/Timer.cpp \
	Misc/Timer.h \
	Misc/Utils.cpp \
	Misc/Utils.h \
	Logic/IdleDetector.cpp \
	Logic/IdleDetector.h \
	Logic/Workspace.cpp \
	Logic/Workspace.h \
	GUI/GUIFactory.cpp \
	GUI/GUIFactory.h \
	GUI/internal/ICalendar.h \
	GUI/internal/IDetailsDialog.h \
	GUI/internal/ISummary.h \
	GUI/IActionObserver.h \
	GUI/IAddTaskDIalog.h \
	GUI/IEditTaskDialog.h \
	GUI/IGUIFactory.h \
	GUI/IStatusIcon.h \
	GUI/internal/AddTime.cpp \
	GUI/internal/AddTime.h \
	GUI/internal/Details.cpp \
	GUI/internal/Details.h \
	GUI/internal/DetailsDialog.cpp \
	GUI/internal/DetailsDialog.h \
	GUI/internal/EditTaskDialog.cpp \
	GUI/internal/EditTaskDialog.h \
	GUI/internal/IdleDialog.cpp \
	GUI/internal/IdleDialog.h \
	GUI/internal/LZSpinButton.cpp \
	GUI/internal/LZSpinButton.h \
	GUI/internal/StatusIcon.cpp \
	GUI/internal/StatusIcon.h \
	GUI/internal/TimeItAboutDialog.cpp \
	GUI/internal/TimeItAboutDialog.h \
	GUI/internal/MainWindow/Calendar.cpp \
	GUI/internal/MainWindow/Calendar.h \
	GUI/internal/MainWindow/MainWindow.cpp \
	GUI/internal/MainWindow/MainWindow.h \
	GUI/internal/MainWindow/Menu.cpp \
	GUI/internal/MainWindow/Menu.h \
	GUI/internal/MainWindow/Summary.cpp \
	GUI/internal/MainWindow/Summary.h \
	GUI/internal/MainWindow/SummaryViews.h \
	GUI/internal/MainWindow/TaskList.cpp \
	GUI/internal/MainWindow/TaskList.h \
	GUI/internal/MainWindow/Toolbar.cpp \
	GUI/internal/MainWindow/Toolbar.h \
	DB/Database.cpp \
	DB/Database.h \
	DB/IAutotrackAccessor.h \
	DB/ITaskAccessor.h \
	DB/ITimeAccessor.h \
	DB/internal/AutotrackAccessor.cpp \
	DB/internal/AutotrackAccessor.h \
	DB/internal/TaskAccessor.cpp \
	DB/internal/TaskAccessor.h \
	DB/internal/TimeAccessor.cpp \
	DB/internal/TimeAccessor.h \
	Logic/Controller.cpp \
	Logic/Controller.h \
	DB/internal/SettingsAccessor.cpp \
	DB/internal/SettingsAccessor.h \
	Main.cpp \
	GUI/internal/ISummary.cpp \
	GUI/internal/ParentChooser.cpp \
	GUI/internal/ParentChooser.h \
	Logic/ITimeKeeper.h \
	DB/ISettingsAccessor.h \
	GUI/IWidget.h \
	GUI/IIdleDialog.h \
	GUI/IMainWindow.h \
	Main.h \
	GUI/internal/PreferenceDialog.cpp \
	GUI/internal/PreferenceDialog.h \
	DB/DefaultValues.h

timeit_CXXFLAGS =        \
	-O0

timeit_LDFLAGS = 

timeit_LDADD = $(TIMEIT_LIBS)

EXTRA_DIST =

INCLUDES = -I$(top_srcdir)/src/DB/DBAbstraction \
	-I$(top_srcdir)/src/DB \
	-I$(top_srcdir)/src/GUI \
	-I$(top_srcdir)/src/GUI/internal \
	-I$(top_srcdir)/src/GUI/internal/MainWindow \
	-I$(top_srcdir)/src/Logic \
	-I$(top_srcdir)/src/Misc

