SRC = $(wildcard src/*.cpp)
DEP = $(patsubst src/%.cpp, dep/%.d, $(SRC))
OBJ = $(patsubst src/%.cpp, obj/%.o, $(SRC))
BIN = proj

LDFLAGS = $(shell pkg-config --libs gtkmm-2.4 gtkglextmm-1.2 lua)
CPPFLAGS = $(shell pkg-config --cflags gtkmm-2.4 gtkglextmm-1.2 lua)
CXXFLAGS = $(CPPFLAGS) -W -Wall -O3

all: $(BIN)

depend: $(DEP)

$(BIN): $(OBJ)
	@echo Linking $@...
	@$(CXX) -o $@ $(OBJ) $(LDFLAGS)
	@echo Done linking $@.

dep/%.d: src/%.cpp
	@echo Generating dependancies for $<...
	@set -e; $(CC) -M $(CPPFLAGS) $< \
					| sed 's/\($*\)\.o[ :]*/obj\/\1.o $(patsubst dep/%, dep\/%, $@) : /g' > $@; \
			[ -s $@ ] || rm -f $@
	@echo Done generating dependancies for $@.

obj/%.o: src/%.cpp
	@echo Compiling $@...
	@$(CXX) -o $@ -c $(CXXFLAGS) $<
	@echo Done compiling $@.

clean:
	rm -f obj/*
	rm -f dep/*
	rm -f $(BIN)

include $(DEP)
