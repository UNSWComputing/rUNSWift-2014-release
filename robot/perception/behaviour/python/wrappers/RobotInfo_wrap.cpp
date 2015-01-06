class_<RobotInfo>("RobotInfo")
   .def_readonly("rr"       , &RobotInfo::rr      )
   .def_readonly("type"     , &RobotInfo::type    );

enum_<RobotInfo::Type>("RobotInfoType")
   .value("rUnknown"  , RobotInfo::rUnknown    )
   .value("rBlue"     , RobotInfo::rBlue       )
   .value("rRed"      , RobotInfo::rRed        );

