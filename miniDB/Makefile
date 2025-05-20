flags=-Wall -O2 -std=c1x

# Detect platform
ifeq ($(OS),Windows_NT)
    RM = del /Q
    RMDIR = rmdir /S /Q
    RM_NULL = >nul 2>&1
	EXEEXT = .exe
else
    RM = rm -f
    RMDIR = rm -rf
    RM_NULL = >/dev/null 2>&1
	EXEEXT = 
endif

all: clean tree

tree: tree.o
	gcc ${flags} $^ -o $@ ${ldflags}

tree.o: tree.c
	gcc ${flags} -c $^

clean:
	-$(RM) *.o $(RM_NULL)
	-$(RM) tree$(EXEEXT) $(RM_NULL)
