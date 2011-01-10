# This file is licensed under the terms of MIT license, see LICENSE file.

target = src/fflisp
objs = src/fflisp.o src/environment.o src/eval.o src/read.o \
		src/print.o src/heap.o src/object.o src/subr.o src/repl.o
headers = include/fflisp.h include/environment.h include/eval.h include/read.h \
			include/print.h include/heap.h include/object.h include/subr.h \
			include/repl.h

LDFLAGS +=
CFLAGS += -g

.PHONY: all clean
all: $(objs)
	gcc -o $(target) $(objs) $(LDFLAGS)

$(objs): $(headers)

clean:
	rm -fv $(objs) $(target)
