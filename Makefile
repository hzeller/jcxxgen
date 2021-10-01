CXX_FLAGS=-W -Wall -Wextra

jcxxgen: schema-compiler.cc
	$(CXX) $(CXX_FLAGS) $^ -o$@

clean:
	rm -f jcxxgen
