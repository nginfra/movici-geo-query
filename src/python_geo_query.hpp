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

        virtual QueryResults nearest_to(const LocationArray &xy, const IndexArray &rowPtr, const std::string &type) = 0;
        virtual QueryResults intersects_with(const LocationArray &xy, const IndexArray &rowPtr, const std::string &type) = 0;
        virtual QueryResults overlaps_with(const LocationArray &xy, const IndexArray &rowPtr, const std::string &type) = 0;
        virtual QueryResults within_distance_of(const LocationArray &xy, const IndexArray &rowPtr, const std::string &type, Distance d) = 0;
    };

    template <class T, class = GEO_TYPES<T>>
    class BoostGeoQueryWrapper : public GeoQueryWrapperBase
    {
    public:
        using TargetVector = std::vector<T>;
        BoostGeoQueryWrapper(const LocationArray &xy, const IndexArray &rowPtr)
        {
            _boostQuery = RTreeGeometryQuery<T>(_convert_geometry_for_query<T>(xy, rowPtr));
        }

        // Given unknown type, we need to figure out type and call convert<T> and function<T>
        QueryResults nearest_to(const LocationArray &xy, const IndexArray &rowPtr, const std::string &type)
        {
            if (type == accepted_input_types::point)
            {
                return _boostQuery.nearest_to(_convert_geometry_for_query<Point>(xy, rowPtr));
            }
            else if (type == accepted_input_types::linestring)
            {
                return _boostQuery.nearest_to(_convert_geometry_for_query<LineString>(xy, rowPtr));
            }
            else if (type == accepted_input_types::openPolygon)
            {
                return _boostQuery.nearest_to(_convert_geometry_for_query<OpenPolygon>(xy, rowPtr));
            }
            else if (type == accepted_input_types::closedPolygon)
            {
                return _boostQuery.nearest_to(_convert_geometry_for_query<ClosedPolygon>(xy, rowPtr));
            }
            else
            {
                throw std::invalid_argument("Input type not supported in BoostGeoQueryWrapper");
            }
        }

        QueryResults intersects_with(const LocationArray &xy, const IndexArray &rowPtr, const std::string &type)
        {
            if (type == accepted_input_types::point)
            {
                return _boostQuery.intersects_with(_convert_geometry_for_query<Point>(xy, rowPtr));
            }
            else if (type == accepted_input_types::linestring)
            {
                return _boostQuery.intersects_with(_convert_geometry_for_query<LineString>(xy, rowPtr));
            }
            else if (type == accepted_input_types::openPolygon)
            {
                return _boostQuery.intersects_with(_convert_geometry_for_query<OpenPolygon>(xy, rowPtr));
            }
            else if (type == accepted_input_types::closedPolygon)
            {
                return _boostQuery.intersects_with(_convert_geometry_for_query<ClosedPolygon>(xy, rowPtr));
            }
            else
            {
                throw std::invalid_argument("Input type not supported in BoostGeoQueryWrapper");
            }
        }

        QueryResults overlaps_with(const LocationArray &xy, const IndexArray &rowPtr, const std::string &type)
        {
            if (type == accepted_input_types::point)
            {
                return _boostQuery.overlaps_with(_convert_geometry_for_query<Point>(xy, rowPtr));
            }
            else if (type == accepted_input_types::linestring)
            {
                return _boostQuery.overlaps_with(_convert_geometry_for_query<LineString>(xy, rowPtr));
            }
            else if (type == accepted_input_types::openPolygon)
            {
                return _boostQuery.overlaps_with(_convert_geometry_for_query<OpenPolygon>(xy, rowPtr));
            }
            else if (type == accepted_input_types::closedPolygon)
            {
                return _boostQuery.overlaps_with(_convert_geometry_for_query<ClosedPolygon>(xy, rowPtr));
            }
            else
            {
                throw std::invalid_argument("Input type not supported in BoostGeoQueryWrapper");
            }
        }

        QueryResults within_distance_of(const LocationArray &xy, const IndexArray &rowPtr, const std::string &type, Distance d)
        {
            if (type == accepted_input_types::point)
            {
                return _boostQuery.within_distance_of(_convert_geometry_for_query<Point>(xy, rowPtr), d);
            }
            else if (type == accepted_input_types::linestring)
            {
                return _boostQuery.within_distance_of(_convert_geometry_for_query<LineString>(xy, rowPtr), d);
            }
            else if (type == accepted_input_types::openPolygon)
            {
                return _boostQuery.within_distance_of(_convert_geometry_for_query<OpenPolygon>(xy, rowPtr), d);
            }
            else if (type == accepted_input_types::closedPolygon)
            {
                return _boostQuery.within_distance_of(_convert_geometry_for_query<ClosedPolygon>(xy, rowPtr), d);
            }
            else
            {
                throw std::invalid_argument("Input type not supported in BoostGeoQueryWrapper");
            }
        }

    private:
        RTreeGeometryQuery<T> _boostQuery;

        template <class U, class = GEO_TYPES<U>>
        std::vector<U> _convert_geometry_for_query(const LocationArray &xy, const IndexArray &rowPtr)
        {
#ifdef NOPYBIND
               // assume 2D arrays
               Index unitSize = 2;
               Index length = xy.size()/2;
#else
               // check input dimensions
               if (xy.ndim() != 2)
                    throw std::runtime_error("Input should be 2-D NumPy array");
               if (xy.shape()[1] != 2 && xy.shape()[1] != 3)
                    throw std::runtime_error("Input should have size [N,2] or [N,3]");
               if (rowPtr.ndim() != 1)
                    throw std::runtime_error("IndexArray should be 1-D NumPy array");

               Index unitSize = xy.shape()[1];
               Index length = xy.shape()[0];
#endif

            auto data = xy.data();
            std::vector<U> rv;
            if constexpr (std::is_same_v<U, Point>)
            {
                for (Index i = 0; i < length; ++i)
                {
                    rv.push_back(Point(data[i * unitSize], data[i * unitSize + 1]));
                }
            }
            else if constexpr (std::is_same_v<U, LineString>)
            {
                for (Index i = 0; i < rowPtr.size() - 1; ++i)
                {
                    U line;
                    for (Index j = rowPtr.data()[i]; j < rowPtr.data()[i + 1]; ++j)
                    {
                        line.push_back(Point(data[j * unitSize], data[j * unitSize + 1]));
                    }
                    rv.push_back(line);
                }
            }
            else if constexpr (std::is_same_v<U, OpenPolygon> || std::is_same_v<U, ClosedPolygon>)
            {
                for (Index i = 0; i < rowPtr.size() - 1; ++i)
                {
                    U polygon;
                    for (Index j = rowPtr.data()[i]; j < rowPtr.data()[i + 1]; ++j)
                    {
                        polygon.outer().push_back(Point(data[j * unitSize], data[j * unitSize + 1]));
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
        PythonGeoQuery(const LocationArray &xy, const IndexArray &rowPtr, const std::string &type);
        ~PythonGeoQuery() {}

        QueryResults nearest_to(const LocationArray &xy, const IndexArray &rowPtr, const std::string &type) { return _query->nearest_to(xy, rowPtr, type); }
        QueryResults intersects_with(const LocationArray &xy, const IndexArray &rowPtr, const std::string &type) { return _query->intersects_with(xy, rowPtr, type); }
        QueryResults overlaps_with(const LocationArray &xy, const IndexArray &rowPtr, const std::string &type) { return _query->overlaps_with(xy, rowPtr, type); }
        QueryResults within_distance_of(const LocationArray &xy, const IndexArray &rowPtr, const std::string &type, Distance d) { return _query->within_distance_of(xy, rowPtr, type, d); }

    private:
        std::unique_ptr<GeoQueryWrapperBase> _query;
    };

}
