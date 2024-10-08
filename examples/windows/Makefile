#
# Makefile for application
#

CROSS_COMPILE	?= x86_64-w64-mingw32-

AS			:=	$(CROSS_COMPILE)gcc -x assembler-with-cpp
CC			:=	$(CROSS_COMPILE)gcc
CXX			:=	$(CROSS_COMPILE)g++
LD			:=	$(CROSS_COMPILE)ld
AR			:=	$(CROSS_COMPILE)ar
SZ			:=	$(CROSS_COMPILE)size
OC			:=	$(CROSS_COMPILE)objcopy
OD			:=	$(CROSS_COMPILE)objdump
STRIP		:=	$(CROSS_COMPILE)strip
MKDIR		:=	mkdir -p
RM			:=	rm -fr

ASFLAGS		:=	-g -ggdb -Wall -O3 -ffunction-sections -fdata-sections -ffreestanding -municode -std=gnu99
CFLAGS		:=	-g -ggdb -Wall -O3 -ffunction-sections -fdata-sections -ffreestanding -municode -std=gnu99
CXXFLAGS	:=	-g -ggdb -Wall -O3 -ffunction-sections -fdata-sections -ffreestanding -municode
LDFLAGS		:=
OCFLAGS		:=	-v -O binary
ODFLAGS		:=
MCFLAGS		:=

LIBDIRS		:=
LIBS 		:=
INCDIRS		:=	.
SRCDIRS		:=	.

CFLAGS		+=	$(shell SDL2-2.30.8/x86_64-w64-mingw32/bin/sdl2-config --cflags)
CXXFLAGS	+=	$(shell SDL2-2.30.8/x86_64-w64-mingw32/bin/sdl2-config --cflags)
LIBDIRS		+=
LIBS		+=	$(shell SDL2-2.30.8/x86_64-w64-mingw32/bin/sdl2-config --static-libs)
sinclude	../../src/xnes.mk

OUTPUT		:=	output
NAME		:=	$(OUTPUT)/xnes

INCDIRS		:=	$(patsubst %, -I %, $(sort $(INCDIRS)))
LIBDIRS		:=	$(patsubst %, -L %, $(sort $(LIBDIRS)))
SRCDIRS		:=	$(sort $(SRCDIRS))
OBJDIRS		:=	$(patsubst %, .obj/%, $(SRCDIRS))

SFILES		:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.S))
CFILES		:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))
CPPFILES	:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.cpp))

SDEPS		:= $(patsubst %, .obj/%, $(SFILES:.S=.o.d))
CDEPS		:= $(patsubst %, .obj/%, $(CFILES:.c=.o.d))
CPPDEPS		:= $(patsubst %, .obj/%, $(CPPFILES:.cpp=.o.d))
DEPS		:= $(SDEPS) $(CDEPS) $(CPPDEPS)

SOBJS		:= $(patsubst %, .obj/%, $(SFILES:.S=.o))
COBJS		:= $(patsubst %, .obj/%, $(CFILES:.c=.o))
CPPOBJS		:= $(patsubst %, .obj/%, $(CPPFILES:.cpp=.o)) 
OBJS		:= $(SOBJS) $(COBJS) $(CPPOBJS)

VPATH		:= $(OBJDIRS)

.PHONY: all clean

all : $(NAME)

$(NAME) : $(OBJS)
	@echo [LD] Linking $@
	@$(CC) $(LDFLAGS) $(LIBDIRS) $^ -o $@ $(LIBS) -static

$(SOBJS) : .obj/%.o : %.S
	@echo [AS] $<
	@$(AS) $(ASFLAGS) -MD -MP -MF $@.d $(INCDIRS) -c $< -o $@

$(COBJS) : .obj/%.o : %.c
	@echo [CC] $<
	@$(CC) $(CFLAGS) -MD -MP -MF $@.d $(INCDIRS) -c $< -o $@

$(CPPOBJS) : .obj/%.o : %.cpp
	@echo [CXX] $<
	@$(CXX) $(CXXFLAGS) -MD -MP -MF $@.d $(INCDIRS) -c $< -o $@

clean:
	@$(RM) .obj $(OUTPUT)

sinclude $(shell $(MKDIR) $(OBJDIRS) $(OUTPUT)) $(DEPS)
