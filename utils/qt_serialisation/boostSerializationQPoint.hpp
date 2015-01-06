#pragma once

namespace boost {
   namespace serialization {
      template<class Archive>
      void serialize(Archive &ar, QPointF &g, const unsigned int version) {
         ar & g.rx();
         ar & g.ry();
      }
      template<class Archive>
      void serialize(Archive &ar, QPoint &g, const unsigned int version) {
         ar & g.rx();
         ar & g.ry();
      }
   } // namespace serialization
} // namespace boost
