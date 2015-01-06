import actioncommand
import math
import robot
import Sensors
import Global
from util import MathUtil

class HeadSkill(object):

   def tick(self, blackboard):
      return FindBall(blackboard).tick(blackboard)


# This object should be used globally to access the HeadSkill interface
instance = HeadSkill()


class FindBall(object):

   def __init__(self, blackboard):
      self.ballYaw = 0
      self.ballPitch = 0

      self.closeInPitch = math.radians(17)
      self.farPitch = math.radians(0)

      self.ballCloseIn = True
      self.ballCloseInDistance = 800.0

      self.targetYaw = 0
      self.targetPitch = self.closeInPitch

      self.numFramesTracked = 0

   def tick(self, blackboard):
      self.numFramesTracked += 1

      # get ball into centre of frame
      self.lostBall = (blackboard.localisation.ballLostCount > 60)
      request = robot.BehaviourRequest()
      request.actions = robot.All()

      neckRelative = blackboard.localisation.ballNeckRelative
      self.targetPitch = self.calculateDesiredPitch(neckRelative)

      if len(blackboard.vision.balls) > 0:
         ball = blackboard.vision.balls[0]
         imgCoords = blackboard.vision.balls[0].imageCoords
         
         if blackboard.vision.balls[0].topCamera:
            CONSTANT_X = 1280.0 / 1.06290551
            xDiff = -(imgCoords[0] - 640) / CONSTANT_X
         else:
            CONSTANT_X = 640.0 / 1.06290551
            xDiff = -(imgCoords[0] - 640/2) / CONSTANT_X

         self.targetYaw = xDiff

      if blackboard.localisation.ballLostCount < 3:
         factor =  abs(self.targetYaw) / 1.06290551

         speed = 0.75 * factor #10.0 * factor * factor
         request.actions.head = actioncommand.head(self.targetYaw, self.targetPitch, True, speed, 0.2)

      else:
         self.ballYaw = self.calculateDesiredYaw(neckRelative)
         self.targetYaw = self.ballYaw
         request.actions.head = actioncommand.head(self.targetYaw, self.targetPitch, False, 0.25, 0.2)

      return request


   def calculateDesiredYaw(self, neckRelativeTarget):
      return math.atan2(neckRelativeTarget.y, neckRelativeTarget.x)


   def calculateDesiredPitch(self, neckRelativeTarget):
      xSq = neckRelativeTarget.x*neckRelativeTarget.x
      ySq = neckRelativeTarget.y*neckRelativeTarget.y
      horizontalDistance = math.sqrt(xSq + ySq)

      if self.ballCloseIn and horizontalDistance > 1.1 * self.ballCloseInDistance:
         self.ballCloseIn = False

      if not self.ballCloseIn and horizontalDistance < 0.9 * self.ballCloseInDistance:
         self.ballCloseIn = True

      if self.ballCloseIn:
         return self.closeInPitch
      else:
         return self.farPitch


   def shouldActiveLocalise(self, blackboard):
      return False
