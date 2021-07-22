from timeit import default_timer as timer

import numpy as np
from boost_geo_query.geo_query import GeoQuery
from boost_geo_query.geometry import (
    PointGeometry,
)


def get_grid_points(nb_x_elem=1000, nb_y_elem=1000):
    return PointGeometry(points=np.mgrid[0:nb_x_elem, 0:nb_y_elem].reshape(2, -1).T)


def in_radius(ds1, ds2):
    GeoQuery(ds1).within_distance_of(ds2, 1.0)


def nearest(ds1, ds2):
    GeoQuery(ds1).nearest_to(ds2)


if __name__ == "__main__":
    total = 0
    nb_runs = 10
    nb_points_sqrt = 1000
    points = get_grid_points(nb_points_sqrt)

    print(f" Testing runtime for {nb_points_sqrt} X {nb_points_sqrt} grid")
    print(f"-- nearest_to --")

    for i in range(nb_runs):
        start = timer()
        nearest(points, points)
        end = timer()
        time_passed = end - start
        total += time_passed
        print(f"run: {time_passed}")
    print(f"avg: {total / nb_runs}")

    print(f"-- within_distance_of --")
    for i in range(nb_runs):
        start = timer()
        in_radius(points, points)
        end = timer()
        time_passed = end - start
        total += time_passed
        print(f"run: {time_passed}")
    print(f"avg: {total / nb_runs}")
