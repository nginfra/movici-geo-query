#include <types.hpp>
#include <python_geo_query.hpp>

namespace boost_geo_query
{

    PythonGeoQuery::PythonGeoQuery(const Input &input)
    {
        if (input.type == accepted_input_types::point)
        {
            _query = std::make_unique<BoostGeoQueryWrapper<Point>>(input);
        }
        else if (input.type == accepted_input_types::linestring)
        {
            _query = std::make_unique<BoostGeoQueryWrapper<LineString>>(input);
        }
        else if (input.type == accepted_input_types::openPolygon)
        {
            _query = std::make_unique<BoostGeoQueryWrapper<OpenPolygon>>(input);
        }
        else if (input.type == accepted_input_types::closedPolygon)
        {
            _query = std::make_unique<BoostGeoQueryWrapper<ClosedPolygon>>(input);
        }
        else
        {
            throw std::invalid_argument("Input type unknown");
        }
    }

}