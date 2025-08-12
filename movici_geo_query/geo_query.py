"""High-performance spatial query engine using R-tree indexing."""

import typing as t

import numpy as np
from _movici_geo_query import CGeoQuery

from .geometry import Geometry


class QueryResult:
    """Container for spatial query results.

    Attributes:
        indices: Array of indices matching the query criteria
        row_ptr: Optional CSR-style row pointer array for grouped results
        distances: Optional array of distances (for nearest neighbor queries)
    """

    def __init__(
        self,
        indices: np.ndarray,
        row_ptr: np.ndarray | None = None,
        distances: np.ndarray | None = None,
    ) -> None:
        self.indices = np.asarray(indices, dtype=np.uint32)
        self.row_ptr = np.asarray(row_ptr, dtype=np.uint32) if row_ptr is not None else None
        self.distances = np.asarray(distances, dtype=np.float64) if distances is not None else None

    def iterate(self) -> t.Iterator[np.ndarray]:
        if self.row_ptr is None:
            for elem in self.indices:
                yield np.array([elem])
        else:
            yield from self._iterate()

    def _iterate(self) -> t.Iterator[np.ndarray]:
        assert self.row_ptr is not None
        for i in range(len(self.row_ptr) - 1):
            start = self.row_ptr[i]
            end = self.row_ptr[i + 1]
            yield self.indices[start:end, ...]


def empty_result(geometry: Geometry) -> QueryResult:
    if geometry.csr:
        row_ptr = np.zeros(len(geometry) + 1, dtype=np.int32)
    else:
        row_ptr = np.zeros(0, dtype=np.int32)

    return QueryResult(
        indices=np.empty(0, dtype=np.int32),
        row_ptr=row_ptr,
        distances=np.empty(0, dtype=np.float64),
    )


OVERLAPS = 1
NEAREST = 2
INTERSECTS = 3
DISTANCE = 4


class GeoQuery:
    """High-performance spatial query engine using R-tree indexing.

    This class creates a spatial index for efficient queries against a target geometry.
    Supports overlap detection, intersection tests, nearest neighbor searches, and
    distance-based queries.

    Args:
        target_geometry: The geometry to index and query against
    """

    _interface: CGeoQuery | None = None

    def __init__(self, target_geometry: Geometry) -> None:
        self._interface = (
            CGeoQuery(*target_geometry.as_c_input()) if len(target_geometry) != 0 else None
        )

    def overlaps_with(self, geometry: Geometry) -> QueryResult:
        """Find all target geometries that overlap with the query geometry.

        Args:
            geometry: Query geometry to test for overlaps

        Returns:
            QueryResult containing indices of overlapping geometries
        """
        return self.query(OVERLAPS, geometry)

    def intersects_with(self, geometry: Geometry) -> QueryResult:
        """Find all target geometries that intersect with the query geometry.

        Args:
            geometry: Query geometry to test for intersections

        Returns:
            QueryResult containing indices of intersecting geometries
        """
        return self.query(INTERSECTS, geometry)

    def nearest_to(self, geometry: Geometry) -> QueryResult:
        """Find the nearest target geometry to each query geometry.

        Args:
            geometry: Query geometry to find nearest neighbors for

        Returns:
            QueryResult containing indices and distances to nearest geometries
        """
        return self.query(NEAREST, geometry)

    def within_distance_of(self, geometry: Geometry, distance: float) -> QueryResult:
        """Find all target geometries within a specified distance of the query geometry.

        Args:
            geometry: Query geometry to search around
            distance: Maximum distance threshold

        Returns:
            QueryResult containing indices of geometries within distance
        """
        return self.query(DISTANCE, geometry, distance=distance)

    def query(self, query_type: int, geometry: Geometry, **kwargs) -> QueryResult:
        if self._interface is None or geometry == 0:
            return empty_result(geometry)

        if query_type == NEAREST:
            distance_result = self._interface.nearest_to(*geometry.as_c_input())
            return QueryResult(
                indices=distance_result.indices(), distances=distance_result.distances()
            )

        elif query_type == OVERLAPS:
            raw = self._interface.overlaps_with(*geometry.as_c_input())
        elif query_type == INTERSECTS:
            raw = self._interface.intersects_with(*geometry.as_c_input())
        elif query_type == DISTANCE:
            distance = kwargs.pop("distance")
            raw = self._interface.within_distance_of(*geometry.as_c_input(), distance)
        else:
            raise ValueError(f"Undefined query type {query_type}")
        return QueryResult(indices=raw.indices(), row_ptr=raw.row_ptr())
