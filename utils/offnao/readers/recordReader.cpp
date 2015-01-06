#include "recordReader.hpp"

#include <boost/archive/binary_oarchive.hpp>
#if BOOST_HAS_COMPRESSION
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#endif
#include <sstream>

using namespace std;

RecordReader::RecordReader(const QString &fileName) : fileName(fileName)
{
}

RecordReader::RecordReader(const QString &fileName, const NaoData &naoData) :
Reader(naoData), fileName(fileName)
{
}

RecordReader::~RecordReader()
{
}

void RecordReader::read(const QString &fileName, NaoData &naoData)
{
   std::ifstream ifs;
   boost::iostreams::filtering_streambuf<boost::iostreams::input> in;

   // catch if file not found
   ifs.exceptions(ifstream::eofbit | ifstream::failbit | ifstream::badbit);
   ifs.open(qPrintable(fileName), ios::in | ios::binary);
   #if BOOST_HAS_COMPRESSION
   if (fileName.endsWith(".gz")) {
      in.push(boost::iostreams::gzip_decompressor());
   }
   if (fileName.endsWith(".bz2")) {
      in.push(boost::iostreams::bzip2_decompressor());
   }
   #endif
   in.push(ifs);
   boost::archive::binary_iarchive ia(in);

   NaoData nd;
   ia & nd;
   for(int f = 0; f < nd.getFramesTotal(); ++f) {
      naoData.appendFrame(nd.getFrame(f));
   }
}

void RecordReader::write(const QString &fileName, const NaoData &naoData)
{
   ofstream ofs(qPrintable(fileName), ios::out | ios::binary | ios::trunc);
   boost::iostreams::filtering_streambuf<boost::iostreams::output> out;
   #if BOOST_HAS_COMPRESSION
   if (fileName.endsWith(".gz"))
      out.push(boost::iostreams::gzip_compressor());
   if (fileName.endsWith(".bz2"))
      out.push(boost::iostreams::bzip2_compressor());
   #endif
   out.push(ofs);
   boost::archive::binary_oarchive oa(out);
   oa & naoData;
}

void RecordReader::run() {
   try {
      read(fileName, naoData);
   } catch(const std::exception& e) {
      QString s("Can not load record: ");
      emit disconnectFromNao();
      emit showMessage(s + e.what());
      emit openFile();
      return;
   }
   stringstream s;
   s << "Finished loading record which consisted of " <<
        naoData.getFramesTotal() << " frames.";
   emit showMessage(s.str().c_str(), 5000);
   emit newNaoData(&naoData);
   
   int currentIndex = 0;
   isAlive = true;
   while (isAlive) {
      if (!naoData.getIsPaused() &&
         naoData.getCurrentFrameIndex() < naoData.getFramesTotal() - 1) {
         naoData.nextFrame();
         emit newNaoData(&naoData);
      } else if (currentIndex != naoData.getCurrentFrameIndex()) {
         emit newNaoData(&naoData);
      }
      currentIndex = naoData.getCurrentFrameIndex();
      msleep(500);
   }
   emit newNaoData(NULL);
}
