# The name of your app.
# NOTICE: name defined in TARGET has a corresponding QML filename.
#         If name defined in TARGET is changed, following needs to be
#         done to match new name:
#         - corresponding QML filename must be changed
#         - desktop icon filename must be changed
#         - desktop filename must be changed
#         - icon definition filename in desktop file must be changed
TARGET = tohiri

CONFIG += sailfishapp
QT += dbus

#Force building to update version and build-date
system(rm $$OUT_PWD/tohir.o)

#show some info about git status
system(git --git-dir $$PWD/.git diff --name-only)

REVISION = $$system(git --git-dir $$PWD/.git --work-tree $$PWD describe --dirty=-dev --always)
DEFINES += "GITHASH=\\\"$${REVISION}\\\""

message($${REVISION})


SOURCES += src/tohiri.cpp \
	src/tohir.cpp
	
HEADERS += src/tohir.h

OTHER_FILES += qml/tohiri.qml \
    qml/cover/CoverPage.qml \
    qml/pages/Tohiri.qml \
    rpm/tohiri.spec \
    rpm/tohiri.yaml \
	tohiri.png \
    tohiri.desktop \
    qml/pages/aboutPage.qml

