C = g++
FLAGS = -W -O3
#OBJECTS = diamond_easy.o
BOOST_REGEX_LOCATION = /usr/local/lib/libboost_regex.a

all : diamond_easy

diamond_easy :
	$(C) $(FLAGS) *.cpp *.h -o diamond_easy $(BOOST_REGEX_LOCATION)

clean :
	rm -rf *.o