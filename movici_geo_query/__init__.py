"""Movici Geo Query - High-performance geospatial queries powered by Boost.Geometry.

This package provides efficient spatial query operations for large-scale geospatial data,
including overlap detection, intersection tests, nearest neighbor searches, and distance queries.
"""

from .geo_query import GeoQuery, QueryResult
from .geometry import (
    Geometry,
    PointGeometry,
    LinestringGeometry,
    OpenPolygonGeometry,
    ClosedPolygonGeometry,
)

__all__ = [
    "GeoQuery",
    "Geometry",
    "QueryResult",
    "PointGeometry",
    "LinestringGeometry",
    "OpenPolygonGeometry",
    "ClosedPolygonGeometry",
]
