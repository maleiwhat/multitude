TEMPLATE = subdirs

SUBDIRS += FireView
SUBDIRS += MoviePlayer

linux-* {
  stuff.path = /src/MultiTouch/multitude/Applications
  stuff.files = Applications.pro Applications.pri Applications_end.pri

  INSTALLS += stuff
}
