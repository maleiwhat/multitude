include(../Examples.pri)

SOURCES += Main.cpp

LIBS += $$LIB_VALUABLE $$LIB_RADIANT

win32: CONFIG += console