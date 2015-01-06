import robot

class World(object):
   """World.

   An interface for the a robot's interaction with the world.
   Includes methods and fields for accessing information about the
   world and performing actions (by creating action requests).
   """

   def __init__(self, blackboard=None):

      # The current blackboard instance. Note that this will get updated on each frame, so objects that have
      # a world instance can simply access world.blackboard to have the current one, and we don't need to pass
      # blackboard down the call chain.
      self.blackboard = blackboard

      # Whether we're in the phase after kickoff where we're not allowed in the centre circle.
      self.inKickOffWaitTime = False
      # Time since we entered the kickoff wait state in seconds.
      self.kickOffWaitTimeElapsed = 0.0

      # A behaviour request to perform at the end of this behaviour processing cycle.
      self.b_request = None

   def update(self, blackboard):
      """Update.

      Updates the current blackboard reference.
      """

      self.blackboard = blackboard
      # New behaviour request.
      self.b_request = robot.BehaviourRequest()