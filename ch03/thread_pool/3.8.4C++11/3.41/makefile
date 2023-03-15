GCC ?= g++
CCMODE = PROGRAM
CXXFLAGS += -std=c++11 
#INCLUDES =  -Iqinclude -Iinclude -DLINUX  -DMW_STDIO
CFLAGS =  -Wall $(MACRO) 
TARGET = threadPool
SRCS := $(wildcard *.cpp)   
LIBS = -lpthread 




ifeq ($(CCMODE),PROGRAM)
$(TARGET): $(LINKS) $(SRCS) 
	$(GCC) $(CFLAGS) $(INCLUDES) -o $(TARGET)  $(SRCS) $(LIBS)
	@chmod +x $(TARGET)
	@echo make $(TARGET) ok.
clean:
	rm -rf $(TARGET)
endif

 
clean:
	rm -f $(TARGET)
 


.PHONY:install
.PHONY:clean