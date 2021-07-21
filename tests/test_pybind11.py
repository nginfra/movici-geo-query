from interface import CInput, CIndexVector, CGeoQuery
import numpy as np


def test_pybind11_input_class_and_mapping():
    indices = CIndexVector(np.array([0, 1, 2], dtype=np.uint32))
    point_array = np.array([[0, 1], [2, 3], [4, 5]], dtype=np.double)
    type = "point"
    bla = CInput(point_array, indices, type)
    query = CGeoQuery(bla)
    nearest = query.nearest_to(bla)
    assert np.array_equal(nearest.results(), np.array([0, 1, 2]))
    assert np.array_equal(nearest.distances(), np.array([0, 0, 0]))
