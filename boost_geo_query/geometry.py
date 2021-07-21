from abc import abstractmethod

import numpy as np
import typing as t


class Geometry:
    points: np.ndarray
    row_ptr: t.Optional[np.ndarray]
    type: str
    csr: bool = True

    def __init__(self, points, row_ptr=None):
        self.points = np.array(points)
        self.row_ptr = row_ptr

        self._verify()

    def __len__(self):
        if not self.csr:
            return self.points.shape[0]
        if self.row_ptr.size > 1:
            return self.row_ptr.size - 1
        return 0

    @abstractmethod
    def _verify(self):
        raise NotImplementedError


class PointGeometry(Geometry):
    type = "point"
    csr = False

    def _verify(self):
        if self.row_ptr is not None:
            raise ValueError("PointGeometry can't have row_ptr")


class LinestringGeometry(Geometry):
    type = "linestring"

    def _verify(self):
        if self.row_ptr is None:
            raise ValueError("LinestringGeometry needs row_ptr")


class OpenPolygonGeometry(Geometry):
    type = "open_polygon"

    def _verify(self):
        if self.row_ptr is None:
            raise ValueError("OpenPolygonGeometry needs row_ptr")


class ClosedPolygonGeometry(Geometry):
    type = "closed_polygon"

    def _verify(self):
        if self.row_ptr is None:
            raise ValueError("ClosedPolygonGeometry needs row_ptr")
