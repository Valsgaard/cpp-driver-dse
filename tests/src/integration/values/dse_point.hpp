/*
  Copyright (c) 2016 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef __TEST_DSE_POINT_HPP__
#define __TEST_DSE_POINT_HPP__
#include "value_interface.hpp"

#include "dse.h"

//#include "objects.hpp"

namespace test {
namespace driver {

/**
 * Simplified type for a point value
 */
typedef struct Point_ {
  /**
   * X coordinate
   */
  cass_double_t x;
  /**
   * y coordinate
   */
  cass_double_t y;
} Point;

/**
 * DSE point wrapped value
 */
class DsePoint : public COMPARABLE_VALUE_INTERFACE_VALUE_ONLY(Point, DsePoint) {
public:
  class Exception : public test::Exception {
  public:
    Exception(const std::string& message)
      : test::Exception(message) {};
  };

  DsePoint()
    : is_null_(true) {
    set_point_string();
  }

  DsePoint(cass_double_t x, cass_double_t y)
    : is_null_(false) {
    point_.x = x;
    point_.y = y;
    set_point_string();
  }

  DsePoint(Point point)
    : point_(point)
    , is_null_(false) {
    set_point_string();
  }

  DsePoint(const CassValue* value)
    : is_null_(false) {
    initialize(value);
    set_point_string();
  }

  DsePoint(const ::DseGraphResult* result)
    : is_null_(false) {
    initialize(result);
    set_point_string();
  }

  /**
   * @throws DsePoint::Exception
   */
  DsePoint(const std::string& value)
    : is_null_(false) {
    std::string value_trim = Utils::trim(Utils::to_lower(value));

    // Determine if the value is NULL or valid
    if (value_trim.empty() ||
      value_trim.compare("null") == 0) {
      is_null_ = true;
    } else {
      // Strip all value information markup for a DSE point
      std::string point_value = Utils::replace_all(value_trim, "point", "");
      point_value = Utils::replace_all(point_value, "(", "");
      point_value = Utils::trim(Utils::replace_all(point_value, ")", ""));

      // Make sure the DSE point value is valid and convert into wrapped object
      std::vector<std::string> point_values = Utils::explode(point_value);
      if (point_values.size() == 2) {
        std::stringstream x(Utils::trim(point_values[0]));
        std::stringstream y(Utils::trim(point_values[1]));
        if ((x >> point_.x).fail()) {
          std::stringstream message;
          message << "Invalid X Value: " << x.str() << " is not valid for a point";
          throw Exception(message.str());
        }
        if ((y >> point_.y).fail()) {
          std::stringstream message;
          message << "Invalid Y Value: " << y.str() << " is not valid for a point";
          throw Exception(message.str());
        }
      } else {
        std::stringstream message;
        message << "Invalid Number of Coordinates: " << point_value << " is not valid for a point";
        throw Exception(message.str());
      }
    }

    set_point_string();
  }

  DsePoint(const CassRow* row, size_t column_index)
    : is_null_(false) {
    initialize(row, column_index);
    set_point_string();
  }

  const char* c_str() const {
    return point_string_.c_str();
  }

  std::string cql_type() const {
    return std::string("'PointType'");
  }

  std::string cql_value() const {
    if (is_null_) {
      return point_string_;
    }
    return "'POINT(" + point_string_ + ")'";
  }

  /**
   * Comparison operation for driver value DSE point
   *
   * @param rhs Right hand side to compare
   * @return -1 if LHS < RHS, 1 if LHS > RHS, and 0 if equal
   */
  int compare(const Point& rhs) const {
    if (point_.x < rhs.x) return -1;
    if (point_.x > rhs.x) return 1;

    if (point_.y < rhs.y) return -1;
    if (point_.y > rhs.y) return 1;

    return 0;
  }

  /**
   * Comparison operation for driver value DSE point
   *
   * @param rhs Right hand side to compare
   * @return -1 if LHS < RHS, 1 if LHS > RHS, and 0 if equal
   */
  int compare(const DsePoint& rhs) const {
    if (is_null_ && rhs.is_null_) return 0;
    return compare(rhs.point_);
  }

  void statement_bind(Statement statement, size_t index) {
    if (is_null_) {
      ASSERT_EQ(CASS_OK, cass_statement_bind_null(statement.get(), index));
    } else {
      ASSERT_EQ(CASS_OK, cass_statement_bind_dse_point(statement.get(), index, point_.x, point_.y));
    }
  }

  bool is_null() const {
    return is_null_;
  }

  std::string str() const {
    return point_string_;
  }

  Point value() const {
    return point_;
  }

  CassValueType value_type() const {
    return CASS_VALUE_TYPE_CUSTOM;
  }

private:
  /**
   * Simple point value
   */
  Point point_;
  /**
   * Wrapped native driver value as string
   */
  std::string point_string_;
  /**
   * Flag to determine if value is NULL
   */
  bool is_null_;

  void initialize(const CassValue* value) {
    // Ensure the value types
    ASSERT_TRUE(value != NULL) << "Invalid CassValue: Value should not be null";
    CassValueType value_type = cass_value_type(value);
    ASSERT_EQ(CASS_VALUE_TYPE_CUSTOM, value_type)
      << "Invalid Value Type: Value is not a DSE point (custom) [" << value_type << "]";
    const CassDataType* data_type = cass_value_data_type(value);
    value_type = cass_data_type_type(data_type);
    ASSERT_EQ(CASS_VALUE_TYPE_CUSTOM, value_type)
      << "Invalid Data Type: Value->DataType is not a DSE point (custom)";

    // Get the DSE point type
    if (cass_value_is_null(value)) {
      is_null_ = true;
    } else {
      ASSERT_EQ(CASS_OK, cass_value_get_dse_point(value, &point_.x, &point_.y))
        << "Unable to Get DSE Point: Invalid error code returned";
    }
  }

  void initialize(const CassRow* row, size_t column_index) {
    ASSERT_TRUE(row != NULL) << "Invalid Row: Row should not be null";
    initialize(cass_row_get_column(row, column_index));
  }

  void initialize(const ::DseGraphResult* result) {
    if (dse_graph_result_is_null(result)) {
      is_null_ = true;
    } else {
      ASSERT_EQ(CASS_OK, dse_graph_result_as_point(result, &point_.x, &point_.y));
    }
  }

  /**
   * Set the string value of the DSE point
   */
  void set_point_string() {
    if (is_null_) {
      point_string_ = "null";
    } else {
      std::stringstream point_string;
      point_string << point_.x << " " << point_.y;
      point_string_ = point_string.str();
    }
  }
};

} // namespace driver
} // namespace test

#endif //  __TEST_DSE_POINT_HPP__
