
TOOL=convert
IMG=ecow.mem

all: tool
	@echo " [IMG] $(IMG)"
	@./$(TOOL)

clean:
	rm -f $(TOOL) $(IMG) *~

tool:
	@echo " [CC]  $(TOOL)"
	@cc -pedantic -Wall -o $(TOOL) convert.c
