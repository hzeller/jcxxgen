CXX_FLAGS=-W -Wall -Wextra

THIRD_PARTY_INCLUDES=-I../third_party/json/include

testing-lsp: testing-lsp.cc lsp-protocol.h
	$(CXX) $(CXX_FLAGS) $(THIRD_PARTY_INCLUDES) $< -o$@

lsp-protocol.h: lsp-protocol.yaml ../schema-compiler
	../schema-compiler $< > $@

../schema-compiler:
	$(MAKE) -C ..

clean:
	rm -f lsp-protocol.h testing-lsp
