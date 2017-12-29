# Copyright (c) 2010 Clearswift Ltd.
#
# Linux/UNIX Makefile for icl

# Project-specific compiler flags
CXXFLAGS_PROJ = -D"_ICL_IMPL" 

# ImageMagick may or may not be built by us - allow for both possibilities
CXXFLAGS_PROJ += -I/usr/include/ImageMagick -I$(USR_LOCAL)/include/ImageMagick 

LDFLAGS_PROJ = -L/$(USR_LOCAL)/lib -lopencv_core -lopencv_highgui -lopencv_ml \
	-lopencv_imgproc -lopencv_video -lopencv_features2d -lopencv_legacy \
	-lMagick++ -lalglib -lscl$(V)$(C) -lucl$(V)$(C) -lpthread 

MAPFILE = ../../../map_all

# Root of target name
TARGET_ROOT = icl
# C source files
CXXSOURCES = \
classifier.cpp \
colourlayoutfeat.cpp \
dfclassifier.cpp \
edgehistfeat.cpp \
momentsfeat.cpp \
image.cpp \
imgclassifier.cpp \
imgfeat.cpp \
lrclassifier.cpp \
metaclassifier.cpp \
skinfeat.cpp \
surffeat.cpp

# Public header files
HEADERS = $(shell ls *.hpp) export.h 

# Default goal
GOALS = shared

# Include common make stuff
include ../../../tech_common.mak
