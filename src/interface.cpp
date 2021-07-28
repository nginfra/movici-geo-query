#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl_bind.h>
#include <pybind11/stl.h>

#include <types.hpp>
#include <python_geo_query.hpp>

namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(boost_geo_query::IndexVector);
PYBIND11_MAKE_OPAQUE(boost_geo_query::DistanceVector);

//struct NumpyInput: public boost_geo_query::Input
//{
//
//
//    py::capsule free_when_done(result, [](void *f) {
//      auto foo = reinterpret_cast<std::vector<T> *>(f);
//      delete foo;
//    });
//
//    return py::array_t<T>({result->size()}, // shape
//                          {sizeof(T)},      // stride
//                          result->data(),   // data pointer
//                          free_when_done);
//}

PYBIND11_MODULE(interface, m)
{
    py::bind_vector<boost_geo_query::DistanceVector>(m, "CDistanceVector");
    py::bind_vector<boost_geo_query::IndexVector>(m, "CIndexVector");

    py::class_<boost_geo_query::PythonGeoQuery>(m, "CGeoQuery")
        .def(py::init<
             py::array_t<double, py::array::c_style | py::array::forcecast> &,
             const boost_geo_query::IndexVector &,
             const std::string &>())
        .def("nearest_to", &boost_geo_query::PythonGeoQuery::nearest_to)
        .def("intersects_with", &boost_geo_query::PythonGeoQuery::intersects_with)
        .def("overlaps_with", &boost_geo_query::PythonGeoQuery::overlaps_with)
        .def("within_distance_of", &boost_geo_query::PythonGeoQuery::within_distance_of);

    // py::class_<boost_geo_query::Input>(m, "CInput")
    //     .def(py::init<
    //          py::array_t<double, py::array::c_style | py::array::forcecast> &,
    //          const boost_geo_query::IndexVector &,
    //          const std::string &>());

    //    py::capsule free_when_done(result, [](void *f) {
    //      auto foo = reinterpret_cast<Input *>(f);
    //      delete foo;
    //    });
    //    py::capsule free_when_done(foo, [](void *f) {
    //            double *foo = reinterpret_cast<double *>(f);
    //            std::cerr << "Element [0] = " << foo[0] << "\n";
    //            std::cerr << "freeing memory @ " << f << "\n";
    //            delete[] foo;
    //        });
    py::class_<boost_geo_query::QueryResults>(m, "CQueryResults")
        //        .def("indices", [](const boost_geo_query::QueryResults &qr)
        //            {
        //            return py::array_t<boost_geo_query::Location>(
        //                {qr.indices.size()}, // shape
        //                {sizeof boost_geo_query::Location}, // strides
        //                qr.indices.data(), // the data pointer
        //                free_when_done) ;}) // numpy array references this parent; })
        .def("indices", [](const boost_geo_query::QueryResults &qr)
             { return qr.indices; })
        .def("row_ptr", [](const boost_geo_query::QueryResults &qr)
             { return qr.rowPtr; })
        .def("distances", [](const boost_geo_query::QueryResults &qr)
             { return qr.distances; });
}