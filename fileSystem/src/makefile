CC := gcc
CFLAGS := -Wall
INCLUDES := -I "/home/utnso/workspace/commons"
LIBRARIES := -lcommons -lpthread

MFILESYSTEM := ABM.o fileSystem.o

# Compilar 
fileSystem: $(MFILESYSTEM)
	$(CC) -g $(CFLAGS) $(INCLUDES) -o fileSystem $(MFILESYSTEM) $(LIBRARIES)

fileSystem.o: fileSystem.c
	$(CC) $(CFLAGS) $(INCLUDES) -c fileSystem.c $(LIBRARIES)

# All
all: clean fileSystem

# Clean
clean:
	 rm -f $(MFILESYSTEM)
	 rm -f fileSystem
