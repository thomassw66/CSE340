CODE_DIR=./code
BUILD_DIR=./build


a.out: $(BUILD_DIR)/compiler.o $(BUILD_DIR)/ir_debug.o $(BUILD_DIR)/lexer.o $(BUILD_DIR)/inputbuf.o 
	g++ -o a.out $(BUILD_DIR)/compiler.o $(BUILD_DIR)/ir_debug.o $(BUILD_DIR)/lexer.o $(BUILD_DIR)/inputbuf.o
	rm $(BUILD_DIR)/*.o

$(BUILD_DIR)/compiler.o: $(CODE_DIR)/compiler.h
	g++ -c $(CODE_DIR)/compiler.cc -o $(BUILD_DIR)/compiler.o

$(BUILD_DIR)/ir_debug.o: $(CODE_DIR)/ir_debug.h
	g++ -c $(CODE_DIR)/ir_debug.cc -o $(BUILD_DIR)/ir_debug.o 

$(BUILD_DIR)/lexer.o: $(CODE_DIR)/lexer.h
	g++ -c $(CODE_DIR)/lexer.cc -o $(BUILD_DIR)/lexer.o 

$(BUILD_DIR)/inputbuf.o: $(CODE_DIR)/inputbuf.h 
	g++ -c $(CODE_DIR)/inputbuf.cc -o $(BUILD_DIR)/inputbuf.o

clean:
	rm $(BUILD_DIR)/*.o

