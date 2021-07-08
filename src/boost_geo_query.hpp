#pragma once

#include <types.hpp>
namespace boost_geo_query
{
    template <class T>
    class RTreeGeometryQuery
    {
    public:
        using SrcVec = std::vector<T>;
        RTreeGeometryQuery(const SrcVec &pv, const PointVector &points, const RTree &rTree) : _src_geom(pv), _target_geom(points), _rTree(rTree) {}
        RTreeGeometryQuery(const SrcVec &pv, const PointVector &points) : _src_geom(pv), _target_geom(points)
        {

            RTree rTree;
            for (Index i = 0; i < points.size(); i++)
            {
                Box b;
                boost::geometry::envelope(points[i], b);
                rTree.insert(std::make_pair(b, i));
            }
            _rTree = rTree;
        }
        ~RTreeGeometryQuery() {}

        IntersectingResults find_intersecting(bool require_full_overlap = false);
        // touching != overlapping
        IntersectingResults find_overlapping() { return find_intersecting(true); }
        IndexVector find_intersecting(const T &poly, bool require_full_overlap);

        DistanceResult find_nearest(const T &poly); // element idx + distance
        DistanceResults find_nearest();

        IndexVector find_in_radius(const T &poly, Distance dist);
        IntersectingResults find_in_radius(Distance dist);

    private:
        const SrcVec _src_geom;
        const PointVector _target_geom;
        RTree _rTree;
    };

}