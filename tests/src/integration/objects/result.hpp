/*
  Copyright (c) 2016 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef __TEST_RESULT_HPP__
#define __TEST_RESULT_HPP__
#include "cassandra.h"

#include "objects/object_base.hpp"

#include "objects/future.hpp"
#include "objects/rows.hpp"

#include "exception.hpp"

#include <string>

#include <gtest/gtest.h>

namespace test {
namespace driver {

/**
 * Wrapped result object
 */
class Result : public Object<const CassResult, cass_result_free> {
public:
  /**
   * Create the result object from the native driver object
   *
   * @param result Native driver object
   */
  Result(const CassResult* result)
    : Object<const CassResult, cass_result_free>(result) {}

  /**
   * Create the result object from a shared reference
   *
   * @param result Shared reference
   */
  Result(Ptr result)
    : Object<const CassResult, cass_result_free>(result) {}

  /**
   * Create the result object from a future object
   *
   * @param future Wrapped driver object
   */
  Result(Future future)
    : Object<const CassResult, cass_result_free>(future.result())
    , future_(future) {}

  /**
   * Get the attempted host/address of the future
   *
   * @return Attempted host/address
   */
  const std::vector<std::string> attempted_hosts() {
    return future_.attempted_hosts();
  }

  /**
   * Get the error code from the future
   *
   * @return Error code of the future
   */
  CassError error_code() {
    return future_.error_code();
  }

  /**
   * Get the human readable description of the error code
   *
   * @return Error description
   */
  const std::string error_description() {
    return future_.error_description();
  }

  /**
   * Get the error message of the future if an error occurred
   *
   * @return Error message
   */
  const std::string error_message() {
    return future_.error_message();
  }

  /**
   * Get the host/address of the future
   *
   * @return Host/Address
   */
  const std::string host() {
    return future_.host();
  }

  /**
   * Get the number of columns from the result
   *
   * @return The number of columns in the result
   */
  size_t column_count() {
    return cass_result_column_count(get());
  }

  /**
   * Get the first row from the result
   *
   * @return The first row
   */
  const CassRow* first_row() {
    return cass_result_first_row(get());
  }

  /**
   * Get the number of rows from the result
   *
   * @return The number of rows in the result
   */
  size_t row_count() {
    return cass_result_row_count(get());
  }

  /**
   * Get the rows from the result
   *
   * @return The rows from the result
   */
  Rows rows() {
    return Rows(cass_iterator_from_result(get()), row_count(), column_count());
  }

private:
  /**
   * Future wrapped object
   */
  Future future_;
};

} // namespace driver
} // namespace test

#endif // __TEST_RESULT_HPP__
