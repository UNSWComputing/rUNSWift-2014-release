import actioncommand
from Task import BehaviourTask

class StandStraight(BehaviourTask):

   def tick(self):

      body = actioncommand.standStraight()
      head = actioncommand.head(0, 0, False, 1, 1)
      leds = actioncommand.leds()

      self.world.b_request.actions = actioncommand.compose(head, body, leds)
