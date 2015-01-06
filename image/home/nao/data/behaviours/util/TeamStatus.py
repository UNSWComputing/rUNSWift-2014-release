import robot
import Constants
import math
from util import MathUtil
from util import FieldGeometry
from util import Vector2D

blackboard = None
fieldPlayerIndices = None
# This method should be called at the start of every Behaviour frame, as soon as we
# enter Python.
def update(newBlackboard):
   global blackboard
   blackboard = newBlackboard
   global fieldPlayerIndices
   if fieldPlayerIndices is None :
       fieldPlayerIndices = []
       for i in xrange(1, numTeammates()):
           if myPlayerNumber() != getTeammatePlayerNum(i) :
               fieldPlayerIndices.append(i)

# The number of frames that none of the robots have seen the ball for for us to consider
# the ball lost to the team, and should trigger a "distributed find ball" behaviour.
NUM_LOST_FRAMES_TEAM_BALL_LOST = 80

NUM_LOST_FRAMES_CAN_SEE_BALL = 80

#Boolean. Returns whether nobody on the team has recently seen the ball.
def hasTeamLostBall():
   for i in xrange(robot.ROBOTS_PER_TEAM):
      if isTeammateActive(i) and \
         blackboard.receiver.data[i].lostCount < NUM_LOST_FRAMES_TEAM_BALL_LOST:
         return False

   else:
      return True


def numTeammatesSeeBall():
   count = 0;
   for i in xrange(robot.ROBOTS_PER_TEAM):
      if isTeammateActive(i) and \
         blackboard.receiver.data[i].lostCount < NUM_LOST_FRAMES_CAN_SEE_BALL:
         count += 1
   return count


def getTimeToReachBall(index):
   return blackboard.receiver.data[index].behaviourSharedData.timeToReachBall

def getTimeToReachUpfielder(index):
   return blackboard.receiver.data[index].behaviourSharedData.timeToReachUpfielder

def getTimeToReachMidfielder(index):
   return blackboard.receiver.data[index].behaviourSharedData.timeToReachMidfielder

def getTimeToReachDefender(index):
   return blackboard.receiver.data[index].behaviourSharedData.timeToReachDefender

def myPlayerNumber():
   return blackboard.gameController.player_number


def getTeammatePlayerNum(index):
   return index + 1
   #return blackboard.receiver.data[index].playerNum

def getTeammatePose(index):
   pose = blackboard.receiver.data[index].robotPos
   return (Vector2D.Vector2D(pose.x, pose.y), pose.theta)

def getTeammateRole(index):
   return blackboard.receiver.data[index].behaviourSharedData.currentRole

def getFirstOfRole(role):
   for i in range(Constants.ROBOTS_PER_TEAM):
      if blackboard.receiver.data[i].behaviourSharedData.currentRole == role:
         return i
   return -1

def getSharedKickoffTarget():
   side_of_field = 1
   upfielder = getFirstOfRole(Constants.ROLE_UPFIELDER)
   if upfielder >= 0:
      upfielder_pos, _ = getTeammatePose(upfielder)
      side_of_field = math.copysign(1, upfielder_pos.y)
   else:
      midfielder = getFirstOfRole(Constants.ROLE_MIDFIELDER)
      if midfielder >= 0:
         midfielder_pos, _ = getTeammatePose(midfielder)
         side_of_field = math.copysign(1, midfielder_pos.y)

   # Now we have a side of field to kick to.
   return Vector2D.Vector2D(2400, 1200 * side_of_field)

def countNumTeammatesHaveRole(targetRole):
   count = 0
   for i in xrange(0, numTeammates()):
      if isTeammateActive(i) and \
         getTeammatePlayerNum(i) != myPlayerNumber() and \
         blackboard.receiver.data[i].behaviourSharedData.currentRole == targetRole:
         count += 1
       
   return count


# Integer. Returns the number of teammates robots who have current role set to Striker.
def countNumStrikers(onlyCountIfSeeBall = False):
   if onlyCountIfSeeBall:
      count = 0
      for i in xrange(0, numTeammates()):
         if isTeammateActive(i) and \
            getTeammatePlayerNum(i) != myPlayerNumber() and \
            blackboard.receiver.data[i].behaviourSharedData.currentRole == Constants.ROLE_STRIKER and \
            blackboard.receiver.data[i].lostCount < NUM_LOST_FRAMES_CAN_SEE_BALL:
            count += 1
       
      return count
   else:
      return countNumTeammatesHaveRole(Constants.ROLE_STRIKER)


def teammateLostBallCount(index):
   return blackboard.receiver.data[index].lostCount

# Integer. Returns the number of teammates robots who have current role set to Defenders.
def countNumDefenders():
   return countNumTeammatesHaveRole(Constants.ROLE_DEFENDER)


# Integer. Returns the number of teammates robots who have current role set to MidFielders.
def countNumMidfielders():
   return countNumTeammatesHaveRole(Constants.ROLE_MIDFIELDER)


# Integer. Returns the number of teammates robots who have current role set to Forward.
def countNumUpfielders():
   return countNumTeammatesHaveRole(Constants.ROLE_UPFIELDER)


# Boolean. Returns whether the wireless is currently down.
def isWirelessDown():
   return False


# Boolean. Returns the number of teammate robots, including the goalie, that are
# currently on the field, communicating on wireless, and notpenalised.
def countActiveTeammates():
   count = 0
   for i in xrange(0, numTeammates()):
      if not blackboard.receiver.incapacitated[i]:
         count += 1

   return count

def isTeammateActive(index):
   if getTeammatePlayerNum(index) == myPlayerNumber():
      return True
   else:
      return not blackboard.receiver.incapacitated[index]

def numTeammates():
   return len(blackboard.receiver.data)

def getFieldPlayerIndices():
   return fieldPlayerIndices

def isGoalieAttacking():
   for i in xrange(0, numTeammates()):
      if isTeammateActive(i) and \
         blackboard.receiver.data[i].behaviourSharedData.goalieAttacking:
         return True
   else:
      return False

def myTeamColor():
   return blackboard.gameController.our_team.teamColour

def isTeamMate(unknownRobot):
   myTeam = myTeamColor()
   return (unknownRobot.type == robot.RobotInfoType.rBlue and myTeam == robot.TEAM_BLUE) or \
          (unknownRobot.type == robot.RobotInfoType.rRed and myTeam == robot.TEAM_RED)

def onBlueTeam():
   return myTeamColor() == robot.TEAM_BLUE

def onRedTeam():
   return myTeamColor() == robot.TEAM_RED

def ourKickOff():
   return myTeamColor() == blackboard.gameController.data.kickOffTeam

def playerNumber():
   return blackboard.gameController.player_number

def teamNumber():
   return blackboard.gameController.our_team.teamNumber

def lonelyPlayer():
   """Lonely Player.

   Return whether the current player is lonely, i.e, hasn't heard from his comrades in a while.
   """

   incapacitated = blackboard.receiver.incapacitated
   for i in xrange(robot.ROBOTS_PER_TEAM):
      if i == blackboard.gameController.player_number - 1:
         # Skip myself.
         continue
      if not incapacitated[i]:
         return False
   return True

def numActivePlayers():
   incapacitated = blackboard.receiver.incapacitated
   count = 0
   for i in range(robot.ROBOTS_PER_TEAM):
      if not incapacitated[i]: count+=1

   return count

def teamCanSeeBall(maxLostCount=30*3, maxLastHeardFrom=10):
   """Team Can See Ball.

   Return whether the team can see the ball, i.e, there's at least one robot (not self) which has a lost count less
   than maxLostCount which we've heard from in less than maxLastHeardFrom seconds.
   """

   teamData = blackboard.receiver.data
   lastReceived = blackboard.receiver.lastReceived
   now =  blackboard.vision.timestamp / 1000000.0
   for i in xrange(robot.ROBOTS_PER_TEAM):
      if i == blackboard.gameController.player_number - 1:
         continue

      # If a player we've heard from in the last 5 seconds had a lost count < 3 seconds (team can see the ball).
      if teamData[i].lostCount < maxLostCount and (now - lastReceived[i]) < maxLastHeardFrom:
         return True

   return False

# This is to detect if we see a teammate closer to the ball then us
def teammateMinWeightedDistToBall():
   minDist = robot.FULL_FIELD_LENGTH
   ballPosAbs = blackboard.localisation.ballPos
   lostCount = blackboard.localisation.ballLostCount
   robotPos = blackboard.localisation.robotPos
   if lostCount < 100:
      robotObs = blackboard.localisation.robotObstacles
      for i in range(0,len(robotObs)):
         isFriend = isTeamMate(robotObs[i])
         # Find position of the robots in absolute terms and discard if near the end of field (false positive)
         robotObsAbs = MathUtil.rrToAbs(robotPos, robotObs[i].rr)
         if robotObsAbs[0] > 2800:
            isFriend = False
         if isFriend:
            dist = math.hypot(ballPosAbs.x - robotObsAbs[0], ballPosAbs.y - robotObsAbs[1])
            if dist < minDist:
               minDist = dist
   return minDist

def minWeightedDistanceToTeamBall():
   incapacitated = blackboard.receiver.incapacitated
   minDist = robot.FULL_FIELD_LENGTH
   minDistPlayer = blackboard.gameController.player_number
   teamData = blackboard.receiver.data
   teamBallPos = blackboard.localisation.teamBall.pos
   for i in xrange(robot.ROBOTS_PER_TEAM):
      robotPos = teamData[i].robotPos
      lostCount = teamData[i].lostCount
      if (i+1) != blackboard.gameController.player_number and not incapacitated[i] and i+1 != 1:
         if not math.isnan(robotPos.x):
            turnAngle = abs(FieldGeometry.angleToTeamBallToGoal(teamBallPos, robotPos))
            weightedDist = math.hypot(teamBallPos.x - robotPos.x, teamBallPos.y - robotPos.y) + turnAngle*(1000.0/math.pi)
            if weightedDist < minDist:
               minDist = weightedDist
               minDistPlayer = i+1
   return minDist, minDistPlayer


def closestToTeamBall():

   teamBall = blackboard.localisation.teamBall
   if teamBall.contributors == 0:
      return False

   myPos = blackboard.localisation.robotPos
   turnAngle = abs(FieldGeometry.angleToTeamBallToGoal(teamBall.pos, myPos))
   myDist = math.hypot(teamBall.pos.x - myPos.x, teamBall.pos.y - myPos.y) + turnAngle*(1000.0/math.pi)
   otherDist = minWeightedDistanceToTeamBall()[0]
   return myDist < (otherDist * 1.4)
