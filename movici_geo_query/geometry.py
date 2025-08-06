"""Geometry classes for spatial query operations."""

import typing as t
from abc import abstractmethod

import numpy as np


class Geometry:
    """Base class for all geometry types.

    Attributes:
        points: NumPy array of coordinate points
        row_ptr: Optional CSR-style row pointer for grouped geometries
        type: String identifier for the geometry type
        csr: Whether this geometry uses CSR (Compressed Sparse Row) format
    """

    points: np.ndarray
    row_ptr: t.Optional[np.ndarray]
    type: str
    csr: bool = True

    def __init__(self, points, row_ptr=None):
        self.points = np.asarray(points)
        self.row_ptr = np.asarray(row_ptr) if row_ptr is not None else None

        self._verify()

    def __len__(self):
        if not self.csr:
            return self.points.shape[0]
        return self.row_ptr.size - 1

    @abstractmethod
    def _verify(self):
        raise NotImplementedError

    def as_c_input(self) -> t.Tuple[np.ndarray, np.ndarray, str]:
        row_ptr = self.row_ptr if self.csr else np.array([0], dtype=np.uint32)
        return self.points, row_ptr, self.type


class PointGeometry(Geometry):
    """Represents a collection of point geometries.

    Points are stored as a simple array of (x, y) coordinates without CSR grouping.
    """

    type = "point"
    csr = False

    def _verify(self):
        if self.row_ptr is not None:
            raise ValueError("PointGeometry can't have row_ptr")


class LinestringGeometry(Geometry):
    """Represents a collection of linestring geometries.

    Linestrings are stored in CSR format where row_ptr indicates the start/end
    of each linestring's points in the points array.
    """

    type = "linestring"

    def _verify(self):
        if self.row_ptr is None:
            raise ValueError("LinestringGeometry needs row_ptr")


class OpenPolygonGeometry(Geometry):
    """Represents a collection of open polygon geometries (polylines).

    Open polygons are linestrings that are not closed. Stored in CSR format.
    """

    type = "open_polygon"

    def _verify(self):
        if self.row_ptr is None:
            raise ValueError("OpenPolygonGeometry needs row_ptr")


class ClosedPolygonGeometry(Geometry):
    """Represents a collection of closed polygon geometries.

    Closed polygons form complete loops. The last point connects back to the first.
    Stored in CSR format.
    """

    type = "closed_polygon"

    def _verify(self):
        if self.row_ptr is None:
            raise ValueError("ClosedPolygonGeometry needs row_ptr")
