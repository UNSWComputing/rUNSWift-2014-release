import Global
from Task import BehaviourTask

from skills.WalkToPoint import WalkToPoint

class WalkInACircle(BehaviourTask):
    
   def init(self):
      self.walk2Point = WalkToPoint(self.world)

   def tick(self):
      self.walk2Point.tick(Global.ballDistance() - 50, -130, Global.ballHeading(), relative=True)