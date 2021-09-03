import numpy as np
import pytest
from movici_geo_query.geo_query import GeoQuery
from movici_geo_query.geometry import (
    PointGeometry,
    LinestringGeometry,
)


@pytest.fixture
def some_points():
    return PointGeometry(points=[[0.1, 0.1], [1.1, 1.1], [1.5, 1.9]])


@pytest.fixture
def other_points():
    return PointGeometry(points=[[0.2, 0.2], [0.3, 0.1], [1.5, 1.9], [1.0, 1.0]])


@pytest.fixture
def far_away_points():
    return PointGeometry(points=[[100.1, 100.1], [101.1, 101.1], [101.5, 101.9]])


@pytest.fixture
def some_lines():
    return LinestringGeometry(
        points=[[0.1, 0.1], [1.1, 0.1], [1.5, 0.1], [2.3, 2.0], [5.0, 5.0]],
        row_ptr=[0, 3, 5],
    )


class TestPointQueries:
    def test_nearest_points_to_points(self, some_points, other_points):
        rv = GeoQuery(some_points).nearest_to(other_points)
        assert np.array_equal(rv.indices, [0, 0, 2, 1])
        assert np.allclose(
            rv.distances, [np.sqrt(2) / 10, 0.2, 0, np.sqrt(2) / 10], rtol=1.0e-6
        )

    def test_nearest_points_to_points_reverse(self, some_points, other_points):
        rv = GeoQuery(other_points).nearest_to(some_points)
        assert np.array_equal(rv.indices, [0, 3, 2])

    def test_within_distance_of_points_to_points(self, some_points, other_points):
        rv = GeoQuery(other_points).within_distance_of(some_points, 1.0)
        assert np.array_equal(rv.indices, [0, 1, 2, 3, 2])
        assert np.array_equal(rv.row_ptr, [0, 2, 4, 5])

    def test_nearest_lines_to_points(self, some_points, some_lines):
        rv = GeoQuery(some_lines).nearest_to(some_points)
        assert np.array_equal(rv.indices, [0, 0, 1])
        assert np.allclose(
            rv.distances, [0, 1, np.sqrt(0.8 * 0.8 + 0.1 * 0.1)], rtol=1.0e-6
        )

    def test_points_to_lines(self):
        lines = LinestringGeometry(
            np.array(
                [[0, 0], [1, 0], [1, 1], [-1, 1], [-100, 0], [-101, 0]], dtype=float
            ),
            row_ptr=[0, 2, 4, 6],
        )
        points = PointGeometry(
            np.array(
                [[1.5, 0.4], [0.5, 0], [0.5, 1.5], [0, 1], [1.5, 0.5]], dtype=float
            )
        )

        rv = GeoQuery(points).within_distance_of(lines, 0.1)
        assert np.array_equal(rv.indices, np.array([1, 3]))
        assert np.array_equal(rv.row_ptr, np.array([0, 1, 2, 2]))

    def test_with_non_contiguous_array(self):
        lines_3d = np.array(
            [[0, 0, 0], [1, 0, 0], [1, 1, 0], [-1, 1, 0], [-100, 0, 0], [-101, 0, 0]],
            dtype=float,
        )
        lines = LinestringGeometry(
            lines_3d[:, 0:2],
            row_ptr=[0, 2, 4, 6],
        )
        points = PointGeometry(
            np.array(
                [[1.5, 0.4], [0.5, 0], [0.5, 1.5], [0, 1], [1.5, 0.5]], dtype=float
            )
        )

        rv = GeoQuery(points).within_distance_of(lines, 0.1)
        assert np.array_equal(rv.indices, np.array([1, 3]))
        assert np.array_equal(rv.row_ptr, np.array([0, 1, 2, 2]))
