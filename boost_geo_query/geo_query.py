import typing as t

from interface import CInput, CIndexVector, CGeoQuery
import numpy as np

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
        self.distances: t.Optional[np.ndarray] = distances

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
    _interface: t.Optional[CGeoQuery] = None

    def __init__(self, target_geometry: Geometry) -> None:
        self._interface = None
        if len(target_geometry) > 0:
            row_ptr = CIndexVector(
                target_geometry.row_ptr if target_geometry.csr else np.array([0], dtype=np.uint32)
            )
            query_input = CInput(target_geometry.points, row_ptr, target_geometry.type)
            self._interface = CGeoQuery(query_input)

    def _query_is_empty(self, target_geometry: Geometry) -> bool:
        return self._interface is None or len(target_geometry) == 0

    #
    # def _query(self, target_geometry: Geometry, method: str) -> QueryResult:
    #     if self._check_query_empty(target_geometry):
    #         return self._empty_result(target_geometry)

    def overlaps_with(self, geometry: Geometry) -> QueryResult:
        if self._query_is_empty(geometry):
            return self._empty_result(geometry)

        intersecting_result = self._interface.overlaps_with(geometry.as_c_input())
        return QueryResult(
            results=intersecting_result.results(), row_ptr=intersecting_result.row_ptr()
        )

    def intersects_with(self, geometry: Geometry) -> QueryResult:
        if self._query_is_empty(geometry):
            return self._empty_result(geometry)

        intersecting_result = self._interface.intersects_with(geometry.as_c_input())
        return QueryResult(
            results=intersecting_result.results(), row_ptr=intersecting_result.row_ptr()
        )

    def nearest_to(self, geometry: Geometry) -> QueryResult:
        if self._query_is_empty(geometry):
            return self._empty_result(geometry)

        distance_result = self._interface.nearest_to(geometry.as_c_input())
        return QueryResult(
            results=distance_result.results(), distances=distance_result.distances()
        )

    def within_distance_of(self, geometry: Geometry, distance: float) -> QueryResult:
        if self._query_is_empty(geometry):
            return self._empty_result(geometry)

        intersecting_result = self._interface.within_distance_of(geometry.as_c_input(), distance)
        return QueryResult(
            results=intersecting_result.results(), row_ptr=intersecting_result.row_ptr()
        )

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
