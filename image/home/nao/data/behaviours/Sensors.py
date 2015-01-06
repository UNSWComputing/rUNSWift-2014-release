import robot

class angles(object):
   def __init__(self, blackboard):
      self.blackboard = blackboard

   def __getitem__(self, i):
      return self.blackboard.motion.sensors.joints.angles[i]

class stiffnesses(object):
   def __init__(self, blackboard):
      self.stiffnesses = robot.floatArray_frompointer(blackboard.motion.sensors.joints.stiffnesses)
   def __getitem__(self, i):
      return self.stiffnesses[i]

class temperatures(object):
   def __init__(self, blackboard):
      self.temperatures = robot.floatArray_frompointer(blackboard.motion.sensors.joints.temperatures)
   def __getitem__(self, i):
      return self.temperatures[i]

class sensors(object):
   def __init__(self, blackboard):
      self.sensors = robot.floatArray_frompointer(blackboard.motion.sensors.sensors)
   def __getitem__(self, i):
      return self.sensors[i]

class sonar(object):
   def __init__(self, blackboard):
      self.sonar = robot.floatArray_frompointer(blackboard.motion.sensors.sonar)
   def __getitem__(self, i):
      return self.sonar[i]

