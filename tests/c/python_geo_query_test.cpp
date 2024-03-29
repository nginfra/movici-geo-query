#include <types.hpp>
#include <catch.hpp>
#include <python_geo_query.hpp>

namespace boost_geo_query
{

    SCENARIO("Using python input converter for points->nearest(points)")
    {
        GIVEN("A set of points in a tree and some points")
        {
            LocationArray points;
            RTree rtree;

            for (double y = 0.0; y <= 4.0; y++)
            {
                for (double x = 0.0; x <= 4.0; x++)
                {
                    Point p = Point(x, y);
                    Box b;
                    boost::geometry::envelope(p, b);
                    points.push_back(x);
                    points.push_back(y);
                    rtree.insert(std::make_pair(b, (int)(y * 5 + x)));
                }
            }

            LocationArray pv;
            pv.push_back(1.2);
            pv.push_back(1.2);

            pv.push_back(3.0);
            pv.push_back(1.4);

            WHEN("Nearest is called for the points")
            {
                PythonGeoQuery query(points, IndexVector(), "point");
                QueryResults qr = query.nearest_to(pv, IndexVector(), "point");

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

    SCENARIO("Using python input converter for points->nearest(polygons)")
    {
        GIVEN("A set of points in a tree and some polygon")
        {
            LocationArray points;
            RTree rtree;

            for (double y = 0.0; y <= 4.0; y++)
            {
                for (double x = 0.0; x <= 4.0; x++)
                {
                    Point p = Point(x, y);
                    Box b;
                    boost::geometry::envelope(p, b);
                    points.push_back(x);
                    points.push_back(y);
                    rtree.insert(std::make_pair(b, (int)(y * 5 + x)));
                }
            }

            LocationArray polygon;
            polygon.push_back(1.2);
            polygon.push_back(1.2);

            polygon.push_back(1.3);
            polygon.push_back(1.2);

            polygon.push_back(1.3);
            polygon.push_back(1.3);

            polygon.push_back(1.2);
            polygon.push_back(1.3);

            polygon.push_back(1.2);
            polygon.push_back(1.2);

            WHEN("Nearest is called for a polygon")
            {
                PythonGeoQuery query(points, IndexVector(), "point");
                QueryResults qr = query.nearest_to(polygon, IndexVector({0, 5}), "closed_polygon");

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
    }

}
