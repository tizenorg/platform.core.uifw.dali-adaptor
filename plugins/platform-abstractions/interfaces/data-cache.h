#ifndef __DALI_PLATFORM_DATA_CACHE_H__
#define __DALI_PLATFORM_DATA_CACHE_H__

/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

// EXTERNAL INCLUDES
#include <vector>
#include <string>

namespace Dali
{

namespace Platform
{

/**
 * Abstract interface for storing and reading data using a unique search key.
 *
 * | Key | Data |
 * | Key | Data |
 * | Key | Data |
 *
 * For example, the key could be a character code, the data a distance field.
 *
 * Only two operations are supported: Add and Find.
 *
 */
class DataCache
{
public:

   /**
    * Read write mode.
    */
   enum ReadWriteMode
   {
     READ_ONLY,      ///< The owner can only read data
     READ_WRITE      ///< The owner can read / write data
   };

   /**
    * Compression mode
    */
   enum CompressionMode
   {
     COMPRESSION_OFF,      ///< No data compression
     RUN_LENGTH_ENCODING   ///< RLE encoding
   };

   /**
    * Data.
    * A plain old data structure (POD).
    * Used when searching and adding to the data cache.
    */
   struct Data
   {
     /**
      *  Constructor
      */
      Data()
     :data( NULL ),
      length( 0 ),
      exists(false)
      {}

      /**
       * Destructor
       */
      ~Data() {}

     /**
      * copy constructor
      */
     Data( const Data& rhs )
     {
       data = rhs.data;
       length = rhs.length;
       exists = rhs.exists;
     }

     /**
      * Assignment operator
      */
     Data& operator=( const Data& rhs)
     {
        data = rhs.data;
        length = rhs.length;
        exists = rhs.exists;
        return *this;
      }

     /**
      * Set Data
      * @param newData new data to set
      * @param dataLength length of the data (in bytes)
      */
     void SetData( unsigned char* newData, unsigned int dataLength)
     {
       data = newData;
       length = dataLength;
       exists = true;
     }
     unsigned char* data;    ///< pointer to binary data
     unsigned int length:31; ///< length of the data in bytes
     unsigned int exists:1;  ///< whether the data exists in the cache
   };

   typedef unsigned int DataKey;             ///< Data Key
   typedef std::vector< DataKey> KeyVector; ///< Vector of keys
   typedef std::vector< Data> DataVector;   ///< Vector of data

  /**
   * Create a new data cache.
   * @param[in] mode whether the owning object wants to read or read / write the data
   * @param[in] compressionMode whether the data should be compressed
   * @param[in] fileName used to prefix files for storing the data / key information
   * @param[in] maxDataSize maximum size of the data
   * @param[in] maxNumberEntries maximum number of entries in the cache, used to check for overflows
   */
  static DataCache* New( ReadWriteMode mode,
      CompressionMode compressionMode,
      const std::string& fileName,
      unsigned int maxDataSize,
      unsigned int maxNumberEntries );

  /**
   * Destructor.
   */
  virtual ~DataCache()
  {
  }

  /**
   * Given an array of keys, retrieve an array of data associated with it.
   * The dataVector will be filled with Data objects. Each data object
   * will have its data.exists flag set to true, if the data was found or
   * false if it was not.
   * @param[in] keyVector vector of keys to search for
   * @param[out] dataVector vector of data objects
   */
  virtual void Find(const KeyVector& keyVector, DataVector& dataVector) = 0;

  /**
   * Adds an array of key/data to the cache.
   * @param[in] keyVector vector of keys to add
   * @param[in] dataVector vector of data objects to save
   */
  virtual void Add(const KeyVector& keyVector, const DataVector& dataVector) = 0;

protected:

  /**
   * Constructor
   */
  DataCache()
  {
  }

private:

  // Undefined copy constructor.
  DataCache( const DataCache& );

  // Undefined assignment operator.
  DataCache& operator=( const DataCache& );
};

} // namespace Platform

} // namespace Dali

#endif // __DALI_PLATFORM_DATA_CACHE_H__
