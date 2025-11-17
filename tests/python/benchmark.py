import contextlib
import logging
import tracemalloc
from timeit import default_timer as timer

import numpy as np

from movici_geo_query.geo_query import GeoQuery
from movici_geo_query.geometry import PointGeometry


def get_grid_points(nb_x_elem=1000, nb_y_elem=1000):
    return PointGeometry(points=np.mgrid[0:nb_x_elem, 0:nb_y_elem].reshape(2, -1).T)


def in_radius(ds1, ds2):
    GeoQuery(ds1).within_distance_of(ds2, 1.0)


def nearest(ds1, ds2):
    GeoQuery(ds1).nearest_to(ds2)


def run(func, nb_runs=10, nb_points_sqrt=1000):
    print(f"Testing runtime for {nb_points_sqrt} X {nb_points_sqrt} grid")
    total = 0
    points = get_grid_points(nb_points_sqrt)
    for i in range(nb_runs):
        start = timer()
        func(points, points)
        end = timer()
        time_passed = end - start
        total += time_passed
        print(f"run: {time_passed}")
    print(f"avg: {total / nb_runs}")


@contextlib.contextmanager
def trace(enable=True):
    if not enable:
        yield
        return
    tracemalloc.start()
    tracemalloc.start()
    snapshot1 = tracemalloc.take_snapshot()
    yield

    snapshot2 = tracemalloc.take_snapshot()

    top_stats = snapshot2.compare_to(snapshot1, "lineno")

    print("[ Top 10 differences ]")
    for stat in top_stats[:10]:
        print(stat)


if __name__ == "__main__":
    TRACE = True
    with trace(TRACE):
        print(f"-- nearest_to --")
        run(nearest, 10, 10)
        print(f"-- within_distance_of --")
        run(in_radius, 10, 10)
