import robot
import math
from util.Vector2D import Vector2D
from Constants import GameState


# Object caches.
_robotObstacles = None
_ballWorldPos = None
_myPose = None
_ballLostCount = None

blackboard = None
_ticksSinceStart = 0
def update(newBlackboard):
   global blackboard
   blackboard = newBlackboard

   global _robotObstacles
   _robotObstacles = None

   global _ticksSinceStart
   _ticksSinceStart +=1

   global _ballWorldPos
   _ballWorldPos = blackboard.localisation.ballPos

   global _myPose
   _myPose = blackboard.localisation.robotPos

   global _ballLostCount
   _ballLostCount = blackboard.localisation.ballLostCount

# Vector2D world coordinates of the ball
# TODO: consider using a "teamball" type thing here if our confidence
# is very low.
def ballWorldPos():
   return Vector2D(_ballWorldPos.x, _ballWorldPos.y)

# Vector2D robot relative coordinates of the ball.
def ballRelPos():
   ballPosRRC = blackboard.localisation.ballPosRRC
   return Vector2D(ballPosRRC.x, ballPosRRC.y)


# Vector2D world velocity of the ball, in mm/s
def ballWorldVel():
   ballVel = blackboard.localisation.ballVel
   return Vector2D(ballVel.x, ballVel.y)


# Lower values for the minConfidence means closer to the mean estimated velocity, but
# also results in higher uncertainty that the velocity is truly that fast.
def ballWorldVelHighConfidence(minConfidence = 0.15):
   velocity = ballWorldVel()
   speed = velocity.length()

   uncertainty = blackboard.localisation.ballVelEigenvalue * minConfidence
   if uncertainty > speed:
      return Vector2D(0.0, 0.0)

   velocity.normalise()
   speed = max(speed - uncertainty, 0.0)
   velocity.scale(speed)

   return velocity


# Vector2D robot relative velocity of the ball, in mm/s
def ballRelVel():
   ballVelRRC = blackboard.localisation.ballVelRRC
   return Vector2D(ballVelRRC.x, ballVelRRC.y)


#Float. Returns the Euclidian distance to the ball.
def ballDistance():
   return blackboard.localisation.ballPosRR.distance


def ballHeading():
   return blackboard.localisation.ballPosRR.heading


def myPose():
   return _myPose


# Vector2D robot world coordinates
def myPos():
   return Vector2D(_myPose.x, _myPose.y)


# Float of the robot world relative heading, in radians.
def myHeading():
   return _myPose.theta


# Boolean of whether the robot can currently see the ball.
def canSeeBall():
   return _ballLostCount == 0


def amILost():
   return False

def ballPosUncertainty():
   return math.sqrt(blackboard.localisation.ballPosUncertainty)

def isBallLost():
   # TODO: these numbers are not necessarily appropriate
   return blackboard.localisation.ballPosUncertainty > (5000.0 * 5000.0)

def amILiningUpKick():
   return blackboard.behaviour.behaviourSharedData.doingBallLineUp


def offNetwork():
   return False


# Robot Obstacles.
def robotObstaclesList():
   # Convert to an easier to use list (boost wrapping for iterators is a bitch so you can't yet do
   # for a in alist with arrays on the blackboard.
   global _robotObstacles
   if _robotObstacles is not None:
      return _robotObstacles
   _robotObstacles = []
   for i in range(len(blackboard.localisation.robotObstacles)):
      _robotObstacles.append(blackboard.localisation.robotObstacles[i])
   return _robotObstacles


def ballLostFrames():
   return _ballLostCount


def EmptyBehaviourRequest():
   b = robot.BehaviourRequest()
   b.actions = robot.All()
   return b

def currentVisionTime():
   return blackboard.vision.timestamp

def iAmPenalised():
   return blackboard.gameController.data.state == GameState.PENALISED

def iAmPenalisedOrInitial():
   return blackboard.gameController.data.state in [GameState.PENALISED, GameState.INITIAL]

def usingGameControllerSkill():
   return blackboard.behaviour.skill == "GameController"

def getVisionPosts():
   posts = []
   for i in range(len(blackboard.vision.posts)):
      posts.append(blackboard.vision.posts[i])
   return posts

robotName = ""
try:
   # Quick hack to access the robot's hostname, so we can make exception cases for helo.
   hostname_file = open("/etc/hostname")
   robotName = hostname_file.read()
   hostname_file.close()
except:
   pass

def ticksSinceStart():
   global _ticksSinceStart
   return _ticksSinceStart