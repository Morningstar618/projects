TARGET = bin/fileDB
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	#./$(TARGET) -n -f mynewdb.db
	./$(TARGET) -n -f mynewdb.db -a "Ayush|Chandigarh|100"

default: $(TARGET)

clean:
	rm -rf obj/*.o
	rm -rf bin/*
	rm -rf *.db

$(TARGET): $(OBJ)
	gcc -o $@ $? -lws2_32

obj/%.o: src/%.c
	gcc -c $< -o $@ -Iinclude
