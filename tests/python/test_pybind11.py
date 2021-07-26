from interface import CInput, CIndexVector, CGeoQuery
import numpy as np
from boost_geo_query.geometry import (
    PointGeometry,
)


def get_grid_points(nb_x_elem=1000, nb_y_elem=1000):
    return PointGeometry(points=np.mgrid[0:nb_x_elem, 0:nb_y_elem].reshape(2, -1).T)


def test_memory_usage_test():
    hundred_million_points = get_grid_points(10000,10000)
    indices = CIndexVector(np.array([0], dtype=np.uint32))
    input_type = "point"
    points = hundred_million_points.as_c_input()
    print(len(points))


def test_pybind11_input_class_and_mapping():
    indices = CIndexVector(np.array([0, 1, 2], dtype=np.uint32))
    point_array = np.array([[0, 1], [2, 3], [4, 5]], dtype=np.double)
    input_type = "point"
    points = CInput(point_array, indices, input_type)
    query = CGeoQuery(points)
    nearest = query.nearest_to(points)
    assert np.array_equal(nearest.indices(), np.array([0, 1, 2]))
    assert np.array_equal(nearest.distances(), np.array([0, 0, 0]))
