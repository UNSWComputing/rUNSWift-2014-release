class_<SensorValues>("SensorValues")
   .add_property("joints"       , &SensorValues::joints )
   .add_property("sensors"      , &SensorValues::sensors)
   .add_property("sonar"        , &SensorValues::sonar  );

