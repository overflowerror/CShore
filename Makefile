CC = gcc
LD = gcc
CFLAGS = -Wall -g -std=c99 -ICFloor/src/ -ICson/src/ -D_POSIX_SOURCE
LDFLAGS = -lpthread -lrt

CFLOOR_LIB = CFloor/libcfloor.a
CSON_LIB = Cson/libcson.a
LIBS = $(CFLOOR_LIB) $(CSON_LIB)

OBJS = obj/router.o obj/request.o obj/base_cfloor.o
DEPS = $(OBJS:%.o=%.d)

DEMO_OBJS = obj/demo.o

HAS_MAIN =
NEEDS_MAIN = \
if test "$(HAS_MAIN)" != "yes"; then\
	echo "ERROR: Trying to build standalone without base.";\
	echo "       Specify target base_cfloor instead.";\
	exit 1;\
fi

all: base_cfloor clean

base_cfloor: CFLAGS += -DBASE_CFLOOR
base_cfloor: HAS_MAIN = "yes"
base_cfloor: standalone


standalone: $(DEMO_OBJS) $(OBJS) $(LIBS)
	@$(NEEDS_MAIN)
	$(LD) $(LDFLAGS) -o $@ $^

$(CFLOOR_LIB):
	$(MAKE) -C CFloor/ libcfloor.a
	
$(CSON_LIB):
	$(MAKE) -C Cson/ libcson.a

-include $(DEPS)

obj/%.o: demo/%.c obj
	$(CC) $(CFLAGS) -Isrc/ -MMD -c -o $@ $<

obj/%.o: src/%.c obj
	$(CC) $(CFLAGS) -MMD -c -o $@ $<
	
obj:
	@mkdir -p obj

clean:
	@echo "Cleaning up..."
	@rm -f obj/*.o
	@rm -f obj/*.d
	@rm -f standalone
	$(MAKE) -C CFloor/ clean
	$(MAKE) -C Cson/ clean
