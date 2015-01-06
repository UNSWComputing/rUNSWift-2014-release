import robot
import math

FIELD_WIDTH                = robot.FIELD_WIDTH
FIELD_LENGTH               = robot.FIELD_LENGTH
GOAL_BOX_WIDTH             = robot.GOAL_BOX_WIDTH
GOAL_BOX_LENGTH            = robot.GOAL_BOX_LENGTH
CENTER_CIRCLE_DIAMETER     = robot.CENTER_CIRCLE_DIAMETER
GOAL_WIDTH                 = robot.GOAL_WIDTH
BALL_RADIUS                = robot.BALL_RADIUS
GOAL_POST_ABS_X            = robot.GOAL_POST_ABS_X
GOAL_POST_ABS_Y            = robot.GOAL_POST_ABS_Y
GOAL_POST_DIAMETER         = robot.GOAL_POST_DIAMETER
ROBOTS_PER_TEAM            = robot.ROBOTS_PER_TEAM
MARKER_CENTER_X            = robot.MARKER_CENTER_X

# Role enums
ROLE_NONE = 0
ROLE_GOALIE = 1
ROLE_STRIKER = 2
ROLE_DEFENDER = 3
ROLE_MIDFIELDER = 4
ROLE_UPFIELDER = 5

# Head joint limits
MIN_HEAD_YAW = math.radians(-119.5)
MAX_HEAD_YAW = math.radians(119.5)
MIN_HEAD_PITCH = math.radians(-38.5)
MAX_HEAD_PITCH = math.radians(29.5)

# Kicking foot enums
# Note: their specific values are actually important - they are used to flip vectors and stuff
LEFT_FOOT = 1
NO_FOOT = 0
RIGHT_FOOT = -1

# Foot geometry, used for dribblin/approaching the ball
TOE_CENTRE_X = 60
HIP_OFFSET = 50

# LED Colours.
class LEDColour(object):
   off = robot.rgb(False, False, False)
   red = robot.rgb(True, False, False)
   green = robot.rgb(False, True, False)
   blue = robot.rgb(False, False, True)
   yellow = robot.rgb(True, True, False)
   cyan = robot.rgb(False, True, True)
   magenta = robot.rgb(True, False, True)
   white = robot.rgb(True, True, True)

# Game State for GameController.
class GameState(object):
   INITIAL = robot.STATE_INITIAL
   READY = robot.STATE_READY
   SET = robot.STATE_SET
   PLAYING = robot.STATE_PLAYING
   FINISHED = robot.STATE_FINISHED
   INVALID = robot.STATE_INVALID
   PENALISED = robot.STATE_PENALISED
   STATE2_NORMAL = robot.STATE2_NORMAL
   STATE2_PENALTYSHOOT = robot.STATE2_PENALTYSHOOT

KICKOFF_MIN_WAIT = 10 * 1000 * 1000 # Time you have to wait before you can enter the circle in a defensive kickoff.

# Robot dimensions.
ROBOT_DIAM = 450 # Ritwik's estimate. TODO(Ritwik): Check this.

WALK_MAX_TURN = math.radians(50)
