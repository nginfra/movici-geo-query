#include <types.hpp>
#include <catch.hpp>
#include <boost_geo_query.hpp>

namespace boost_geo_query
{

    SCENARIO("Searching for points in a polygon")
    {
        GIVEN("A set of points in a tree and a polygon")
        {
            PointVector points;
            RTree rtree;

            for (double y = 0.0; y <= 4.0; y++)
            {
                for (double x = 0.0; x <= 4.0; x++)
                {
                    Point p = Point(x, y);
                    Box b;
                    boost::geometry::envelope(p, b);
                    points.push_back(p);
                    rtree.insert(std::make_pair(b, (int)(y * 5 + x)));
                }
            }

            ClosedPolygonVector pv;
            ClosedPolygon p;
            p.outer().push_back(Point(0, 0));
            p.outer().push_back(Point(1, 0));
            p.outer().push_back(Point(1, 1));
            p.outer().push_back(Point(0, 1));
            p.outer().push_back(Point(0, 0));
            pv.push_back(p);

            WHEN("Intersecting is called for a polygon")
            {
                RTreeGeometryQuery<Point> query(points, rtree);
                QueryResults qr = query.intersects_with(pv);

                THEN("intersecting.indices == (0, 1, 5, 6)")
                {
                    IndexVector expected = {0, 1, 5, 6};
                    REQUIRE(qr.indices == expected);
                }

                THEN("intersecting.rowPtr == (0,4)")
                {
                    IndexVector expected = {0, 4};
                    REQUIRE(qr.rowPtr == expected);
                }
            }

            WHEN("Overlapping is called for a polygon")
            {
                RTreeGeometryQuery<Point> query(points, rtree);
                QueryResults qr = query.overlaps_with(pv);

                THEN("intersecting.indices == ()")
                {
                    IndexVector expected = {};
                    REQUIRE(qr.indices == expected);
                }

                THEN("intersecting.rowPtr == (0, 0)")
                {
                    IndexVector expected = {0, 0};
                    REQUIRE(qr.rowPtr == expected);
                }
            }

            WHEN("No tree is given")
            {
                RTreeGeometryQuery<Point> query(points);
                QueryResults qr = query.intersects_with(pv);

                THEN("it is created on the fly and the results are the same")
                {
                    QueryResults expected = RTreeGeometryQuery<Point>(points, rtree).intersects_with(pv);
                    REQUIRE(qr.indices == expected.indices);
                    REQUIRE(qr.rowPtr == expected.rowPtr);
                }
            }
        }
    }

    SCENARIO("Searching for nearest points to a polygon")
    {
        GIVEN("A set of points in a tree and a polygon")
        {
            PointVector points;
            RTree rtree;

            for (double y = 0.0; y <= 4.0; y++)
            {
                for (double x = 0.0; x <= 4.0; x++)
                {
                    Point p = Point(x, y);
                    Box b;
                    boost::geometry::envelope(p, b);
                    points.push_back(p);
                    rtree.insert(std::make_pair(b, (int)(y * 5 + x)));
                }
            }

            ClosedPolygonVector pv;
            ClosedPolygon p;
            p.outer().push_back(Point(1.2, 1.2));
            p.outer().push_back(Point(1.3, 1.2));
            p.outer().push_back(Point(1.3, 1.3));
            p.outer().push_back(Point(1.2, 1.3));
            p.outer().push_back(Point(1.2, 1.2));
            pv.push_back(p);

            WHEN("Nearest is called for a polygon")
            {
                RTreeGeometryQuery<Point> query(points, rtree);
                QueryResults qr = query.nearest_to(pv);

                THEN("nearest.indices == (6)")
                {
                    IndexVector expected = {6};
                    REQUIRE(qr.indices == expected);
                }

                THEN("nearest.distance == (sqrt(2 * 0.2 * 0.2))")
                {
                    Distance expected = sqrt(2 * 0.2 * 0.2);
                    REQUIRE(abs(qr.distances[0] - expected) < (1.0e-6));
                }
            }
        }

        // todo: difficult test where box(geom) is close but geom is further away.
        // but closer than <nb_search_elements> other elements
    }

    SCENARIO("Searching for points within radius of a polygon")
    {
        GIVEN("A set of points in a tree and a polygon")
        {
            PointVector points;
            RTree rtree;

            for (double y = 0.0; y <= 4.0; y++)
            {
                for (double x = 0.0; x <= 4.0; x++)
                {
                    Point p = Point(x, y);
                    Box b;
                    boost::geometry::envelope(p, b);
                    points.push_back(p);
                    rtree.insert(std::make_pair(b, (int)(y * 5 + x)));
                }
            }

            ClosedPolygonVector pv;
            ClosedPolygon p;
            p.outer().push_back(Point(1.9, 1.9));
            p.outer().push_back(Point(2.1, 1.9));
            p.outer().push_back(Point(2.1, 2.1));
            p.outer().push_back(Point(1.9, 2.1));
            p.outer().push_back(Point(1.9, 1.9));
            pv.push_back(p);

            WHEN("find in radius is called for a polygon with radius 0")
            {
                RTreeGeometryQuery<Point> query(points, rtree);
                Distance radius = 0.0;
                QueryResults qr = query.within_distance_of(pv, radius);

                THEN("One point is found: intersecting.indices == (12)")
                {
                    IndexVector expected = {12};
                    REQUIRE(qr.indices == expected);
                }

                THEN("intersecting.rowPtr == (0, 1)")
                {
                    IndexVector expected = {0, 1};
                    REQUIRE(qr.rowPtr == expected);
                }
            }

            WHEN("find in radius is called for a polygon with radius 1")
            {
                RTreeGeometryQuery<Point> query(points, rtree);
                Distance radius = 1.0;
                QueryResults qr = query.within_distance_of(pv, radius);

                THEN("5 points are found: intersecting.indices == (7,11,12,13,17)")
                {
                    IndexVector expected = {7, 11, 12, 13, 17};
                    REQUIRE(qr.indices == expected);
                }

                THEN("intersecting.rowPtr == (0, 5)")
                {
                    IndexVector expected = {0, 5};
                    REQUIRE(qr.rowPtr == expected);
                }
            }
        }
    }

    SCENARIO("Searching for nearest points to some other points")
    {
        GIVEN("A set of points in a tree and some points")
        {
            PointVector points;
            RTree rtree;

            for (double y = 0.0; y <= 4.0; y++)
            {
                for (double x = 0.0; x <= 4.0; x++)
                {
                    Point p = Point(x, y);
                    Box b;
                    boost::geometry::envelope(p, b);
                    points.push_back(p);
                    rtree.insert(std::make_pair(b, (int)(y * 5 + x)));
                }
            }

            PointVector pv;
            pv.push_back(Point(1.2, 1.2));
            pv.push_back(Point(3.0, 1.4));

            WHEN("Nearest is called for the points")
            {
                RTreeGeometryQuery<Point> query(points, rtree);
                QueryResults qr = query.nearest_to(pv);

                THEN("nearest.indices == (6, 8)")
                {
                    IndexVector expected = {6, 8};
                    REQUIRE(qr.indices == expected);
                }

                THEN("nearest.distance == (sqrt(2 * 0.2 * 0.2), 0.4)")
                {
                    REQUIRE(abs(qr.distances[0] - sqrt(2 * 0.2 * 0.2)) < (1.0e-6));
                    REQUIRE(abs(qr.distances[1] - 0.4) < (1.0e-6));
                }
            }
        }
    }

    SCENARIO("Searching for points in multiple polygons")
    {
        GIVEN("A set of points and polygons, some without hits")
        {
            PointVector points;

            for (double y = 0.0; y <= 4.0; y++)
            {
                for (double x = 0.0; x <= 4.0; x++)
                {
                    Point p = Point(x, y);
                    Box b;
                    boost::geometry::envelope(p, b);
                    points.push_back(p);
                }
            }

            ClosedPolygonVector pv;
            for (double y = 0.0; y <= 20.0; y = y + 10)
            {
                for (double x = 0.0; x <= 20.0; x = x + 10)
                {
                    ClosedPolygon p;
                    p.outer().push_back(Point(x, y));
                    p.outer().push_back(Point(x + 1, y));
                    p.outer().push_back(Point(x + 1, y + 1));
                    p.outer().push_back(Point(x, y + 1));
                    p.outer().push_back(Point(x, y));
                    pv.push_back(p);
                }
            }

            WHEN("Intersecting is called for the polygons")
            {
                RTreeGeometryQuery<Point> query(points);
                QueryResults qr = query.intersects_with(pv);

                THEN("Only the first one should have hits: intersecting.indices == (0, 1, 5, 6)")
                {
                    IndexVector expected = {0, 1, 5, 6};
                    REQUIRE(qr.indices == expected);
                }

                THEN("Only the first one should have hits: intersecting.rowPtr == (0,4,4,4,4,4,4,4,4,4)")
                {
                    IndexVector expected = {0, 4, 4, 4, 4, 4, 4, 4, 4, 4};
                    REQUIRE(qr.rowPtr == expected);
                }
            }
        }
    }

    SCENARIO("Searching for points in radius of some other points")
    {
        GIVEN("A set of points and some other points")
        {
            PointVector points;

            for (double y = 0.0; y <= 4.0; y++)
            {
                for (double x = 0.0; x <= 4.0; x++)
                {
                    Point p = Point(x, y);
                    Box b;
                    boost::geometry::envelope(p, b);
                    points.push_back(p);
                }
            }

            PointVector pv;
            pv.push_back(Point(10.0, 10.0));
            pv.push_back(Point(1.5, 1.5));
            pv.push_back(Point(1.0, 1.0));

            WHEN("within_distance_of(0.8) is called for the points")
            {
                RTreeGeometryQuery<Point> query(points);
                QueryResults qr = query.within_distance_of(pv, 0.8);

                THEN("nearest.indices == (6, 7, 11, 12, 6)")
                {
                    IndexVector expected = {6, 7, 11, 12, 6};
                    REQUIRE(qr.indices == expected);
                }

                THEN("intersecting.rowPtr == (0, 0, 4, 5)")
                {
                    IndexVector expected = {0, 0, 4, 5};
                    REQUIRE(qr.rowPtr == expected);
                }
            }
        }
    }

}
