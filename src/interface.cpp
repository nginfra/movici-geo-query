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
        .def(py::init<const boost_geo_query::Input &>())
        .def("nearest_to", &boost_geo_query::PythonGeoQuery::nearest_to)
        .def("intersects_with", &boost_geo_query::PythonGeoQuery::intersects_with)
        .def("overlaps_with", &boost_geo_query::PythonGeoQuery::overlaps_with)
        .def("within_distance_of", &boost_geo_query::PythonGeoQuery::within_distance_of);

    py::class_<boost_geo_query::Input>(m, "CInput")
        .def(py::init<
             py::array_t<double, py::array::c_style | py::array::forcecast> &,
             const boost_geo_query::IndexVector &,
             const std::string &>());

    // todo maybe return as nparray?
    py::class_<boost_geo_query::DistanceResults>(m, "CDistanceResults")
        .def("results", [](const boost_geo_query::DistanceResults &dr)
             { return dr.results; })
        .def("distances", [](const boost_geo_query::DistanceResults &dr)
             { return dr.distances; });

    // todo maybe return as nparray?
    py::class_<boost_geo_query::IntersectingResults>(m, "CIntersectingResults")
        .def("results", [](const boost_geo_query::IntersectingResults &ir)
             { return ir.results; })
        .def("row_ptr", [](const boost_geo_query::IntersectingResults &ir)
             { return ir.rowPtr; });
}
