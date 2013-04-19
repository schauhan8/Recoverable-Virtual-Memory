CFLAGS = -Wall -g
LFLAGS =
CC = gcc
RM = /bin/rm -rf
AR = ar rc
RANLIB = ranlib

LIBRARY = lrvm_lib.a
LIB_SRC = gt_lrvm.c gt_segment.c gt_directory.c gt_utils.c gt_transaction.c
LIB_OBJ = $(patsubst %.c,%.o,$(LIB_SRC))

# pattern rule for object files
%.o: %.c
		$(CC) -c $(CFLAGS) $< -o $@

all: $(LIBRARY)

$(LIBRARY): $(LIB_OBJ)
		$(AR) $(LIBRARY) $(LIB_OBJ)
		$(RANLIB) $(LIBRARY)

clean:
		$(RM) $(LIBRARY) $(LIB_OBJ)

.PHONY: depend
depend:
		makedepend -Y -- $(CFLAGS) -- $(LIB_SRC) 2>/dev/null
