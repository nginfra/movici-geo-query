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

        virtual QueryResults nearest_to(const Input &i) = 0;
        virtual QueryResults intersects_with(const Input &i) = 0;
        virtual QueryResults overlaps_with(const Input &i) = 0;
        virtual QueryResults within_distance_of(const Input &i, Distance d) = 0;
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
        QueryResults nearest_to(const Input &input)
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
                throw std::invalid_argument("Input type not supported in BoostGeoQueryWrapper");
            }
        }

        QueryResults intersects_with(const Input &input)
        {
            if (input.type == accepted_input_types::point)
            {
                return _boostQuery.intersects_with(_convert_geometry_for_query<Point>(input));
            }
            else if (input.type == accepted_input_types::linestring)
            {
                return _boostQuery.intersects_with(_convert_geometry_for_query<LineString>(input));
            }
            else if (input.type == accepted_input_types::openPolygon)
            {
                return _boostQuery.intersects_with(_convert_geometry_for_query<OpenPolygon>(input));
            }
            else if (input.type == accepted_input_types::closedPolygon)
            {
                return _boostQuery.intersects_with(_convert_geometry_for_query<ClosedPolygon>(input));
            }
            else
            {
                throw std::invalid_argument("Input type not supported in BoostGeoQueryWrapper");
            }
        }

        QueryResults overlaps_with(const Input &input)
        {
            if (input.type == accepted_input_types::point)
            {
                return _boostQuery.overlaps_with(_convert_geometry_for_query<Point>(input));
            }
            else if (input.type == accepted_input_types::linestring)
            {
                return _boostQuery.overlaps_with(_convert_geometry_for_query<LineString>(input));
            }
            else if (input.type == accepted_input_types::openPolygon)
            {
                return _boostQuery.overlaps_with(_convert_geometry_for_query<OpenPolygon>(input));
            }
            else if (input.type == accepted_input_types::closedPolygon)
            {
                return _boostQuery.overlaps_with(_convert_geometry_for_query<ClosedPolygon>(input));
            }
            else
            {
                throw std::invalid_argument("Input type not supported in BoostGeoQueryWrapper");
            }
        }

        QueryResults within_distance_of(const Input &input, Distance d)
        {
            if (input.type == accepted_input_types::point)
            {
                return _boostQuery.within_distance_of(_convert_geometry_for_query<Point>(input), d);
            }
            else if (input.type == accepted_input_types::linestring)
            {
                return _boostQuery.within_distance_of(_convert_geometry_for_query<LineString>(input), d);
            }
            else if (input.type == accepted_input_types::openPolygon)
            {
                return _boostQuery.within_distance_of(_convert_geometry_for_query<OpenPolygon>(input), d);
            }
            else if (input.type == accepted_input_types::closedPolygon)
            {
                return _boostQuery.within_distance_of(_convert_geometry_for_query<ClosedPolygon>(input), d);
            }
            else
            {
                throw std::invalid_argument("Input type not supported in BoostGeoQueryWrapper");
            }
        }

    private:
        RTreeGeometryQuery<T> _boostQuery;

        template <class U, class = GEO_TYPES<U>>
        std::vector<U> _convert_geometry_for_query(const Input &input)
        {
            auto data = input.xy.data();
            std::vector<U> rv;
            if constexpr (std::is_same_v<U, Point>)
            {
                for (Index i = 0; i < input.length; ++i)
                {
                    rv.push_back(Point(data[i * input.unitSize], data[i * input.unitSize + 1]));
                }
            }
            else if constexpr (std::is_same_v<U, LineString>)
            {
                for (Index i = 0; i < input.rowPtr.size() - 1; ++i)
                {
                    U line;
                    for (Index j = input.rowPtr[i]; j < input.rowPtr[i + 1]; ++j)
                    {
                        line.push_back(Point(data[j * input.unitSize], data[j * input.unitSize + 1]));
                    }
                    rv.push_back(line);
                }
            }
            else if constexpr (std::is_same_v<U, OpenPolygon> || std::is_same_v<U, ClosedPolygon>)
            {
                for (Index i = 0; i < input.rowPtr.size() - 1; ++i)
                {
                    U polygon;
                    for (Index j = input.rowPtr[i]; j < input.rowPtr[i + 1]; ++j)
                    {
                        polygon.outer().push_back(Point(data[j * input.unitSize], data[j * input.unitSize + 1]));
                    }
                    rv.push_back(polygon);
                }
            }
            return rv;
        }
    };

    class PythonGeoQuery
    {
    public:
        PythonGeoQuery(const Input &input);
        ~PythonGeoQuery() {}

        QueryResults nearest_to(const Input &i) { return _query->nearest_to(i); }
        QueryResults intersects_with(const Input &i) { return _query->intersects_with(i); }
        QueryResults overlaps_with(const Input &i) { return _query->overlaps_with(i); }
        QueryResults within_distance_of(const Input &i, Distance d) { return _query->within_distance_of(i, d); }

    private:
        std::unique_ptr<GeoQueryWrapperBase> _query;
    };

}
