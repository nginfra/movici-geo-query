#pragma once

#include <types.hpp>
#include <boost_geo_query.hpp>

namespace boost_geo_query
{
    class GeoQueryWrapperBase
    {
    public:
        GeoQueryWrapperBase() = default;
        virtual ~GeoQueryWrapperBase() = default;

        virtual DistanceResults nearest_to(const Input &i) = 0;
        // virtual IntersectingResults intersects_with(const Input &i) = 0;
        // virtual IntersectingResults overlaps_with(const Input &i) = 0;
        // virtual IntersectingResults within_distance_of(const Input &i, Distance d) = 0;
    };

    template <class T, class = GEO_TYPES<T>>
    class BoostGeoQueryWrapper : public GeoQueryWrapperBase
    {
    public:
        using TargetVector = std::vector<T>;
        BoostGeoQueryWrapper(const Input &input)
        {
            _boostQuery = RTreeGeometryQuery<T>(_convert_geometry_for_query<T>(input));
        }

        // Given unknown type, we need to figure out type and call convert<T> and function<T>
        DistanceResults nearest_to(const Input &input)
        {
            if (input.type == accepted_input_types::point)
            {
                return _boostQuery.nearest_to(_convert_geometry_for_query<Point>(input));
            }
            else if (input.type == accepted_input_types::linestring)
            {
                return _boostQuery.nearest_to(_convert_geometry_for_query<LineString>(input));
            }
            else if (input.type == accepted_input_types::openPolygon)
            {
                return _boostQuery.nearest_to(_convert_geometry_for_query<OpenPolygon>(input));
            }
            else if (input.type == accepted_input_types::closedPolygon)
            {
                return _boostQuery.nearest_to(_convert_geometry_for_query<ClosedPolygon>(input));
            }
            else
            {
                throw std::invalid_argument("Input type unknown in BoostGeoQueryWrapper");
            }
        }

        // DistanceResults nearest_to(const Input &i) { return _boostQuery.nearest_to(_convert_geometry_for_query(i)); }
        // IntersectingResults intersects_with(const Input &i) { return _boostQuery.intersects_with(_convert_geometry_for_query(i)); }
        // IntersectingResults overlaps_with(const Input &i) { return _boostQuery.overlaps_with(_convert_geometry_for_query(i)); }
        // IntersectingResults within_distance_of(const Input &i, Distance d) { return _boostQuery.within_distance_of(_convert_geometry_for_query(i), d); }

    private:
        RTreeGeometryQuery<T> _boostQuery;

        template <class U, class = GEO_TYPES<U>>
        std::vector<U> _convert_geometry_for_query(const Input &input)
        {
            std::vector<U> rv;
            if constexpr (std::is_same_v<U, Point>)
            {
                auto data = input.xy.data();
                for (Index i = 0; i < input.length; ++i)
                {
                    rv.push_back(Point(data[i * input.unitSize], data[i * input.unitSize + 1]));
                }
            }
            //            else if constexpr (std::is_same_v<U, LineString>) //todo
            //            {
            //                for (Index i = 0; i < input.rowPtr.size() - 1; ++i)
            //                {
            //                    U line;
            //                    for (Index j = input.rowPtr[i]; j < input.rowPtr[i + 1]; ++j)
            //                    {
            //                        line.push_back(Point(input.points[j].x, input.points[j].y));
            //                    }
            //                    rv.push_back(line);
            //                }
            //            }
            //            else if constexpr (std::is_same_v<U, OpenPolygon> || std::is_same_v<U, ClosedPolygon>)
            //            {
            //                for (Index i = 0; i < input.rowPtr.size() - 1; ++i)
            //                {
            //                    U polygon;
            //                    for (Index j = input.rowPtr[i]; j < input.rowPtr[i + 1]; ++j)
            //                    {
            //                        polygon.outer().push_back(Point(input.points[j].x, input.points[j].y));
            //                    }
            //                    rv.push_back(polygon);
            //                }
            //            }
            return rv;
        }
    };

    class PythonGeoQuery
    {
    public:
        PythonGeoQuery(const Input &input);
        ~PythonGeoQuery() {}

        DistanceResults nearest_to(const Input &i) { return _query->nearest_to(i); }
        // IntersectingResults intersects_with(const Input &i) { return _query->intersects_with(i); }
        // IntersectingResults overlaps_with(const Input &i) { return _query->overlaps_with(i); }
        // IntersectingResults within_distance_of(const Input &i, Distance d) { return _query->within_distance_of(i, d); }

    private:
        std::unique_ptr<GeoQueryWrapperBase> _query;
    };

}
