#pragma once

#include <boost/polygon/polygon.hpp>
#include <cv.h>

#define FACTOR (1 << 16)

typedef std::vector<cv::Point2d> MyPolygon;
typedef std::vector<MyPolygon> PolygonList;

namespace boost { namespace polygon {
    template <>
	struct geometry_concept<cv::Point2d> { typedef point_concept type; };
 
    
    //Then we specialize the gtl point traits for our point type
    template <>
    struct point_traits<cv::Point2d> {
        typedef long long coordinate_type;
    
        static inline coordinate_type get(const cv::Point2d& point, 
        orientation_2d orient) {
            if(orient == HORIZONTAL)
                return static_cast<long long>(point.x * FACTOR);
            return static_cast<long long>(point.y * FACTOR);
        }
    };
    
    template <>
    struct point_mutable_traits<cv::Point2d> {
        static inline void set(cv::Point2d& point, orientation_2d orient, long long value) {
            if(orient == HORIZONTAL)
                point.x = static_cast<double>(value) / FACTOR;
            else
            point.y = static_cast<double>(value) / FACTOR;
        }
        static inline cv::Point2d construct(long long x_value, long long y_value) {
            cv::Point2d retval;
            retval.x = static_cast<double>(x_value) / FACTOR;
            retval.y = static_cast<double>(y_value) / FACTOR;
            return retval;
        }
    };
} }

//I'm lazy and use the stl everywhere to avoid writing my own classes
//my toy polygon is a std::list<CPoint>
typedef std::vector<cv::Point2d> CPolygon;

//we need to specialize our polygon concept mapping in boost polygon
namespace boost { namespace polygon {
  //first register CPolygon as a polygon_concept type
  template <>
  struct geometry_concept<CPolygon>{ typedef polygon_concept type; };

  template <>
  struct polygon_traits<CPolygon> {
    typedef long long coordinate_type;
    typedef CPolygon::const_iterator iterator_type;
    typedef cv::Point2d point_type;

    // Get the begin iterator
    static inline iterator_type begin_points(const CPolygon& t) {
      return t.begin();
    }

    // Get the end iterator
    static inline iterator_type end_points(const CPolygon& t) {
      return t.end();
    }

    // Get the number of sides of the polygon
    static inline std::size_t size(const CPolygon& t) {
      return t.size();
    }

    // Get the winding direction of the polygon
    static inline winding_direction winding(const CPolygon& t) {
      return unknown_winding;
    }
  };

  template <>
  struct polygon_mutable_traits<CPolygon> {
    //expects stl style iterators
    template <typename iT>
    static inline CPolygon& set_points(CPolygon& t, 
                                       iT input_begin, iT input_end) {
      t.clear();
	  std::transform(input_begin, input_end, std::back_inserter(t), [](const std::iterator_traits<iT>::value_type& pt) -> cv::Point2d
		  {
			  cv::Point2d myPt;
			  boost::polygon::assign(myPt, pt);
			  return myPt;
	  });
      return t;
    }

  };
} }

//OK, finally we get to declare our own polygon set type
typedef std::vector<CPolygon> CPolygonSet;

//deque isn't automatically a polygon set in the library
//because it is a standard container there is a shortcut
//for mapping it to polygon set concept, but I'll do it
//the long way that you would use in the general case.
namespace boost { namespace polygon {
  //first we register CPolygonSet as a polygon set
  template <>
  struct geometry_concept<CPolygonSet> { typedef polygon_set_concept type; };

  //next we map to the concept through traits
  template <>
  struct polygon_set_traits<CPolygonSet> {
    typedef long long coordinate_type;
    typedef CPolygonSet::const_iterator iterator_type;
    typedef CPolygonSet operator_arg_type;

    static inline iterator_type begin(const CPolygonSet& polygon_set) {
      return polygon_set.begin();
    }

    static inline iterator_type end(const CPolygonSet& polygon_set) {
      return polygon_set.end();
    }

    //don't worry about these, just return false from them
    static inline bool clean(const CPolygonSet& polygon_set) { return false; }
    static inline bool sorted(const CPolygonSet& polygon_set) { return false; }
  };

  template <>
  struct polygon_set_mutable_traits<CPolygonSet> {
    template <typename input_iterator_type>
    static inline void set(CPolygonSet& polygon_set, input_iterator_type input_begin, input_iterator_type input_end) {
      polygon_set.clear();
      //this is kind of cheesy. I am copying the unknown input geometry
      //into my own polygon set and then calling get to populate the
      //deque
      polygon_set_data<long long> ps;
      ps.insert(input_begin, input_end);
      ps.get(polygon_set);
      //if you had your own odd-ball polygon set you would probably have
      //to iterate through each polygon at this point and do something
      //extra
    }
  };
} }