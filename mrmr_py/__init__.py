#
# Copyright (C) 2019 Michael Diponio
# Email: mdiponio@gmail.com
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
#

from ctypes import *
from enum import Enum
from os.path import realpath, dirname, isfile
from typing import List, Tuple

from numpy import ubyte
from pandas import DataFrame

# Loaded native library.
_mrmr_lib = None


class MRMRMethod(Enum):
    """
    mRMR methods supported. These are:

      - MID - mutual information difference
      - MIQ - mutual information quotient
    """
    MID = 0 
    MIQ = 1


def setup(path: str = None) -> None:
    """
    Setup by loading native library.

    :param path: native library path (optional, default relative to this file)
    :raises OSError: parameter not found
    """
    global _mrmr_lib

    if _mrmr_lib:
        # Library previously loaded
        return

    if not path:
        path = dirname(realpath(__file__)) + "/../mrmr/libmrmr_py.so"

    if not isfile(path):
        raise OSError("library path not found")

    path = realpath(path)
    _mrmr_lib = cdll.LoadLibrary(path)

    if not _mrmr_lib:
        raise OSError("failed linking with library")

    _mrmr_lib.get_feature_ranks.restype = POINTER(c_char_p)
    _mrmr_lib.get_mrmr_score.restype = POINTER(c_double)
    _mrmr_lib.get_last_error.restype = c_char_p

    
def mrmr(dataset: DataFrame, features: List[str] = [], label: str = None, num_features: int = 0,
         method: MRMRMethod = MRMRMethod.MID) -> Tuple[List[str], List[float]]:
    """
    Run MRMR algorithm

    :param dataset: pandas data frame with feature values
    :param features: list of features to use (optional, default all)
    :param label: feature label (optional, default first column)
    :param num_features: top number of features to rank
    :param method: MRMR method (defaults to MID)
    :return: tuple containing feature ranks and MRMR scores 
    :raises OSError: native library not linked
    :raises MRMRError mRMR execution error
    """
    global _mrmr_lib
    if not _mrmr_lib:
        raise OSError("native library not linked")

    # Create environment 
    env = _mrmr_lib.setup_mrmr(c_int(0))
    if not env:
        raise MRMRError("failed setting up environment")

    try:

        # Pass in data feature data
        if len(features) == 0:
            features = list(dataset.columns)

        if not label:
            label = features.pop(0)
        elif label in features:
            features.remove(label)
        else:
            raise MRMRError("label not in dataset")

        dataset = dataset.dropna()

        c_uint8_p = POINTER(c_uint8)
        data = dataset[label].astype(ubyte)
        _mrmr_lib.add_attribute_byte(c_void_p(env), c_char_p(label.encode('utf-8')),
                                     data.values.ctypes.data_as(c_uint8_p), c_int(data.size))

        for feature in features:
            data = dataset[feature].astype(ubyte)
            _mrmr_lib.add_attribute_byte(c_void_p(env), c_char_p(feature.encode('utf-8')),
                                         data.values.ctypes.data_as(c_uint8_p), c_int(data.size))

        # Run MRMR
        num_ranked = _mrmr_lib.perform_mrmr(c_void_p(env), c_uint(method.value), c_uint(0), c_uint(num_features))

        if num_ranked < 0:
            # Error occurred
            err = str(_mrmr_lib.get_last_error(), encoding='utf-8')
            raise MRMRError("Error %d, %s" % (num_ranked, err))

        if num_ranked < 1:
            # No features ranked
            return [], []

        # Extract results
        num = c_int()
        buf = _mrmr_lib.get_feature_ranks(c_void_p(env), byref(num))

        ranks = []
        for i in range(num.value):
            ranks.append(buf[i].decode('utf-8'))

        scores = []
        score_buf = _mrmr_lib.get_mrmr_score(c_void_p(env), byref(num))
        for i in range(num.value):
            scores.append(score_buf[i])

        return ranks, scores

    finally:
        _mrmr_lib.destroy_mrmr(c_void_p(env))


class MRMRError(Exception):
    """Exception executing mRMR."""
    pass

