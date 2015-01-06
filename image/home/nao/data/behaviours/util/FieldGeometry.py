import robot
from util import MathUtil
from util import Vector2D
import math
import Global
import Constants

blackboard = None

class Kick(object):
   NONE=-1
   MIDDLE=0
   LEFT=1
   RIGHT=2

# Enemy goal vectors.
ENEMY_GOAL_CENTER = Vector2D.Vector2D(Constants.FIELD_LENGTH/2.0, 0)
ENEMY_GOAL_BEHIND_CENTER = Vector2D.Vector2D(Constants.FIELD_LENGTH/2.0 + 100, 0) # +100 offset so angles aren't too sharp near goals
ENEMY_GOAL_INNER_LEFT = Vector2D.Vector2D(Constants.FIELD_LENGTH/2.0, Constants.GOAL_POST_ABS_Y - (Constants.GOAL_POST_DIAMETER/2))
ENEMY_GOAL_INNER_RIGHT = Vector2D.Vector2D(Constants.FIELD_LENGTH/2.0, -Constants.GOAL_POST_ABS_Y + (Constants.GOAL_POST_DIAMETER/2))
ENEMY_GOAL_OUTER_LEFT = Vector2D.Vector2D(Constants.FIELD_LENGTH/2.0, Constants.GOAL_POST_ABS_Y + (Constants.GOAL_POST_DIAMETER/2))
ENEMY_GOAL_OUTER_RIGHT = Vector2D.Vector2D(Constants.FIELD_LENGTH/2.0, -Constants.GOAL_POST_ABS_Y - (Constants.GOAL_POST_DIAMETER/2))
ENEMY_PENALTY_CENTER = Vector2D.Vector2D(Constants.MARKER_CENTER_X, 0)

# Own goal vectors.
OWN_GOAL_CENTER = Vector2D.Vector2D(-Constants.FIELD_LENGTH/2.0, 0)
OWN_GOAL_BEHIND_CENTER = Vector2D.Vector2D(-Constants.FIELD_LENGTH/2.0 - 100, 0) # +100 offset so angles aren't too sharp near goals

# This method should be called at the start of every Behaviour frame, as soon as we
# enter Python.
def update(newBlackboard):
   global blackboard
   blackboard = newBlackboard

TURN_RATE = math.radians(60.0) # radians/second
WALK_RATE = 300.0 # mm/second
CIRCLE_STRAFE_RATE = math.radians(40.0) # radians/second

def calculateTimeToReachPose(myPos, myHeading, targetPos, targetHeading=None):
   toTarget = targetPos.minus(myPos)
   toTargetHeading = math.atan2(toTarget.y, toTarget.x)
   
   # How far we need to turn to point at the targetPos
   toTargetTurn = abs(MathUtil.normalisedTheta(toTargetHeading - myHeading))

   # The straightline distance to walk to the targetPos
   toTargetDistance = toTarget.length()

   # How far we need to turn once we get to the targetPos so that we are facing the targetHeading
   if targetHeading is None:
      toTargetHeadingTurn = 0.0
   else:
      toTargetHeadingTurn = abs(MathUtil.normalisedTheta(toTargetHeading - targetHeading))
   
   return toTargetTurn/TURN_RATE + toTargetDistance/WALK_RATE + toTargetHeadingTurn/CIRCLE_STRAFE_RATE


def calculateTimeToReachBall():
   opponentGoal = ENEMY_GOAL_CENTER
   interceptPoint = getBallIntersectionWithRobot(maintainCanSeeBall = False)
   interceptToGoal = opponentGoal.minus(interceptPoint)
   interceptToGoalHeading = MathUtil.normalisedTheta(math.atan2(interceptToGoal.y, interceptToGoal.x))
   return calculateTimeToReachPose(Global.myPos(), Global.myHeading(), interceptPoint, interceptToGoalHeading)

def calculateTimeToReachPositioning(positioning):
   myPos = positioning.getInstantPosition(Global.ballWorldPos(), Global.myPos())
   return calculateTimeToReachPose(Global.myPos(), Global.myHeading(), myPos, Global.myHeading())


def angleBetweenBallAndGoalCenter():
   ballPos = blackboard.localisation.ballPos
   ballVec = Vector2D.Vector2D(ballPos.x, ballPos.y)
   goalCenter = OWN_GOAL_CENTER
   phi = Vector2D.angleBetween(ballVec, goalCenter)
   return MathUtil.normalisedTheta(phi)

def angleToPoint(point, absCoord):
   phi = Vector2D.angleBetween(point, Vector2D.makeVector2DCopy(absCoord))
   return MathUtil.normalisedTheta(phi)

def angleToPenalty(absCoord):
   phi = Vector2D.angleBetween(ENEMY_PENALTY_CENTER, Vector2D.makeVector2DCopy(absCoord))
   return MathUtil.normalisedTheta(phi)

def angleToGoal(absCoord):
   phi = Vector2D.angleBetween(ENEMY_GOAL_BEHIND_CENTER, Vector2D.makeVector2DCopy(absCoord))
   return MathUtil.normalisedTheta(phi)

def angleToOwnGoal(absCoord):
   phi = Vector2D.angleBetween(OWN_GOAL_BEHIND_CENTER, Vector2D.makeVector2DCopy(absCoord))
   return MathUtil.normalisedTheta(phi)

def angleToBallToGoal(absCoord):
  ball = blackboard.localisation.ballPos
  ballRR = blackboard.localisation.ballPosRR
  goalDir = angleToGoal(ball)
  return MathUtil.normalisedTheta(goalDir - (absCoord.theta + ballRR.heading))

def angleToBallToOwnGoal(absCoord):
  ball = blackboard.localisation.ballPos
  ballRR = blackboard.localisation.ballPosRR
  goalDir = angleToOwnGoal(ball)
  return MathUtil.normalisedTheta(goalDir - (absCoord.theta + ballRR.heading))

def angleToTeamBallToGoal(teamBallPos, robotPos):
  heading = MathUtil.normalisedTheta(math.atan2(teamBallPos.y - robotPos.y, teamBallPos.x - robotPos.x))
  goalDir = angleToGoal(teamBallPos)
  return MathUtil.normalisedTheta(goalDir - (robotPos.theta + heading))

def angleToGoalLeft(absCoord):
   phi = Vector2D.angleBetween(ENEMY_GOAL_INNER_LEFT, Vector2D.makeVector2DCopy(absCoord))
   return MathUtil.normalisedTheta(phi)

def angleToGoalRight(absCoord):
   phi = Vector2D.angleBetween(ENEMY_GOAL_INNER_RIGHT, Vector2D.makeVector2DCopy(absCoord))
   return MathUtil.normalisedTheta(phi)

def angleSubtendedByPoints(from_coord, to_coord_a, to_coord_b):
   aVector = to_coord_a.minus(from_coord)
   bVector = to_coord_b.minus(from_coord)
   return MathUtil.angleDiff(aVector.heading(), bVector.heading())

def angleSubtendedByGoalPosts(from_coord):
   return angleSubtendedByPoints(from_coord, ENEMY_GOAL_INNER_LEFT, ENEMY_GOAL_INNER_RIGHT)

def angleToGoalShot(ballPosAbs, robotPos, robotObstacles, commitToKick):

   goalRight = angleToGoalRight(ballPosAbs)
   goalLeft = angleToGoalLeft(ballPosAbs)
   pointsOfAim = [[angleToGoal(ballPosAbs), -1.0, Kick.MIDDLE],
                  [goalLeft - math.radians(10), -1.0, Kick.LEFT],
                  [goalRight + math.radians(10), -1.0, Kick.RIGHT]]

   # If we've committed to a kick already
   if Kick.MIDDLE <= commitToKick <= Kick.RIGHT:
      return pointsOfAim[commitToKick]

   # If goal angle is so fine that we can only kick for the middle anyway
   if pointsOfAim[Kick.LEFT][0] < pointsOfAim[Kick.MIDDLE][0] or pointsOfAim[Kick.RIGHT][0] > pointsOfAim[Kick.MIDDLE][0]:
      return pointsOfAim[Kick.MIDDLE]

   nearestDist = 10000
   result = pointsOfAim[Kick.MIDDLE]
   for i in range(0,len(robotObstacles)):
      # Find position of the robots in ball relative terms
      ball = (ballPosAbs.x, ballPosAbs.y, 0.0)
      rr = MathUtil.absToRr(ball, MathUtil.rrToAbs(robotPos, robotObstacles[i].rr))
      dist = rr[0]
      heading = rr[1]
      if pointsOfAim[Kick.LEFT][0] > heading > pointsOfAim[Kick.RIGHT][0]:
         if dist < nearestDist:
            nearestDist = dist
            if heading > pointsOfAim[Kick.MIDDLE][0]:
               # Robot is to the left
               #print "Aiming right"
               result = pointsOfAim[Kick.RIGHT]
            else:
               #print "Aiming left"
               # Robot is to the right
               result = pointsOfAim[Kick.LEFT]
   return result

def myAngleToGoalShot(commitToKick=Kick.MIDDLE):
   goalAngle = angleToGoalShot(Global.ballWorldPos(), Global.myPose(), blackboard.localisation.robotObstacles, commitToKick)
   return (MathUtil.normalisedTheta(goalAngle[0] - Global.myHeading()), goalAngle[2])

def ballAngleToPoint(point):
   ballPos = blackboard.localisation.ballPos
   return MathUtil.normalisedTheta(math.atan2(point[1] - ballPos.y, point[0] - ballPos.x))

def isAlignedWithGoal(angle=0, safety=0.5):
   SAFETY_MARGIN = safety
   robotPos = blackboard.localisation.robotPos
   leftGoal = ballAngleToPoint((Constants.FIELD_LENGTH/2, SAFETY_MARGIN * Constants.GOAL_WIDTH/2)) - robotPos.theta
   rightGoal = ballAngleToPoint((Constants.FIELD_LENGTH/2, SAFETY_MARGIN * -Constants.GOAL_WIDTH/2)) - robotPos.theta

   #adjust for shoot angle
   leftGoal -= angle
   rightGoal -= angle

   # Return True if they are on opposite sides of 0.
   return (leftGoal > 0 > rightGoal) or (leftGoal < 0 < rightGoal)


#sharing this for goalie and striker to know where threshold is
def isInGoalBox(absPos, buffx=0, buffy=0, isStriker=False) :
   if isStriker : (buffx, buffy) = (300, 200)
   return (absPos.x < -(robot.FIELD_LENGTH/2 - robot.GOAL_BOX_LENGTH) + buffx
           and abs(absPos.y) < (robot.GOAL_BOX_WIDTH/2) + buffy)

def addRrToRobot(robotPos, rx, ry):
   x = robotPos.x + math.cos(robotPos.theta) * rx - math.sin(robotPos.theta) * ry
   y = robotPos.y + math.sin(robotPos.theta) * rx + math.cos(robotPos.theta) * ry
   return x, y


def getBallIntersectionWithRobot(maintainCanSeeBall = True):
   intervalInSeconds = 1
   numSecondsForward = 1.0 # Estimate the ball position up to 1 second away
   numIterations = int(round(numSecondsForward / intervalInSeconds))
   FRICTION = 0.9 # friction per second
   FRICTION_PER_ITERATION = FRICTION ** intervalInSeconds

   ballVel = Global.ballWorldVelHighConfidence()
   ballPos = Global.ballWorldPos()
   myHeading = Global.myHeading()

   # If he ball is moving slowly, just chase the ball directly
   if ballVel.isShorterThan(10.0):
      return ballPos

   # Dont bother chasing a moving ball if its quite close.
   if Global.ballDistance() < 600.0:
      return ballPos

   ballVel.scale(intervalInSeconds)

   robotPos = Global.myPos()
   
   interceptPoint = ballPos
   bestChasePoint = ballPos.clone()

   seconds = 0.0
   for i in xrange(0, numIterations):
      seconds += intervalInSeconds

      interceptPoint.add(ballVel)
      ballVel.scale(FRICTION_PER_ITERATION)

      toIntercept = interceptPoint.minus(robotPos)
      toInterceptHeading = math.atan2(toIntercept.y, toIntercept.x)
   
      # How far we need to turn to point at the interceptPoint
      toInterceptTurn = abs(MathUtil.normalisedTheta(toInterceptHeading - myHeading))

      timeToTurn = toInterceptTurn / TURN_RATE
      timeToWalk = toIntercept.length() / WALK_RATE

      canReach = (timeToTurn + timeToWalk) <= seconds
      
      # Calculate difference in heading to the current ball position and the intersect position
      # to make sure we don't turn too far and lose sight of the ball
      v1 = interceptPoint.minus(robotPos).normalised()
      v2 = ballPos.minus(robotPos).normalised()
      heading = v1.absThetaTo(v2)

      if maintainCanSeeBall and heading > math.radians(75):
         return bestChasePoint

      if canReach:
         return bestChasePoint
      else:
         bestChasePoint = Vector2D.makeVector2DCopy(interceptPoint)

   return bestChasePoint

def globalPoseToRobotRelativePose(globalVector, globalHeading):

   robotPos = Global.myPos()
   robotHeading = Global.myHeading()

   rrVector = globalVector.minus(robotPos).rotate(-robotHeading)
   rrHeading = globalHeading - robotHeading

   return rrVector, rrHeading

def globalPointToRobotRelativePoint(globalVector):

   robotPos = Global.myPos()
   robotHeading = Global.myHeading()

   return globalVector.minus(robotPos).rotate(-robotHeading)

def nearFieldBorder():
   pos = Global.myPos()
   # print "Near Field edge - %5d > %5d , %5d > %5d? " % (pos.x, Constants.FIELD_LENGTH/2 - 100, pos.y, Constants.FIELD_WIDTH/2 - 100),
   if abs(pos.x) > Constants.FIELD_LENGTH/2 - 300 or abs(pos.y) > Constants.FIELD_WIDTH/2 - 300:
      # print "Yes."
      return True
   #print "No."
   return False

def angleIntoField():
   toCentre = globalPointToRobotRelativePoint(Vector2D.Vector2D(0,0))
   return toCentre.heading()
