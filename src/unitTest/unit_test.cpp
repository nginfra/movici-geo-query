#include <types.hpp>
#include <unitTest/catch.hpp>
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
                RTreeGeometryQuery query(pv, points, rtree);
                IntersectingResults ir = query.find_intersecting();

                THEN("intersecting.results == (0, 1, 5, 6)")
                {
                    IndexVector expected = {0, 1, 5, 6};
                    REQUIRE(ir.results == expected);
                }

                THEN("intersecting.idxPointer == (0,4)")
                {
                    IndexVector expected = {0, 4};
                    REQUIRE(ir.idxPointer == expected);
                }
            }

            WHEN("Overlapping is called for a polygon")
            {
                RTreeGeometryQuery query(pv, points, rtree);
                IntersectingResults ir = query.find_overlapping();

                THEN("intersecting.results == ()")
                {
                    IndexVector expected = {};
                    REQUIRE(ir.results == expected);
                }

                THEN("intersecting.idxPointer == (0)")
                {
                    IndexVector expected = {0};
                    REQUIRE(ir.idxPointer == expected);
                }
            }

            WHEN("No tree is given")
            {
                RTreeGeometryQuery query(pv, points);
                IntersectingResults ir = query.find_intersecting();

                THEN("it is created on the fly and the results are the same")
                {
                    IntersectingResults expected = RTreeGeometryQuery(pv, points, rtree).find_intersecting();
                    REQUIRE(ir.results == expected.results);
                    REQUIRE(ir.idxPointer == expected.idxPointer);
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
                RTreeGeometryQuery query(pv, points, rtree);
                DistanceResults dr = query.find_nearest();

                THEN("nearest.results == (6)")
                {
                    IndexVector expected = {6};
                    REQUIRE(dr.results == expected);
                }

                THEN("nearest.distance == (0,4)")
                {
                    Distance expected = sqrt(2 * 0.2 * 0.2);
                    REQUIRE(abs(dr.distances[0] - expected) < (1.0e-6));
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
                RTreeGeometryQuery query(pv, points, rtree);
                Distance radius = 0.0;
                IntersectingResults ir = query.find_in_radius(radius);

                THEN("One point is found: intersecting.results == (12)")
                {
                    IndexVector expected = {12};
                    REQUIRE(ir.results == expected);
                }

                THEN("intersecting.idxPointer == (0, 1)")
                {
                    IndexVector expected = {0, 1};
                    REQUIRE(ir.idxPointer == expected);
                }
            }

            WHEN("find in radius is called for a polygon with radius 1")
            {
                RTreeGeometryQuery query(pv, points, rtree);
                Distance radius = 1.0;
                IntersectingResults ir = query.find_in_radius(radius);

                THEN("5 points are found: intersecting.results == (7,11,12,13,17)")
                {
                    IndexVector expected = {7, 11, 12, 13, 17};
                    REQUIRE(ir.results == expected);
                }

                THEN("intersecting.idxPointer == (0, 5)")
                {
                    IndexVector expected = {0, 5};
                    REQUIRE(ir.idxPointer == expected);
                }
            }
        }
    }

}
