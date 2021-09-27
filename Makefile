CXX_FLAGS=-W -Wall -Wextra

schema-compiler: schema-compiler.cc
	$(CXX) $(CXX_FLAGS) $^ -o$@

clean:
	rm -f schema-compiler
