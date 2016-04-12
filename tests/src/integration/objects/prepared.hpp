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
#ifndef __DRIVER_OBJECT_PREPARED_HPP__
#define __DRIVER_OBJECT_PREPARED_HPP__
#include "cassandra.h"

#include "objects/statement.hpp"

#include "smart_ptr.hpp"

namespace driver {
  namespace object {

    /**
     * Deleter class for driver object CassPrepared
     */
    class PreparedDeleter {
    public:
      void operator()(const CassPrepared* prepared) {
        if (prepared) {
          cass_prepared_free(prepared);
        }
      }
    };

    // Create scoped and shared pointers for the native driver object
    namespace scoped {
      namespace native {
        typedef cass::ScopedPtr<const CassPrepared, PreparedDeleter> PreparedPtr;
      }
    }
    namespace shared {
      namespace native {
        typedef SmartPtr<const CassPrepared, PreparedDeleter> PreparedPtr;
      }
    }

    /**
     * Wrapped prepared object
     */
    class Prepared {
    public:
      /**
       * Create the prepared object from the native driver object
       *
       * @param prepared Native driver object
       */
      Prepared(const CassPrepared* prepared)
        : prepared_(prepared) {}

      /**
       * Create the prepared object from a shared reference
       *
       * @param prepared Shared reference
       */
      Prepared(shared::native::PreparedPtr prepared)
        : prepared_(prepared) {}

      /**
       * Bind the prepared object and create a statement
       *
       * @return Statement
       */
      shared::StatementPtr bind() {
        return new Statement(cass_prepared_bind(prepared_.get()));
      }

      /**
       * Get the data type for a given column index
       *
       * @param index The column index to retrieve the data type
       * @return Data type at the specified column index
       */
      const CassDataType* data_type(size_t index) {
        return cass_prepared_parameter_data_type(prepared_.get(), index);
      }

      /**
       * Get the data type for a given column name
       *
       * @param name The column name to retrieve the data type
       * @return Data type at the specified column name
       */
      const CassDataType* data_type(const std::string& name) {
        return cass_prepared_parameter_data_type_by_name(prepared_.get(), name.c_str());
      }

      /**
       * Get the value type for a given column index
       *
       * @param index The column index to retrieve the value type
       * @return Value type at the specified column index
       */
      CassValueType value_type(size_t index) {
        return cass_data_type_type(data_type(index));
      }

      /**
       * Get the value type for a given column name
       *
       * @param name The column name to retrieve the value type
       * @return Value type at the specified column index
       */
      CassValueType value_type(const std::string& name) {
        return cass_data_type_type(data_type(name));
      }

    private:
      /**
       * Prepared driver reference object
       */
      shared::native::PreparedPtr prepared_;
    };

    // Create scoped and shared pointers for the wrapped object
    namespace scoped {
      typedef cass::ScopedPtr<Prepared> PreparedPtr;
    }
    namespace shared {
      typedef SmartPtr<Prepared> PreparedPtr;
    }
  }
}

#endif // __DRIVER_OBJECT_PREPARED_HPP__