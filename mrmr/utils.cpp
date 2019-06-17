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

#include <iomanip>

#include "utils.hpp"

logger * logger::_instance = nullptr;

logger *  logger::get() {
    if ( ! _instance ) {
        _instance = new logger(QUIET, std::cerr);
    }

    return _instance;
}

void logger::message( char const * message, verbosity_level verbosity, message_type mtype ) {
	if( _level >= verbosity ) {
		if( mtype == STANDARD || mtype == START ) {
			std::time_t time = std::time( nullptr );
			*_out << std::put_time( std::localtime( &time ), "%Y-%m-%d %H:%M:%S" ) << " - " << message;
		}
		if( mtype == STANDARD ) {
			*_out << '\n';
		} else if( mtype == START ) {
			_start_time = std::chrono::high_resolution_clock::now();
		} else if( mtype == FINISH ) {
			std::chrono::duration<double> time_span = std::chrono::duration_cast< std::chrono::duration<double> >( 
                std::chrono::high_resolution_clock::now() - _start_time );
			*_out << "DONE (" << time_span.count() << " seconds)\n";
		}
	}
}
