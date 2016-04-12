/*
  Copyright (c) 2014-2016 DataStax

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/
#ifndef __DRIVER_VALUE_VALUE_INTERFACE_HPP__
#define __DRIVER_VALUE_VALUE_INTERFACE_HPP__
#include "cassandra.h"

#include "objects.hpp"


#include <string>

#include <gtest/gtest.h>

#define COMPARABLE_VALUE_INTERFACE(native, object) driver::value::ValueInterface<native>, \
                                                   driver::value::Comparable<native>, \
                                                   driver::value::Comparable<object>

#define COMPARABLE_VALUE_INTERFACE_OBJECT_ONLY(native, object) driver::value::ValueInterface<native>, \
                                                               driver::value::Comparable<object>

using namespace driver::object;

namespace driver {
  namespace value {

    /**
     * Create a comparable template to act as an interface for comparing
     * values. This is only a partial implementation of a comparable interface
     * and will only handle == and != operators.
     */
    template<typename T>
    class Comparable {
      friend bool operator==(const T& lhs, const T& rhs) {
        return lhs.compare(rhs) == 0;
      }

      friend bool operator!=(const T& lhs, const T& rhs) {
        return lhs.compare(rhs) != 0;
      }
    };

    /**
     * Value is a common interface for all the data types provided by the
     * DataStax C/C++ driver. This interface will perform expectations on the
     * value type and other miscellaneous needs for testing.
     */
    template<typename T>
    class ValueInterface {
    public:
      /**
       * Convert the value to a c-string
       *
       * @return C style string representation
       */
      virtual const char* c_str() const = 0;

      /**
       * Get the CQL type
       *
       * @return CQL type name
       */
      virtual std::string cql_type() const = 0;

      /**
       * Get the CQL value (for embedded simple statements)
       *
       * @return CQL type value
       */
      virtual std::string cql_value() const = 0;

      /**
       * Comparison operation for Comparable template
       *
       * @param rhs Right hand side to compare
       * @return -1 if LHS < RHS, 1 if LHS > RHS, and 0 if equal
       */
      virtual int compare(const T& rhs) const = 0;

      /**
       * Bind the value to a statement at the given index
       *
       * @param statement The statement to bind the value to
       * @param index The index/position where the value will be bound in the
       *              statement
       */
      virtual void statement_bind(shared::native::StatementPtr statement, size_t index) = 0;

      /**
       * Convert the value to a standard string
       *
       * @return Standard string representation
       */
      virtual std::string str() const = 0;

      /**
       * Get the native driver value
       *
       * @return Native driver value
       */
      virtual T value() const = 0;

      /**
       * Get the type of value the native driver value is
       *
       * @return Value type of the native driver value
       */
      virtual CassValueType value_type() const = 0;

    protected:
      /**
       * Initialize the Value from the CassValue
       *
       * @param value CassValue to initialize Value from
       */
      virtual void initialize(const CassValue* value) = 0;

      /**
       * Initialize the Value from an element in a CassRow
       *
       * @param row CassRow to initialize Value from
       * @param column_index Index where the Value exists
       */
      virtual void initialize(const CassRow* row, size_t column_index) = 0;
    };
  }
}

#endif // __DRIVER_VALUE_VALUE_INTERFACE_HPP__