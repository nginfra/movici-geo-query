#pragma once

#include <boost/geometry.hpp>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>

#include <vector>

#ifndef NOPYBIND
     #include <pybind11/numpy.h>
     #include <pybind11/pybind11.h>
#endif


namespace boost_geo_query
{
     using Location = double;
     using Index = uint32_t;
#ifdef NOPYBIND
     using LocationArray = std::vector<Location>;
     using IndexArray = std::vector<Index>;
#else
     namespace py = pybind11;
     using LocationArray = py::array_t<Location, py::array::c_style | py::array::forcecast>;
     using IndexArray = py::array_t<Index, py::array::c_style | py::array::forcecast>;
#endif

     namespace bg = boost::geometry;
     namespace bgi = bg::index;
     namespace bgm = bg::model;
     using Point = bgm::d2::point_xy<Location, bg::cs::cartesian>;
     using Box = bgm::box<Point>;
     using LineString = bgm::linestring<Point>;
     using OpenPolygon = bgm::polygon<Point, false, false>;  // ccw, open polygon
     using ClosedPolygon = bgm::polygon<Point, false, true>; // ccw, closed polygon

#define Vector std::vector
     using Distance = Location;
     using LocationVector = Vector<Location>;
     using PointVector = Vector<Point>;
     using IndexVector = Vector<Index>;
     using DistanceVector = Vector<Distance>;
     using OpenPolygonVector = Vector<OpenPolygon>;
     using ClosedPolygonVector = Vector<ClosedPolygon>;

     // rtree
     constexpr size_t rtree_node_limit = 10;
     using RTreeLookup = std::pair<Box, unsigned>;
     using RTreeLookupVector = Vector<RTreeLookup>;
     using RTree = bgi::rtree<RTreeLookup, bgi::rstar<rtree_node_limit>>;
     using RTreeIterator = RTree::const_query_iterator;

     // only compile templates for these cases
     template <class T>
     using GEO_TYPES = std::enable_if_t<std::is_same_v<T, Point> || std::is_same_v<T, LineString> || std::is_same_v<T, OpenPolygon> || std::is_same_v<T, ClosedPolygon>>;

     struct DistanceResult
     {
          Index index;
          Distance distance;
     };

     struct QueryResults
     {
          IndexVector indices;
          IndexVector rowPtr;
          DistanceVector distances;
     };

     namespace accepted_input_types
     {
          const std::string point = "point";
          const std::string linestring = "linestring";
          const std::string openPolygon = "open_polygon";
          const std::string closedPolygon = "closed_polygon";
     }
}