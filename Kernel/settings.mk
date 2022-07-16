# Libraries
LIBS=static commons pthread

# Custom libraries' paths
SHARED_LIBPATHS= 
STATIC_LIBPATHS= ../static

# Compiler flags
CDEBUG=-g -Wall -DDEBUG
CRELEASE=-O3 -Wall -DNDEBUG

# Arguments when executing with start, memcheck or helgrind
ARGS=

# Valgrind flags
MEMCHECK_FLAGS=--leak-check=full --show-leak-kinds=all --track-origins=yes --log-file="memcheck.log"
HELGRIND_FLAGS=--log-file="helgrind.log"
