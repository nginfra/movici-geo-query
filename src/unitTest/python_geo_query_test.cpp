#include <types.hpp>
#include <unitTest/catch.hpp>
#include <python_geo_query.hpp>

namespace boost_geo_query
{

   SCENARIO("Using python input converter for points->nearest(points)")
    {
        GIVEN("A set of points in a tree and some points")
        {
            InputPointVector points;
            RTree rtree;

            for (double y = 0.0; y <= 4.0; y++)
            {
                for (double x = 0.0; x <= 4.0; x++)
                {
                    Point p = Point(x, y);
                    Box b;
                    boost::geometry::envelope(p, b);
                    points.push_back(InputPoint{x,y});
                    rtree.insert(std::make_pair(b, (int)(y * 5 + x)));
                }
            }

            Input input {points, IndexVector(), "point"};

            InputPointVector pv;
            pv.push_back(InputPoint{1.2, 1.2});
            pv.push_back(InputPoint{3.0, 1.4});

            WHEN("Nearest is called for the points")
            {
                PythonGeoQuery query(input);
                Input input2 {pv, IndexVector(), "point"};
                DistanceResults dr = query.nearest_to(input2);

                THEN("nearest.results == (6, 8)")
                {
                    IndexVector expected = {6, 8};
                    REQUIRE(dr.results == expected);
                }

                THEN("nearest.distance == (sqrt(2 * 0.2 * 0.2), 0.4)")
                {
                    REQUIRE(abs(dr.distances[0] - sqrt(2 * 0.2 * 0.2)) < (1.0e-6));
                    REQUIRE(abs(dr.distances[1] - 0.4) < (1.0e-6));
                }
            }
        }
    }

    SCENARIO("Using python input converter for points->nearest(polygons)")
    {
        GIVEN("A set of points in a tree and some polygon")
        {
            InputPointVector points;
            RTree rtree;

            for (double y = 0.0; y <= 4.0; y++)
            {
                for (double x = 0.0; x <= 4.0; x++)
                {
                    Point p = Point(x, y);
                    Box b;
                    boost::geometry::envelope(p, b);
                    points.push_back(InputPoint{x,y});
                    rtree.insert(std::make_pair(b, (int)(y * 5 + x)));
                }
            }

            Input input {points, IndexVector(), "point"};

            InputPointVector polygon;
            polygon.push_back(InputPoint{1.2, 1.2});
            polygon.push_back(InputPoint{1.3, 1.2});
            polygon.push_back(InputPoint{1.3, 1.3});
            polygon.push_back(InputPoint{1.2, 1.3});
            polygon.push_back(InputPoint{1.2, 1.2});

            WHEN("Nearest is called for a polygon")
            {
                PythonGeoQuery query(input);
                Input input2 {polygon, IndexVector({0, 5}), "closed_polygon"};
                DistanceResults dr = query.nearest_to(input2);

                THEN("nearest.results == (6)")
                {
                    IndexVector expected = {6};
                    REQUIRE(dr.results == expected);
                }

                THEN("nearest.distance == (sqrt(2 * 0.2 * 0.2))")
                {
                    Distance expected = sqrt(2 * 0.2 * 0.2);
                    REQUIRE(abs(dr.distances[0] - expected) < (1.0e-6));
                }
            }
        }
    }

}
