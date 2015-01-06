import actioncommand
from math import cos, sin, radians
from Task import BehaviourTask, TaskState
from util import MathUtil
from util import FieldGeometry
from util import Vector2D
from util.Hysteresis import Hysteresis


PRIORITY_FORWARD, PRIORITY_LEFT, PRIORITY_TURN, PRIORITY_MIX = 0, 1, 2, 3

_prevPriority = None

def limitWalk(forward, left, turn):

   global _prevPriority

   priority = PRIORITY_FORWARD

   if abs(turn) > radians(60):
      priority = PRIORITY_TURN

   elif abs(forward) < 100 and abs(left) < 100:
      priority = PRIORITY_MIX

   elif abs(left) > abs(forward):
      priority = PRIORITY_LEFT

   if priority != _prevPriority:
      _prevPriority = priority

   clampingNeeded = False

   if abs(forward) == 0 and abs(left) == 0 and abs(turn) ==0:
      forward = 3

   return forward, left, turn, clampingNeeded


class WalkToPoint(BehaviourTask):

   NEARBY_SWITCHING_DISTANCE = 400
   FARAWAY_SWITCHING_DISTANCE = 600

   def init(self, keepFacing=False, relative=False, useAvoidance=True):
      self.keepFacing = keepFacing
      self.relativeTarget=relative
      self.currentState = Initial(self)
      self.currentTarget = Vector2D.Vector2D(0, 0)
      self.target = None

      self.movingAvoidanceHistory = []

      self.avoidanceHys = Hysteresis(-10, 10)
      self.visionPostHys = Hysteresis(0, 10)

   def transition(self):

      prevState = self.currentState

      if self.keepFacing:
         if not isinstance(self.currentState, Facing):
            self.currentState = Facing(self)

      else:
         if isinstance(self.currentState, FarAway) and self.currentTarget.isShorterThan(WalkToPoint.NEARBY_SWITCHING_DISTANCE):
            self.currentState = Nearby(self)
         elif isinstance(self.currentState, Nearby) and self.currentTarget.isLongerThan(WalkToPoint.FARAWAY_SWITCHING_DISTANCE):
            self.currentState = FarAway(self)
         elif isinstance(self.currentState, FarAwayAndFacingAway) and abs(self.currentTarget.heading()) < radians(20):
            self.currentState = FarAway(self)
         elif isinstance(self.currentState, FarAway) and abs(self.currentTarget.heading()) > radians(90):
            self.currentState = FarAwayAndFacingAway(self)
         elif not self.keepFacing and (isinstance(self.currentState, Facing) or isinstance(self.currentState, Initial)):
            self.currentState = FarAwayAndFacingAway(self)

      self.printStateChange(prevState, self.currentState)

   def _tick(self, x, y, theta, urgency=0, keepFacing=False, relative=False, useAvoidance=True, useOnlySonarAvoid=False):

      urgency = min(1.0, urgency)

      self.keepFacing = keepFacing

      # Convert everything to relative.
      if relative:
         vectorToTarget = Vector2D.Vector2D(x, y)
         facingTurn = theta
      else:
         vectorToTarget, facingTurn = FieldGeometry.globalPoseToRobotRelativePose(Vector2D.Vector2D(x, y), theta)

      facingTurn = MathUtil.normalisedTheta(facingTurn)
         
      self.currentTarget = vectorToTarget
      targetHeading = MathUtil.normalisedTheta(vectorToTarget.heading())
          
      # forward/left are used for final adjustments, rotate to mean pos after the turn is made
      if vectorToTarget.isShorterThan(400) or keepFacing:
         forward = vectorToTarget.x * cos(-facingTurn) - vectorToTarget.y * sin(-facingTurn)
         left = vectorToTarget.x * sin(-facingTurn) + vectorToTarget.y * cos(-facingTurn)
      else:
         forward = vectorToTarget.x
         left = vectorToTarget.y
      
      self.currentState.urgency = urgency
      self.currentState.tick(forward, left, targetHeading, facingTurn)


class Nearby(TaskState):

   def tick(self, forward, left, _, facingTurn):

      forward, left, facingTurn, _ = limitWalk(forward, left, facingTurn)
      self.world.b_request.actions.body = actioncommand.walk(forward, left, facingTurn)

class FarAway(TaskState):

   def tick(self, forward, left, heading, _):

      forward, left, heading, needsClamping = limitWalk(forward, left, heading)
      self.world.b_request.needsClamping = needsClamping
      self.world.b_request.actions.body = actioncommand.walk(forward, left, heading)

class FarAwayAndFacingAway(TaskState):

   def tick(self, forward, left, heading, _):

      forward, left, heading, needsClamping = limitWalk(0, 0, heading)
      self.world.b_request.needsClamping = needsClamping
      self.world.b_request.actions.body = actioncommand.walk(0, 0, heading)

class Facing(TaskState):

   def tick(self, forward, left, _, facingTurn):

      forward, left, facingTurn, _ = limitWalk(forward, left, facingTurn)
      self.world.b_request.actions.body = actioncommand.walk(forward, left, facingTurn)

class Initial(TaskState):

   def tick(self, *args, **kwargs):
      raise NotImplementedError("Should have changed states!")
