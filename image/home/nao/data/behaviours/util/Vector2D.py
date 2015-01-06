import math

class Vector2D:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __str__(self):
        return "Vec2D(" + str(self.x) + "," + str(self.y) + ")"

    def isShorterThan(self, val):
      return self.length2() < (val*val)

    def isLongerThan(self, val):
      return not self.isShorterThan(val)

    def clone(self):
        return Vector2D(self.x, self.y)

    def length(self):
        return math.sqrt(self.x*self.x + self.y*self.y)

    def length2(self):
        return (self.x*self.x + self.y*self.y)

    def heading(self):
        return math.atan2(self.y, self.x)

    def normalise(self, length = 1):
        if self.length2() == 0:
            return self
        self.scale(length / self.length())
        return self

    def normalised(self, length = 1):
        return self.clone().normalise(length)

    def dotProduct(self, var):
        return self.x * var.x + self.y * var.y

    def absThetaTo(self, var):
        dp = self.dotProduct(var)
        if dp > 1.0:
            return 0.0
  
        if dp < -1.0:
            return math.pi

        return math.fabs(math.acos(dp))

    def rotate(self, theta):
        sinTheta = math.sin(theta)
        cosTheta = math.cos(theta)

        newX = (cosTheta * self.x) - (sinTheta * self.y)
        newY = (sinTheta * self.x) + (cosTheta * self.y)

        self.x = newX
        self.y = newY
        return self

    def rotated(self, theta):
        result = Vector2D(self.x, self.y)
        result.rotate(theta)
        return result

    def distanceTo(self, var):
        dx = self.x - var.x
        dy = self.y - var.y
        return math.sqrt(dx*dx + dy*dy)

    def minus(self, var):
        return Vector2D(self.x - var.x, self.y - var.y)

    def subtract(self, var):
        self.x -= var.x
        self.y -= var.y
        return self

    def plus(self, var):
        return Vector2D(self.x + var.x, self.y + var.y)

    def add(self, var):
        self.x += var.x
        self.y += var.y
        return self

    def multiply(self, var):
        return Vector2D(var * self.x, var * self.y)

    def scale(self, s):
        self.x *= s
        self.y *= s
        return self

    # Scales the current vector maximally such that its dimensions in either direction do not exceed those of `rect`
    def fitLimits(self, rect, grow = True):
        
        scale_x = abs(rect.x / float(self.x)) if self.x else 1e10
        scale_y = abs(rect.y / float(self.y)) if self.y else 1e10

        scale = min(scale_x, scale_y)
        if not grow: scale = min(1.0, scale)
        self.scale(scale)

        return self

    # Scales the current vector such that it fits into an ellipse with major and minor radii x and y respectively
    def fitEllipse(self, ellipse, grow = True):

        ratio = ellipse.y / float(ellipse.x)
        elliptical_length = math.sqrt(self.y*self.y + self.x*self.x * ratio*ratio)

        scale = ellipse.y / elliptical_length
        if not grow: scale = min(1.0, scale)

        self.scale(scale)
        return self

# basically explicit constructors
def makeVector2DCopy(src):
    return Vector2D(src.x, src.y)

def makeVector2DFromTuple(src):
    return Vector2D(src[0], src[1])

def makeVector2DFromDistHeading(distance, heading):
   x = distance * math.cos(heading)
   y = distance * math.sin(heading)
   return Vector2D(x, y)

# Utility functions.
def angleBetween(a, b):
    return math.atan2(a.y - b.y, a.x - b.x)
