SRC = $(wildcard src/*.cpp)
DEP = $(patsubst src/%.cpp, dep/%.d, $(SRC))
OBJ = $(patsubst src/%.cpp, obj/%.o, $(SRC))
BIN = proj

LDFLAGS = $(shell pkg-config --libs gtkmm-2.4 gtkglextmm-1.2) -ljpeg -ltiff
CPPFLAGS = $(shell pkg-config --cflags gtkmm-2.4 gtkglextmm-1.2)
CXXFLAGS = $(CPPFLAGS) -W -Wall -O3

CXX = g++

.PHONY : clean dep

all: $(BIN)

depend: $(DEP)

$(BIN): $(OBJ)
	@echo Linking $@...
	@$(CXX) -o $@ $(OBJ) $(LDFLAGS)
	@echo Done linking $@.

dep/%.d: src/%.cpp
	@echo Generating dependancies for $<...
	@if [ ! -d "dep" ]; then mkdir dep; fi
	@set -e; $(CC) -M $(CPPFLAGS) $< \
					| sed 's/\($*\)\.o[ :]*/obj\/\1.o $(patsubst dep/%, dep\/%, $@) : /g' > $@; \
			[ -s $@ ] || rm -f $@
	@echo Done generating dependancies for $@.

obj/%.o: src/%.cpp
	@echo Compiling $@...
	@if [ ! -d "obj" ]; then mkdir obj; fi
	@$(CXX) -o $@ -c $(CXXFLAGS) $<
	@echo Done compiling $@.

clean:
	rm -f obj/*
	rm -f dep/*
	rm -f $(BIN)

ifneq ($(MAKECMDGOALS),clean)
-include $(DEP)
endif

