import math
from Vector2D import Vector2D

EPSILON = 0.00001


def angleDiff(angle0, angle1):
  return min(math.fabs(angle0 - angle1), math.fabs(math.fabs(angle0 - angle1) - 2.0*math.pi))

def angleSignedDiff(target, current):
  diff = target - current
  return reduce(
    (lambda a, b: a if math.fabs(a) < math.fabs(b) else b),
    [diff, diff + math.pi*2, diff - math.pi*2]
  )

def normalisedTheta(theta):
   r = math.fmod(theta - math.pi, 2*math.pi)
   if r > 0:
      return r - math.pi
   else:
      return r + math.pi


def clipToRange(var, min, max):
  if var < min:
    return min
  elif var > max:
    return max
  else:
    return var


# Check if two line segments intersect.
# All inputs are of type Vector2D, returns a boolean.
def doLinesIntersect(start1, end1, start2, end2):
  denominator = (end2.y - start2.y) * (end1.x - start1.x) - \
      (end2.x - start2.x) * (end1.y - start1.y)

  # lines are parallel.
  if math.fabs(denominator) < EPSILON:
    return False

  numerator1 = (end2.x - start2.x) * (start1.y - start2.y) - \
      (end2.y - start2.y)*(start1.x - start2.x)

  numerator2 = (end1.x - start1.x) * (start1.y - start2.y) - \
      (end1.y - start1.y) * (start1.x - start2.x)

  u1 = numerator1 / denominator
  u2 = numerator2 / denominator

  # intesect point falls outside the segment.
  if u1 < -EPSILON or u1 > 1.0 + EPSILON or u2 < -EPSILON or u2 > 1.0 + EPSILON:
    return False

  return True


# Find the intersetion point of two line segments.
# All inputs are of type Vector2D, returns None if no intersection, otherwise a Vector2D
# of intersection point.
def intersectLines(start1, end1, start2, end2):
  denominator = (end2.y - start2.y) * (end1.x - start1.x) - \
      (end2.x - start2.x) * (end1.y - start1.y)

  # lines are parallel.
  if math.fabs(denominator) < EPSILON:
    return None

  numerator1 = (end2.x - start2.x)*(start1.y - start2.y) - \
      (end2.y - start2.y)*(start1.x - start2.x)

  numerator2 = (end1.x - start1.x)*(start1.y - start2.y) - \
      (end1.y - start1.y)*(start1.x - start2.x)

  u1 = numerator1 / denominator
  u2 = numerator2 / denominator

  # intesect point falls outside the segment.
  if u1 < -EPSILON or u1 > 1.0 + EPSILON or u2 < -EPSILON or u2 > 1.0 + EPSILON:
    return None

  return Vector2D(start1.x + u1 * (end1.x - start1.x), start1.y + u1 * (end1.y - start1.y))


# returns a tuple of (Vector2D, Boolean), where the first is the closest point on the line
# to the given point, and the second is the distance to that point.
def pointSegmentDist(point, start, end):
  v = end.minus(start)
  w = point.minus(start)

  c1 = w.dotProduct(v)
  if c1 <= 0.0:
    return (start, w.length())

  c2 = v.dotProduct(v)
  if c2 <= c1:
    return (end, point.minus(end).length())

  b = c1 / c2
  Pb = start.plus(v.multiply(b))

  return (Pb, point.minus(Pb).length())


def rrToAbs(robotPos, rr):
   x =  robotPos.x + math.cos(robotPos.theta + rr.heading) * rr.distance
   y =  robotPos.y + math.sin(robotPos.theta + rr.heading) * rr.distance
   return (x, y)


def absToRr(fromPos, toPos):
   dist = math.hypot(toPos[0]-fromPos[0], toPos[1]-fromPos[1])
   heading = normalisedTheta(math.atan2(toPos[1]-fromPos[1], toPos[0]-fromPos[0])-fromPos[2])
   return (dist, heading)

clamp = lambda n, minn, maxn: max(min(maxn, n), minn)

