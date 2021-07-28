#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl_bind.h>
#include <pybind11/stl.h>

#include <types.hpp>
#include <python_geo_query.hpp>

namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(boost_geo_query::IndexVector);
PYBIND11_MAKE_OPAQUE(boost_geo_query::DistanceVector);


PYBIND11_MODULE(interface, m)
{
    py::bind_vector<boost_geo_query::DistanceVector>(m, "CDistanceVector");
    py::bind_vector<boost_geo_query::IndexVector>(m, "CIndexVector");

    py::class_<boost_geo_query::PythonGeoQuery>(m, "CGeoQuery")
        .def(py::init<
             py::array_t<boost_geo_query::Location, py::array::c_style | py::array::forcecast> &,
             py::array_t<boost_geo_query::Index, py::array::c_style | py::array::forcecast> &,
             const std::string &>())
        .def("nearest_to", &boost_geo_query::PythonGeoQuery::nearest_to)
        .def("intersects_with", &boost_geo_query::PythonGeoQuery::intersects_with)
        .def("overlaps_with", &boost_geo_query::PythonGeoQuery::overlaps_with)
        .def("within_distance_of", &boost_geo_query::PythonGeoQuery::within_distance_of);


    // Todo: return as numpy arrays?
    py::class_<boost_geo_query::QueryResults>(m, "CQueryResults")
        .def("indices", [](const boost_geo_query::QueryResults &qr)
             { return qr.indices; })
        .def("row_ptr", [](const boost_geo_query::QueryResults &qr)
             { return qr.rowPtr; })
        .def("distances", [](const boost_geo_query::QueryResults &qr)
             { return qr.distances; });
}