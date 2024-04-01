CC   = cc
OBJS = my_string.o tokenizer.o process_file.o

CFLAGS = -O3 -g3 -Wall -Wextra -Werror=format-security -Werror=implicit-function-declaration \
         -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings -Wconversion

# This files are dummy files to test the basics of the interpreter
INSTRUCTIONS := LI_SP_ADD MV_X0 NEG_NOT J_JR

B_SUF = _BUFFER.txt
A_SUF = _ASSEMBLY.txt

all: riscv get_assembly_files $(foreach instr,$(INSTRUCTIONS),$(instr))

get_assembly_files:
	cp assembly_files/* .

riscv: $(OBJS) riscv.o
	$(CC) -o $@ $^

$(INSTRUCTIONS):
	touch $@$(B_SUF)  # Create buffer file
	# Interpret $@$(A_SUF) using ./riscv and populate $@$(B_SUF) with final registers status
	./riscv $@$(A_SUF) $@$(B_SUF)

clean:
	rm -f *.o riscv $(addsuffix $(A_SUF),$(INSTRUCTIONS)) $(addsuffix $(B_SUF),$(INSTRUCTIONS)) seekable_file.txt

my_string.o: my_string.c my_string.h
tokenizer.o: tokenizer.c tokenizer.h my_string.h
process_file.o: process_file.c process_file.h
riscv.o: riscv.c my_string.h tokenizer.h process_file.h

