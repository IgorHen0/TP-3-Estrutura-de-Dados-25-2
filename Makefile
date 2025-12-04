# Variáveis
CC = gcc
CFLAGS = -g -Wall -Wextra -Iinclude
SRCDIR = src
OBJDIR = obj
BINDIR = bin
EXEC = $(BINDIR)/tp3.out

# Obtém todos os arquivos .c em src
SRC = $(wildcard $(SRCDIR)/*.c)
# Converte os arquivos .c para .o, mas salva em obj
OBJ = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))

# Alvo padrão
all: $(EXEC)

# Cria o executável a partir dos objetos
$(EXEC): $(OBJ)
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compila cada arquivo .c para .o
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza
clean:
	rm -rf $(OBJDIR)/*.o $(BINDIR)/tp3.out