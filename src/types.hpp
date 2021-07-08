#pragma once

#include <boost/geometry.hpp>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>

#include <vector>

namespace boost_geo_query
{

     namespace bg = boost::geometry;
     namespace bgi = bg::index;
     namespace bgm = bg::model;
     using Point = bgm::d2::point_xy<float, bg::cs::cartesian>;
     using LineString = bgm::linestring<Point>;
     using Box = bgm::box<Point>;
     using OpenPolygon = bgm::polygon<Point, false, false>;  // ccw, open polygon
     using ClosedPolygon = bgm::polygon<Point, false, true>; // ccw, closed polygon

     #define Vector std::vector
     using Index = uint64_t;
     using Distance = double;
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

     struct DistanceResult
     {
          Index result;
          Distance distance;
     };

     struct DistanceResults
     {
          IndexVector results;
          DistanceVector distances;
     };

     class Vector2D
     {
     public:
          Vector2D(const PointVector &p, const IndexVector &i) : _data{p}, _index{i} {};

          void push_back(const Point &entry)
          {
               _index.push_back(_data.size());
               _data.push_back(entry);
          }
          void push_back(const PointVector &entries)
          {
               _index.push_back(_data.size());
               _data.insert(_data.end(), entries.begin(), entries.end());
          }
          PointVector get(const Index &i) { return PointVector(_data.begin() + _index[i], _data.begin() + _index[i + 1]); }

     private:
          PointVector _data{PointVector(0)};
          IndexVector _index{IndexVector{0}};
     };

     struct IntersectingResults
     {
          IndexVector results;
          IndexVector idxPointer;
     };

}