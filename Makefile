
PROG=cic_iss

GCC=g++

SRCS=main.cpp

CXXFLAGS = -DSC_CPLUSPLUS=201402L -std=c++14
CXXFLAGS+= -Wall

OBJS=$(SRCS:.cpp=.o)
SYSTEMC_HOME=/usr/local/systemc-2.3.3/

LDADD=-L$(SYSTEMC_HOME)/lib-linux64 -Wl,-rpath=$(SYSTEMC_HOME)/lib-linux64 -lsystemc -lm

all: $(PROG)

$(PROG): $(OBJS)
	$(GCC) $(LDADD) -o $(PROG) $(OBJS)
 
%.o: %.cpp
	$(GCC) $(CXXFLAGS) -I$(SYSTEMC_HOME)/include -c $(SRCS)

.PHONY:clean
clean:
	rm -f $(PROG)
	rm -f $(OBJS)
	rm -f *.vcd
	rm -f *.txt
