import math
import Log
import robot

blankRGB = robot.rgb()

def amIFallen(blackboard):
   myPlayerNumber = blackboard.gameController.player_number
   return blackboard.receiver.incapacitated[myPlayerNumber - 1]

def head(yaw=0, pitch=0, isYawRelative=False, yawSpeed=1.0, pitchSpeed=1.0):
   return robot.HeadCommand(yaw, pitch, isYawRelative, yawSpeed, pitchSpeed)

def stand(power=0.1):
   return walk(0, 0, 0, power, bend=0)

def crouch(power=0.4):
   return walk(0, 0, 0, power, bend=1)

robotName = ""
try:
   # Quick hack to access the robot's hostname, so we can make exception cases for helo.
   hostname_file = open("/etc/hostname")
   robotName = hostname_file.read()
   robotName = robotName.lstrip(" ").rstrip(" ").replace("\n", "").lower()
   hostname_file.close()
except:
   robotName = "secretAgent"

Log.info("robotName is:\n%s", robotName)


def walk(forward=0, left=0, turn=0, power=0, bend=1, speed=1.0, foot=robot.Foot.LEFT, isFast=False):
   return robot.BodyCommand(robot.ActionType.WALK, int(forward), int(left), float(turn), float(power), float(bend), float(speed), float(speed), foot, bool(isFast), False)

def kick(power=1.0, kickDirection=0.0, foot=robot.Foot.LEFT, turn=0, misalign=False, turnThreshold=math.radians(20)):
   return robot.BodyCommand(robot.ActionType.KICK, 0, 0, float(turn), power, 0.0, turnThreshold, kickDirection, foot, False, misalign)

def motionCalibrate():
   return robot.BodyCommand(robot.ActionType.MOTION_CALIBRATE, 0, 0, 0, 0.0, 0, 0, 0.0, robot.Foot.LEFT, False, False)

def standStraight():
   return robot.BodyCommand(robot.ActionType.STAND_STRAIGHT, 0, 0, 0, 0.0, 0, 0, 0.0, robot.Foot.LEFT, False, False)

def dribble(foot=robot.Foot.LEFT, turn=0):
   return robot.BodyCommand(robot.ActionType.DRIBBLE, 0, 0, float(turn), 1, 0.0, math.radians(20.0), 0.0, foot, False, False)

def goalieSit():
   return robot.BodyCommand(robot.ActionType.GOALIE_SIT, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, robot.Foot.LEFT, False, False)

def goalieDiveRight():
   return robot.BodyCommand(robot.ActionType.GOALIE_DIVE_RIGHT, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, robot.Foot.LEFT, False, False)

def goalieCentre():
   return robot.BodyCommand(robot.ActionType.GOALIE_CENTRE, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, robot.Foot.LEFT, False, False)

def goalieUncentre():
   return robot.BodyCommand(robot.ActionType.GOALIE_UNCENTRE, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, robot.Foot.LEFT, False, False)

def goalieDiveLeft():
   return robot.BodyCommand(robot.ActionType.GOALIE_DIVE_LEFT, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, robot.Foot.LEFT, False, False)

def goalieStand():
   return robot.BodyCommand(robot.ActionType.GOALIE_STAND, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, robot.Foot.LEFT, False, False)

def defenderCentre():
  return robot.BodyCommand(robot.ActionType.DEFENDER_CENTRE, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, robot.Foot.LEFT, False, False)

def goaliePickup():
   return robot.BodyCommand(robot.ActionType.GOALIE_PICK_UP, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, robot.Foot.LEFT, False, False)

def leds(leye=blankRGB, reye=blankRGB, chest=blankRGB, lfoot=blankRGB, rfoot=blankRGB):
   return robot.LEDCommand(leye, reye, chest, lfoot, rfoot)

def compose(head=head(), body=walk(), leds=leds()):
   return robot.All(head, body, leds, 0.0)
