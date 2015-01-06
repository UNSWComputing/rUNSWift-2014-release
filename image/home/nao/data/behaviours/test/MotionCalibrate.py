import actioncommand
from Task import BehaviourTask

class MotionCalibrate(BehaviourTask):
   def tick(self):
      body = actioncommand.motionCalibrate()
      head = actioncommand.head(0, 0, False, 1, 1)
      leds = actioncommand.leds()
      self.world.b_request.actions = actioncommand.compose(head, body, leds)
