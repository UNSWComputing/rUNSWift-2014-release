#pragma once

#include "reader.hpp"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <fstream>
#include <QString>

/*
 *  Simple reader that reads in recorded dumps from file.
 */
class BBDReader : public Reader {
   Q_OBJECT
   public:
      /**
       * sets up necessary file descriptors for reading
       *
       * @param fileName the name of the file to read
       */
      explicit BBDReader(const QString &fileName);

      /**
       * sets up necessary file descriptors for reading
       *
       * @param fileName the name of the file to read
       * @param naoData the old naoData to append to
       */
      explicit BBDReader(const QString &fileName, const NaoData &naoData);

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
      virtual ~BBDReader();
   private:
      /**
       * the file stream to read from
       */
      std::ifstream ifs;

      /**
       * the input stream buffer.  may contain compression components
       */
      boost::iostreams::filtering_streambuf<boost::iostreams::input> in;

      /**
       * the input archive attached to the stream buffer
       */
      boost::archive::binary_iarchive *ia;
};
