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

#ifndef MRMR_ATTRIBUTE_INFORMATION_HPP
#define MRMR_ATTRIBUTE_INFORMATION_HPP

#include <array>
#include <unordered_map>
#include <cassert>
#include <iterator>
#include <limits>
#include <valarray>
#include <vector>
#include <stdexcept>
#include "typedef.hpp"

template <typename T>
class attribute_information {
	public:
		template <typename ForwardIterator> attribute_information( ForwardIterator first, ForwardIterator last );
		T num_values() const;
		std::vector<T> values() const;
		double entropy() const;
		probability marginal_probability( T index ) const;
	private:
		double _entropy;
		std::unordered_map< T, probability > _pdf;
		std::vector<T> _values;
};

template <typename T>
template <typename ForwardIterator>
attribute_information<T>::attribute_information( ForwardIterator first, ForwardIterator last ) {
	// determine number of elements
	double count = static_cast< double >( last - first );

	while( first != last ) {
		T val = *first;

		if ( _pdf.count( *first ) > 0 ) {
			_pdf[ *first ] += 1;
		} else {
			_pdf[ *first ] = 1;
			_values.push_back( *first );
		}

		++first;
	}

	std::valarray< probability > p( _pdf.size() );
	int i = 0;
	for ( auto it = _pdf.begin(); it != _pdf.end(); it++ ) {
		it->second = it->second / count;
		p[i++] = it->second;
	}

	// compute entropy
	_entropy = -1 * ( p * std::log( p ) ).sum() / std::log( 2 );
}

template <typename T>
T attribute_information<T>::num_values() const {
	return _pdf.size();
}

template <typename T>
std::vector<T> attribute_information<T>::values() const {
	return _values;
}

template <typename T>
double attribute_information<T>::entropy() const {
	return _entropy;
}

template <typename T>
probability attribute_information<T>::marginal_probability( T value ) const {
	try {
		return _pdf.at ( value );
	} catch (std::out_of_range& err) {
		return 0.0;
	}
}

#endif
