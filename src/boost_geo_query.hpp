#pragma once

#include <types.hpp>
namespace boost_geo_query
{

    template <class T, class = GEO_TYPES<T>>
    class RTreeGeometryQuery
    {
    public:
        using TargetVector = std::vector<T>;
        RTreeGeometryQuery() {}
        RTreeGeometryQuery(const TargetVector &target, const RTree &rTree) : _target_geo(target), _rTree(rTree) {}
        RTreeGeometryQuery(const TargetVector &target) : _target_geo(target)
        {

            RTree rTree;
            for (Index i = 0; i < target.size(); i++)
            {
                Box b;
                boost::geometry::envelope(target[i], b);
                rTree.insert(std::make_pair(b, i));
            }
            _rTree = rTree;
        }
        ~RTreeGeometryQuery() {}

        // touching != overlapping
        template <class U, class = GEO_TYPES<U>>
        IntersectingResults overlaps_with(const std::vector<U> &geo) { return intersects_with(geo, true); }

        template <class U, class = GEO_TYPES<U>>
        IntersectingResults intersects_with(const std::vector<U> &geo, bool require_full_overlap = false)
        {
            IntersectingResults ir;
            ir.rowPtr.push_back(0);

            for (const auto &p : geo)
            {
                IndexVector r = intersects_with(p, require_full_overlap);
                if (r.size() > 0)
                {
                    ir.results.insert(ir.results.end(), r.begin(), r.end());
                    ir.rowPtr.push_back(ir.results.size());
                }
            }
            return ir;
        }

        template <class U, class = GEO_TYPES<U>>
        IndexVector intersects_with(const U &geo, bool require_full_overlap)
        {
            RTreeLookupVector results;
            const Box b = bg::return_envelope<Box>(geo);
            _rTree.query(bgi::intersects(b), std::back_inserter(results));
            IndexVector rv;
            for (const auto &result : results)
            {
                Index idx = result.second;
                const T &target = _target_geo[idx];
                if (bg::intersects(geo, target) && (require_full_overlap ? !bg::touches(geo, target) : true))
                {
                    rv.push_back(idx);
                }
            }

            std::sort(rv.begin(), rv.end());
            return rv;
        }

        template <class U, class = GEO_TYPES<U>>
        DistanceResults nearest_to(const std::vector<U> &geo)
        {
            DistanceResults drs;
            drs.results.reserve(geo.size());
            drs.distances.reserve(geo.size());

            for (const auto &p : geo)
            {
                DistanceResult dr = nearest_to(p);
                drs.results.push_back(dr.result);
                drs.distances.push_back(dr.distance);
            }
            return drs;
        }

        template <class U, class = GEO_TYPES<U>>
        DistanceResult nearest_to(const U &geo, Index initial_searchsize = -1)
        {
            // find nearest element idx + distance
            bool guaranteed_found = false;
            Index min_i = boost::numeric::bounds<Index>::highest();
            Distance min_d = boost::numeric::bounds<Distance>::highest();
            const Box geo_box = bg::return_envelope<Box>(geo);
            Index nb_search_elements = initial_searchsize <= 0 ? rtree_node_limit : initial_searchsize;
            Index visited = 0;

            while (!guaranteed_found)
            {
                // iterate over nearest elements. Use iterator to ensure nearest-farthest sorting
                RTreeIterator it = _rTree.qbegin(bgi::nearest(geo_box, nb_search_elements));

                // skip already visited elements
                std::advance(it, visited);

                for (; it != _rTree.qend(); ++it)
                {
                    const Box &b = it->first;
                    Index idx = it->second;

                    const auto &target = _target_geo[idx];

                    Distance d = bg::comparable_distance(geo, target);
                    if (d < min_d)
                    {
                        min_d = d;
                        min_i = idx;
                    }

                    // given || box_i - box_t || <= || i - t ||
                    // we iterate over sorted(|| box_i - box_t ||)
                    // so if at any point we have || box_i - box_t || >= || current_found - t ||
                    // then the other geometries are equal or further away and we found the nearest geometry
                    if (bg::comparable_distance(b, geo_box) >= min_d)
                    {
                        guaranteed_found = true;
                        break;
                    }

                    visited++;
                }

                // went through all elements: closest found
                if (visited >= _target_geo.size())
                {
                    break;
                }

                // if not sure we found the closest geometry (rather than closest box), get more elements
                nb_search_elements *= 2;
            }

            min_d = bg::distance(geo, _target_geo[min_i]);
            return DistanceResult{min_i, min_d};
        }

        template <class U, class = GEO_TYPES<U>>
        IntersectingResults within_distance_of(const std::vector<U> &geo, Distance dist)
        {
            IntersectingResults ir;
            ir.rowPtr.push_back(0);

            for (const auto &p : geo)
            {
                IndexVector r = within_distance_of(p, dist);
                if (r.size() > 0)
                {
                    ir.results.insert(ir.results.end(), r.begin(), r.end());
                    ir.rowPtr.push_back(ir.results.size());
                }
            }
            return ir;
        }

        template <class U, class = GEO_TYPES<U>>
        IndexVector within_distance_of(const U &geo, Distance dist)
        {
            // create box around geometry to capture min/max
            const Box b = bg::return_envelope<Box>(geo);

            // create bigger box with added radius
            Point radius_box_min_point(b.min_corner().x() - dist, b.min_corner().y() - dist);
            Point radius_box_max_point(b.max_corner().x() + dist, b.max_corner().y() + dist);
            const Box b_radius = Box(radius_box_min_point, radius_box_max_point);
            // lookup bigger box to find candidates
            RTreeLookupVector results;
            _rTree.query(bgi::intersects(b_radius), std::back_inserter(results));

            // test candidates for actual distance to geometry
            IndexVector rv;
            for (const auto &result : results)
            {
                Index idx = result.second;
                const auto &target = _target_geo[idx];
                if (bg::distance(geo, target) <= dist)
                {
                    rv.push_back(idx);
                }
            }
            std::sort(rv.begin(), rv.end());
            return rv;
        }

    private:
        TargetVector _target_geo;
        RTree _rTree;
    };

}