CC := gcc
CFLAGS := -Wall
INCLUDES := -I "../"
LIBRARIES := -lcommons -lpthread

MFILESYSTEM := fileSystem.o
MODULOS = fileSystem.o

# Compilar 
fileSystem: $(MFILESYSTEM)
	$(CC) -g $(CFLAGS) $(INCLUDES) $(MFILESYSTEM) $(LIBRARIES) -o fileSystem 

fileSystem.o: fileSystem.c
	$(CC) $(CFLAGS) $(INCLUDES) -c fileSystem.c $(LIBRARIES)

# All
all: clean fileSystem

# Clean
clean:
	 rm -f $(MODULOS)
	 rm -f fileSystem
