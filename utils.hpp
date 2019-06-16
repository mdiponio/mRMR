/*
Copyright (C) 2018 by Ryan N. Lichtenwalter
Email: rlichtenwalter@gmail.com

This file is part of the Improved mRMR code base.

Improved mRMR is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Improved mRMR is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <chrono>
#include <iostream>

enum verbosity_level : char {
	QUIET = 0,
	INFO = 1,
	DEBUG = 2
};

enum message_type : char {
	STANDARD = 0,
	START = 1,
	FINISH = 2
};

class logger {
    public:
        void message( const char * message, verbosity_level verbosity, message_type m_type = STANDARD );

        static logger * get();

         void set_level(verbosity_level level) {
            _level = level;
        }

    private:
        static logger * _instance;

        std::chrono::time_point<std::chrono::high_resolution_clock> _start_time;
        verbosity_level _level;

        std::ostream * _out;

        logger(verbosity_level level, std::ostream& out): _level(level), _out(&out)
        { }      
};

#endif 