import typing as t

import numpy as np
import BoostGeoQueryPythonInterface as Interface

from .geometry import Geometry


class QueryResult:
    def __init__(
            self,
            results: np.ndarray,
            row_ptr: t.Optional[np.ndarray] = None,
            distances: t.Optional[np.ndarray] = None,
    ) -> None:
        self.results: np.ndarray = results
        self.row_ptr: t.Optional[np.ndarray] = row_ptr
        self.dist: t.Optional[np.ndarray] = distances

    def iterate(self) -> t.Iterator[np.ndarray]:
        if self.row_ptr is None:
            for elem in self.results:
                yield np.array([elem])
        else:
            yield from self._iterate()

    def _iterate(self) -> t.Iterator[np.ndarray]:
        for i in range(len(self.row_ptr) - 1):
            start = self.row_ptr[i]
            end = self.row_ptr[i + 1]
            yield self.results[start:end, ...]


class GeoQuery:
    _interface: t.Optional[Interface] = None

    def __init__(
            self, target_geometry: Geometry
    ) -> None:
        self._interface = None
        if len(target_geometry) > 0:
            self._interface = Interface(target_geometry)

    def _query_is_empty(self, target_geometry: Geometry) -> bool:
        return self._interface is None or len(target_geometry) == 0

    #
    # def _query(self, target_geometry: Geometry, method: str) -> QueryResult:
    #     if self._check_query_empty(target_geometry):
    #         return self._empty_result(target_geometry)

    def overlaps_with(self, geometry: Geometry) -> QueryResult:
        if self._query_is_empty(geometry):
            return self._empty_result(geometry)

        results, row_ptr = self._interface.find_overlapping(geometry)
        return QueryResult(results=results, row_ptr=row_ptr)

    def intersect_with(self, geometry: Geometry) -> QueryResult:
        if self._query_is_empty(geometry):
            return self._empty_result(geometry)

        results, row_ptr = self._interface.find_intersecting(geometry)
        return QueryResult(results=results, row_ptr=row_ptr)

    def nearest_to(self, geometry: Geometry) -> QueryResult:
        if self._query_is_empty(geometry):
            return self._empty_result(geometry)

        results, dist = self._interface.find_nearest(geometry)
        return QueryResult(results=results, distances=dist)

    def within_distance_of(
            self, geometry: Geometry, distance: float
    ) -> QueryResult:
        if self._query_is_empty(geometry):
            return self._empty_result(geometry)

        results, row_ptr = self._interface.find_in_radius(geometry, distance)
        return QueryResult(results=results, row_ptr=row_ptr)

    @staticmethod
    def _empty_result(target_geometry: Geometry) -> QueryResult:
        if target_geometry.csr:
            row_ptr = np.zeros(len(target_geometry) + 1, dtype=np.int32)
        else:
            row_ptr = np.zeros(0, dtype=np.int32)

        return QueryResult(
            results=np.empty(0, dtype=np.int32),
            row_ptr=row_ptr,
            distances=np.empty(0, dtype=np.float64),
        )
