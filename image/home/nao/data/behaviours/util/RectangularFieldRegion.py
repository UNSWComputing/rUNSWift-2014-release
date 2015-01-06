import Vector2D


class RectangularFieldRegion(object):
   
   def __init__(self, minExtent, maxExtent):
      self.minExtent = minExtent
      self.maxExtent = maxExtent

      if minExtent.x > maxExtent.x or minExtent.y > maxExtent.y:
         print "Invalid extents given:", minExtent.x, minExtent.y, maxExtent.x, maxExtent.y


   def pointInRegion(self, point, regionInflateAmount):
      testMin = Vector2D.makeVector2DCopy(self.minExtent)
      testMin.x -= regionInflateAmount
      testMin.y -= regionInflateAmount

      testMax = Vector2D.makeVector2DCopy(self.maxExtent)
      testMax.x += regionInflateAmount
      testMax.y += regionInflateAmount

      return point.x >= testMin.x and point.x <= testMax.x and \
             point.y >= testMin.y and point.y <= testMax.y

