CXXFLAGS =	-O3 -g -pthread -static 
INCLUDES = -I/usr/local/include -I/usr/local/include/boost-1_53
LIBS = -L/usr/local/lib
LDFLAGS = -lcddgmp -lgmpxx -lgmp -lboost_regex-mgw47-mt-1_53
SRCS = rfpe.cpp RepeatedGame.cpp Automaton.cpp Belief.cpp Environment.cpp Player.cpp Equation.cpp Payoff.cpp BeliefDivision.cpp Util.cpp Reader.cpp Writer.cpp Parser.cpp Profile.cpp
OBJS =		$(SRCS:.cpp=.o)
TARGET =	rfpe

.SUFFIXES : .cpp .o

$(TARGET):	$(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -o $(TARGET) $(OBJS) $(LDFLAGS) 

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ -c $^		
	 