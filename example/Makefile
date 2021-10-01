CXX_FLAGS=-W -Wall -Wextra -std=c++17

SCHEMA_COMPILER=../jcxxgen

testing-textdocs: testing-textdocs.cc textdocs.h
	$(CXX) $(CXX_FLAGS) $< -o$@

textdocs.h: textdocs.yaml $(SCHEMA_COMPILER)
	$(SCHEMA_COMPILER) $< -o $@

$(SCHEMA_COMPILER):
	$(MAKE) -C ..

clean:
	rm -f textdocs.h testing-textdocs
