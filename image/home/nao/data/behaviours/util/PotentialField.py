import Global
import Constants

from util.Vector2D import Vector2D, angleBetween
from math import cos, degrees

MAX_ATT = 4
MAX_REP = 1000
EPSILON = 1

#How to use: refer to wingman

#input abs pos
def getAttractiveField(targetPos, attScale=1):
    myPos = Global.myPos()
    dist = myPos.minus(targetPos).length()
    return myPos.minus(targetPos).multiply(MAX_ATT * attScale / dist)

#distThreshold - maximum distance to feel the field
def getRepulsiveField(obsPos, repScale=1, distThresh=1400):
    Urep = Vector2D(0, 0)
    myPos = Global.myPos()
    dist = myPos.minus(obsPos).length()
    dist = max(EPSILON, dist - Constants.ROBOT_DIAM/2)
    # find new obsPos with modified dist
    direction = obsPos.minus(myPos).normalised()
    obsPos = myPos.plus(direction.multiply(dist))
    if dist <= distThresh :
        Urep = myPos.minus(obsPos).multiply(MAX_REP * repScale * (1/distThresh - 1/dist)/dist)
    return Urep

class RelativePotentialField(object):
   def __init__(self):
      self.uAtt = Vector2D(0, 0)
      self.uRep = Vector2D(0, 0)
      self.uFinal = Vector2D(0, 0)

   def setTarget(self, target):
      self.target = target

   def addAttractionTo(self, pos, weight=1.0):
      dist = pos.length()
      self.uAtt = self.uAtt.plus(Vector2D(pos.x, pos.y).normalised().multiply(500))

   def addRepulsionFrom(self, pos, weight=1.0, objectDiam=Constants.ROBOT_DIAM):
      cutoff = self.target.length()
      distToObsOuter = max(EPSILON, pos.length() - objectDiam/2)
      if distToObsOuter > cutoff:
         return
      # print "%30s: %5.0f, %5.0f" % ("Obs | RelToTarget", degrees(pos.heading()), degrees(pos.heading()-self.target.heading()))
      perpLen = distToObsOuter * cos(abs(angleBetween(self.target, pos)))
      # print "%30s: %5.0f" % ("PerpLen", perpLen)

      #print "%30s: (%5d < %5.0f)" % ("Avoiding with force", force.length(), degrees(force.heading()))
      forceDirection = self.target.normalised().multiply(perpLen).minus(pos).normalised()
      # print "%30s: (%5d < %5.0f)" % ("Avoiding with forceDirection", forceDirection.length(), degrees(forceDirection.heading()))
      # print "%30s: %5d - %5d = %5d" % ("LengthDiff", cutoff, pos.length(), cutoff-pos.length())
      force = forceDirection.normalised(min(500, cutoff) * (cutoff - pos.length())/cutoff)
      # print "%30s: (%5d < %5.0f)" % ("Avoiding with force", force.length(), degrees(force.heading()))
      # obsOuterPoint = Vector2D(pos.x, pos.y).normalised().multiply(distToObsOuter)

      # repulsiveVector = obsOuterPoint.multiply(-1) # The vector toward us.
      # # Find a scaling value using gradient descent.
      # relativeLength = max(500 - repulsiveVector.length()/2.0, 0.0)
      # # repFactor = MAX_REP * weight * (1/cutoff - 1/distToObsOuter)/distToObsOuter
      # # Scale the repulsion vector.
      # scaledRepulsiveVector = repulsiveVector.normalised().multiply(relativeLength)
      # # Add the repulsion vector to our total repulsion.
      self.uRep = self.uRep.plus(force)

   def netAttraction(self):
      return self.uAtt.plus(self.uRep)

   def netAttractionNormalised(self):
      return self.netAttraction().normalised()