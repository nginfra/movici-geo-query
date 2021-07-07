#pragma once

#include <types.hpp>
namespace boost_geo_mapper
{
    
    class Thingy{
        public:
            IntersectingResults find_intersecting(const ClosedPolygonVector& pv, const PointVector& points, const RTree& rTree, bool require_full_overlap = false);
            // touching != overlapping
            IntersectingResults find_overlapping(const ClosedPolygonVector& pv, const PointVector& points, const RTree& rTree) {return find_intersecting(pv, points, rTree, true);}
            IndexVector find_intersecting(const ClosedPolygon& poly, const PointVector& points, const RTree& rTree, bool require_full_overlap);
            
            DistanceResult find_nearest(const ClosedPolygon& poly, const PointVector& points, const RTree& rTree); // element idx + distance
            DistanceResults find_nearest(const ClosedPolygonVector& pv, const PointVector& points, const RTree& rTree);
            // RadiusResult find_in_radius(); // any result in object + within radius r .. something with buffer?
            IntersectingResults find_in_radius(const Point& p, PointVector v, Distance r);

    };

}