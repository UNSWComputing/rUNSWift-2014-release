#pragma once

#include "reader.hpp"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <fstream>
#include <QString>

#define BOOST_HAS_COMPRESSION false

/*
 *  Simple reader that reads in recorded dumps from file.
 */
class RecordReader : public Reader {
   Q_OBJECT
   public:
      /**
       * sets up necessary file descriptors for reading
       *
       * @param fileName the name of the file to read
       */
      explicit RecordReader(const QString &fileName);

      /**
       * sets up necessary file descriptors for reading
       *
       * @param fileName the name of the file to read
       * @param naoData the old naoData to append to
       */
      explicit RecordReader(const QString &fileName, const NaoData &naoData);

      /**
       * read in record and store in naoData
       *
       * @param fileName the name of the file to read
       * @param naoData to store data
       */
      static void read(const QString &fileName, NaoData &naoData);

      /**
       * helper function to write a file readable by this class
       *
       * @param fileName the name of the file to write
       * @param naoData the data to write
       */
      static void write(const QString &fileName, const NaoData &naoData);
      virtual void run();

      /**
       * closes appropriate file descriptors and frees memory
       */
      virtual ~RecordReader();

   private:
      const QString fileName;

};
