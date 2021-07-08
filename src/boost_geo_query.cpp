#include <boost_geo_query.hpp>
#include <boost/foreach.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <iostream>

namespace boost_geo_query
{

    DistanceResults Thingy::find_nearest(const ClosedPolygonVector &pv, const PointVector &points, const RTree &rTree)
    {
        DistanceResults drs;
        drs.results.reserve(pv.size());
        drs.distances.reserve(pv.size());

        for (const auto &p : pv)
        {
            DistanceResult dr = find_nearest(p, points, rTree);
            drs.results.push_back(dr.result);
            drs.distances.push_back(dr.distance);
        }
        return drs;
    }

    DistanceResult Thingy::find_nearest(const ClosedPolygon &poly, const PointVector &points, const RTree &rTree)
    {
        bool guaranteed_found = false;
        Index min_i;
        Distance min_d = boost::numeric::bounds<double>::highest();
        const Box poly_box = bg::return_envelope<Box>(poly);
        Index nb_search_elements = rtree_node_limit;
        Index visited = 0;

        while (!guaranteed_found)
        {
            // iterate over nearest elements. Use iterator to ensure nearest-farthest sorting
            RTreeIterator it = rTree.qbegin(bgi::nearest(poly_box, nb_search_elements));

            // skip already visited elements
            std::advance(it, visited);

            for (; it != rTree.qend(); ++it)
            {
                const Box &b = it->first;
                Index idx = it->second;

                const Point &point = points[idx];

                Distance d = bg::comparable_distance(poly, point);
                if (d < min_d)
                {
                    min_d = d;
                    min_i = idx;
                }

                // given || box_i - box_t || <= || i - t ||
                // we iterate over sorted(|| box_i - box_t ||)
                // so if at any point we have || box_i - box_t || <= || current_found - t ||
                // then the other geometries are equal or further away and we found the nearest geometry
                if (bg::comparable_distance(b, poly_box) <= d)
                {
                    guaranteed_found = true;
                    break;
                }

                visited++;
            }

            // went through all elements: closest found
            if (visited >= points.size())
            {
                break;
            }

            // if not sure we found the closest geometry (rather than closest box), get more elements
            nb_search_elements *= 2;
        }

        min_d = bg::distance(poly, points[min_i]);
        return DistanceResult{min_i, min_d};
    }

    IntersectingResults Thingy::find_intersecting(const ClosedPolygonVector &pv, const PointVector &points, const RTree &rTree, bool require_full_overlap)
    {
        IntersectingResults ir;
        ir.idxPointer.push_back(0);

        for (const auto &p : pv)
        {
            IndexVector r = find_intersecting(p, points, rTree, require_full_overlap);
            if (r.size() > 0)
            {
                ir.results.insert(ir.results.end(), r.begin(), r.end());
                ir.idxPointer.push_back(ir.results.size());
            }
        }
        return ir;
    }

    IndexVector Thingy::find_intersecting(const ClosedPolygon &poly, const PointVector &points, const RTree &rTree, bool require_full_overlap)
    {
        RTreeLookupVector results;
        const Box b = bg::return_envelope<Box>(poly);
        rTree.query(bgi::intersects(b), std::back_inserter(results));
        IndexVector rv;
        for (auto &result : results)
        {
            Index idx = result.second;
            const Point &point = points[idx];
            if (bg::intersects(poly, point) && (require_full_overlap ? !bg::touches(poly, point) : true))
            {
                rv.push_back(idx);
            }
        }
        // std::transform(result.begin(), result.end(), std::back_inserter(rv), [](auto const& pair){return pair.second;});
        std::sort(rv.begin(), rv.end());
        return rv;
    }

    IntersectingResults Thingy::find_in_radius(const ClosedPolygonVector &pv, const PointVector &points, const RTree &rTree, Distance dist)
    {
        IntersectingResults ir;
        ir.idxPointer.push_back(0);

        for (const auto &p : pv)
        {
            IndexVector r = find_in_radius(p, points, rTree, dist);
            if (r.size() > 0)
            {
                ir.results.insert(ir.results.end(), r.begin(), r.end());
                ir.idxPointer.push_back(ir.results.size());
            }
        }
        return ir;
    }

    IndexVector Thingy::find_in_radius(const ClosedPolygon &poly, const PointVector &points, const RTree &rTree, Distance dist)
    {
        // create box around geometry to capture min/max
        const Box b = bg::return_envelope<Box>(poly);

        // create bigger box with added radius
        Point radius_box_min_point(b.min_corner().x() - dist, b.min_corner().y() - dist);
        Point radius_box_max_point(b.max_corner().x() + dist, b.max_corner().y() + dist);
        const Box b_radius = Box(radius_box_min_point, radius_box_max_point);
        // lookup bigger box to find candidates
        RTreeLookupVector results;
        rTree.query(bgi::intersects(b_radius), std::back_inserter(results));

        // test candidates for actual distance to geometry
        IndexVector rv;
        for (auto &result : results)
        {
            Index idx = result.second;
            const Point &point = points[idx];
            if (bg::distance(poly, point) <= dist)
            {
                rv.push_back(idx);
            }
        }
        std::sort(rv.begin(), rv.end());
        return rv;
    }

}