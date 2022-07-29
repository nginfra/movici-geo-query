from _movici_geo_query import CGeoQuery
import numpy as np


def test_pybind11_input_class_and_mapping():
    indices = np.array([0, 1, 2], dtype=np.uint32)
    point_array = np.array([[0, 1], [2, 3], [4, 5]], dtype=np.double)
    input_type = "point"
    query = CGeoQuery(point_array, indices, input_type)
    nearest = query.nearest_to(point_array, indices, input_type)
    assert np.array_equal(nearest.indices(), np.array([0, 1, 2]))
    assert np.array_equal(nearest.distances(), np.array([0, 0, 0]))
