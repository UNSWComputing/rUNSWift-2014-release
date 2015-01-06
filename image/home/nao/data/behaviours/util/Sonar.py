import robot
from util.Hysteresis import Hysteresis
WINDOW_LENGTH = 30
NEARBY_HIT_DECAY_FRAMES = 10
MIN_FRAMES_TO_SEE_SONAR_OBS = 3
blackboard = None
sonarWindows = [[],[],[]]
nearbySonarHysteresis = [
   Hysteresis(-MIN_FRAMES_TO_SEE_SONAR_OBS,NEARBY_HIT_DECAY_FRAMES),
   Hysteresis(-MIN_FRAMES_TO_SEE_SONAR_OBS,NEARBY_HIT_DECAY_FRAMES)]
# This method should be called at the start of every Behaviour frame, as soon as we
# enter Python.
def update(newBlackboard):
   global blackboard
   blackboard = newBlackboard
   updateNearbySonarValues()

def getNearbySonar(i):
   return nearbySonarHysteresis[i].value

def hasNearbySonarObject(i):
   return nearbySonarHysteresis[i].value > 0

def updateNearbySonarValues():
   sonarLen = len(blackboard.motion.sensors.sonar)
   if sonarLen < 11:
      nearbySonarHysteresis[0].down()
      nearbySonarHysteresis[1].down()
   for i, sonarVal in enumerate([blackboard.motion.sensors.sonar[0], blackboard.motion.sensors.sonar[10]]):
      if sonarVal < 0.4:
         if nearbySonarHysteresis[i].value == 0:
            nearbySonarHysteresis[i].resetMax()
         else:
            nearbySonarHysteresis[i].up()
      else:
         nearbySonarHysteresis[i].down()

def updateWindow():
   global blackboard
   global sonarWindows
   closest = getNearestSonarReadings()
   for index, window in enumerate(sonarWindows):
      item = closest[index] if closest[index] is not None else 10000
      window.insert(0, item)
      if len(window) > WINDOW_LENGTH:
         window.pop()


def printWindows():
   print "SonarWindows: \n%s\n%s\n%s" % (sonarWindows[0][:10], sonarWindows[1][:10], sonarWindows[2][:10])

def getSmoothedValue(i):
   global sonarWindows
   inOrder = sorted(sonarWindows[i][:10])
   return inOrder[len(inOrder)/2] # median.
   # return sum(sonarWindows[i][:10]) / len(sonarWindows[i][:10])

def getSmoothedValues():
   global  sonarWindows
   return [getSmoothedValue(i) for i in range(len(sonarWindows))]

def getNearestSonarReadings():
   global blackboard
   obstacles = blackboard.kinematics.sonarFiltered
   nearestForSonar = []
   for i in range(len(obstacles)):
      closest = None
      for j in range(len(obstacles[i])):
         if closest is None or obstacles[i][j] < closest:
            closest = obstacles[i][j]
      nearestForSonar.append(closest)
   return nearestForSonar

def getNearestCapped(cap):

   return [dist if dist is not None and dist < cap else cap for dist in getNearestSonarReadings()]

def getFilteredSonarArrays():
   full_sonar_list = []
   obstacles = blackboard.kinematics.sonarFiltered
   for i in range(len(obstacles)):
      sonar_list = []
      for j in range(len(obstacles[i])):
         sonar_list.append(obstacles[i][j])
      full_sonar_list.append(sonar_list)
   return full_sonar_list

def getRawSonarArray():
   sonar_list = []
   for i in range(len(blackboard.motion.sensors.sonar)):
      sonar_list.append(blackboard.motion.sensors.sonar[i])
   return sonar_list
   #return robot.floatArray_frompointer(blackboard.motion.sensors.sonar)
