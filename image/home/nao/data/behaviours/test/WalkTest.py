from math import radians
import robot
import actioncommand
from Task import BehaviourTask, TaskState
from skills.WalkToPoint import WalkToPoint
from util.Timer import WallTimer

class WalkTest(BehaviourTask):
    
   def init(self):
      self.stateTimer = WallTimer(10 * 1000000)
      self.w2p = WalkToPoint(self.world)
      self.current_state = Stand(self, timeToRun=10)


class TimedTask(TaskState):

   def init(self, timeToRun=0, initMessage=""):
      self.timer = WallTimer(timeToRun*1000000)
      robot.say(initMessage)
      self.w2p = WalkToPoint(self.world)

   def transition(self):
      return self


class Stand(TimedTask):

   def tick(self):
      self.world.b_request.actions.body = actioncommand.crouch()

   def transition(self):
      if self.timer.finished():
         return WalkForwardFast(self.parent, timeToRun=5)
      else:
         return self

class WalkForwardFast(TimedTask):

   def tick(self):
      self.w2p.tick(500, 0, 0, urgency=1, relative=True)

   def transition(self):
      if self.timer.finished():
         return SpinAroundRealQuick(self, timeToRun=5)
      else:
         return self

class SpinAroundRealQuick(TimedTask):

   def tick(self):
      self.w2p.tick(0, 0, radians(90), relative=True)

   def transition(self):
      if self.timer.finished():
         return WalkDiagonally(self, timeToRun=10)
      else:
         return self

class WalkDiagonally(TimedTask):

   def tick(self):
      self.w2p.tick(500, 500, radians(10), relative=True)

   def transition(self):
      if self.timer.finished():
         return Stand(self, timeToRun=10)
      else:
         return self