##
# Copyright (C) 2018 by Ryan N. Lichtenwalter
# Email: rlichtenwalter@gmail.com
#
# This file is part of the Improved mRMR code base.
#
# Improved mRMR is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Improved mRMR is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
##

CC := g++
COMMON_FLAGS := -std=c++14 -Wall -Wextra -Werror -Wno-unused-local-typedefs -pedantic

DEBUG_FLAGS := -Og -g -fsanitize=address -fno-omit-frame-pointer  -fmax-errors=1
RELEASE_FLAGS := -O3 -flto -fomit-frame-pointer -D NDEBUG

ifeq ($(DEBUG),1)
	CFLAGS := $(COMMON_FLAGS) $(DEBUG_FLAGS)
else
	CFLAGS := $(COMMON_FLAGS) $(RELEASE_FLAGS)
endif

PYTHON_LIB_NAME=libmrmr_py.so

mrmr: main.cpp utils.o 
	$(CC) $(CFLAGS) -o $@ $^

py: mrmr_py.cpp utils.o
	$(CC) -shared -fPIC $(CFLAGS) -o $(PYTHON_LIB_NAME) $^

test: tests
	./tests

tests: tests.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean test

clean:
	rm -f *.o *.so mrmr 

