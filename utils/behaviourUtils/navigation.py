#/usr/bin/env python

#Import Modules
import os, pygame
import math
from pygame.locals import *

FIELD_WIDTH = 4010.0
FIELD_LENGTH = 6000.0
GOAL_BOX_WIDTH = 2210.0
GOAL_BOX_LENGTH = 600.0

OFFNAO_FIELD_WIDTH_OFFSET = 675.0
OFFNAO_FIELD_LENGTH_OFFSET = 675.0

OFFNAO_FULL_FIELD_WIDTH = (FIELD_WIDTH + (OFFNAO_FIELD_WIDTH_OFFSET * 2))
OFFNAO_FULL_FIELD_LENGTH = (FIELD_LENGTH + (OFFNAO_FIELD_LENGTH_OFFSET * 2))

ATT_SCALE = 4
WEAK_ATT_SCALE = 2
REP_SCALE = 1000
DIST_THRESH = 1400
DEST_THRESH = 1
EPSILON = 1
ROBOT_AVOID = 180

class Vector:
    def __init__(self, x, y):
        self.x = x;
        self.y = y;

    def length(self):
        return math.sqrt(self.lengthSquared())

    def lengthSquared(self):
        return self.x*self.x + self.y*self.y

    def distance(self, var):
        return self.minus(var).length()

    def normalise(self):
        l = self.length()
        return Vector(self.x/l, self.y/l)

    def minus(self, var):
        return Vector(self.x - var.x, self.y - var.y)

    def plus(self, var):
        return Vector(self.x + var.x, self.y + var.y)

    def multiply(self, s):
        return Vector(self.x*s, self.y*s)
    
    def crop(self, max):
        if self.length() > max :
            return self.normalise().multiply(max)
        return self
    
    def dot(self, var):
        return self.x * var.x + self.y * var.y
    
    def toString(self):
        return ('%.2f' % self.x, '%.2f' % self.y)


def convertFieldToScreenCoords(coord):
    x = (-coord.y + OFFNAO_FULL_FIELD_WIDTH / 2.0) / 10.0
    y = (-coord.x + OFFNAO_FULL_FIELD_LENGTH / 2.0) / 10.0
    return Vector(x, y)

def convertScreenToFieldCoords(coord):
    x = -(coord.y * 10.0 - OFFNAO_FULL_FIELD_LENGTH / 2.0)
    y = -(coord.x * 10.0 - OFFNAO_FULL_FIELD_WIDTH / 2.0)
    return Vector(x, y)


#classes for our game objects
class Cursor(pygame.sprite.Sprite):
    def __init__(self):
        pygame.sprite.Sprite.__init__(self) #call Sprite initializer
        self.image = pygame.image.load('data/ball.png')
        self.rect = self.image.get_rect()

    def update(self):
        self.rect.midtop = pygame.mouse.get_pos()


class Obstacle(pygame.sprite.Sprite):
    def __init__(self, texture):
        pygame.sprite.Sprite.__init__(self) #call Sprite intializer
        self.image = pygame.image.load(texture)
        self.rect = self.image.get_rect()
        self.rect.midtop = pygame.mouse.get_pos()
        self.pos = convertScreenToFieldCoords(Vector(self.rect.midtop[0], self.rect.midtop[1]))
        
class Destination(pygame.sprite.Sprite):
    def __init__(self, ball):
        pygame.sprite.Sprite.__init__(self) #call Sprite intializer
        self.image = pygame.image.load("data/goal.png")
        self.rect = self.image.get_rect()
        self.pos = Vector(ball.pos.x - 500, ball.pos.y - 500)
        screenCoords = convertFieldToScreenCoords(self.pos);
        self.rect.midtop = (screenCoords.x, screenCoords.y)

class Wingman(pygame.sprite.Sprite):
    def __init__(self, obstacles, destPos):
        pygame.sprite.Sprite.__init__(self) #call Sprite intializer
        self.image = pygame.image.load('data/winger.png')
        self.rect = self.image.get_rect()
        self.rect = self.image.get_rect()
        self.rect.midtop = pygame.mouse.get_pos()
        self.pos = convertScreenToFieldCoords(Vector(self.rect.midtop[0], self.rect.midtop[1]))
        self.obstacles = obstacles
        self.destPos = destPos
        
    def update(self):
        "attractive field offset behind ball"
        dist = self.pos.minus(self.destPos).length()
        if dist <= DEST_THRESH :
            # Uatt = ATT_SCALE * (pos - destPos)
            Uatt = self.pos.minus(self.destPos).multiply(ATT_SCALE);
        else :
            # Uatt = ATT_SCALE * DEST_THRESH * (pos - destPos) / dist
            Uatt = self.pos.minus(self.destPos).multiply(ATT_SCALE * DEST_THRESH / dist)
        
        "weaker attractive field behind striker"
        offset = Vector(300, -300)
        strikerPos = self.obstacles[1].pos
        offsetStrikerPos = strikerPos.minus(offset)
        dist = self.pos.minus(offsetStrikerPos).length()
        if dist <= DEST_THRESH :
            Uatt2 = self.pos.minus(offsetStrikerPos).multiply(WEAK_ATT_SCALE);
        else :
            Uatt2 = self.pos.minus(offsetStrikerPos).multiply(WEAK_ATT_SCALE * DEST_THRESH / dist)
        Uatt = Uatt.plus(Uatt2)
        
        "repulsive field of each obstacle"
        Urep = Vector(0,0)
        for i in range(2, len(self.obstacles)) :
            Urep = Urep.plus(self.repulsiveField(self.obstacles[i].pos))

        "repulsive field of ball to goal line"
        goalPos = Vector(FIELD_LENGTH/2, 0)
        ballPos = self.obstacles[0].pos
        Urep = Urep.plus(self.repulsiveField(self.closestPointToSegment(goalPos, ballPos)))

        "repulsive field of striker to ball line"
        Urep = Urep.plus(self.repulsiveField(self.closestPointToSegment(ballPos, strikerPos)));

        print (Uatt.toString(), Urep.toString())
        
        U = Uatt.plus(Urep)
        self.pos = self.pos.minus(U.multiply(8))
        screenCoords = convertFieldToScreenCoords(self.pos);
        self.rect.midtop = (screenCoords.x, screenCoords.y)

    def repulsiveField(self, obsPos):
        Urep = Vector(0, 0)
        dist = self.pos.minus(obsPos).length()
        dist = max(EPSILON, dist - ROBOT_AVOID)
        # find new obsPos with modified dist
        direction = obsPos.minus(self.pos).normalise()
        obsPos = self.pos.plus(direction.multiply(dist))
        if dist <= DIST_THRESH :
            #REP_SCALE * (1/DIST_THRESH - 1/dist)*(pos - obsPos)/dist^2
            Urep = self.pos.minus(obsPos).multiply(REP_SCALE * (1/DIST_THRESH - 1/dist)/dist)
        return Urep
    
    def closestPointToSegment(self, v, w):
        #minimum distance between point to line segment v-w
        #http://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
        l2 = w.minus(v).lengthSquared()
        if l2 < EPSILON :   # v =~ w
            return v
        t = self.pos.minus(v).dot(w.minus(v)) / l2
        if t < 0:     # Beyond the 'v' end of the segment 
            return v
        elif t > 1:   # Beyond the 'w' end of the segment
            return w  
        return v.plus(w.minus(v).multiply(t))  # Projection falls on the segment

    
def main():
#Initialize Everything
    pygame.init()
    screen = pygame.display.set_mode((536, 735))
    pygame.display.set_caption('Navigation')
    pygame.mouse.set_visible(0)

#Create The Backgound
    background = pygame.Surface(screen.get_size())
    background = background.convert()
    background.fill((250, 250, 250))

    image = pygame.image.load('data/spl_field.png')


#Display The Background
    screen.blit(image, (0, 0))

#Prepare Game Objects
    clock = pygame.time.Clock()
    

    allSprites = pygame.sprite.RenderPlain()
    allSprites.add(Cursor())
    addedObjects = []
    wingman = None;

#Main Loop
    while 1:
        clock.tick(30)

    #Handle Input Events
        for event in pygame.event.get():
            if event.type == QUIT:
                return
            elif event.type == KEYDOWN:
                if wingman is None :
                    if event.key == K_SPACE :
                        if len(addedObjects) == 0 :
                            obstacle = Obstacle("data/ball.png")
                        elif len(addedObjects) == 1 :
                            obstacle = Obstacle("data/supporter.png")
                        else :
                            obstacle = Obstacle("data/defender.png")
                        addedObjects.append(obstacle)
                        allSprites.add(obstacle)
                    elif event.key == K_RETURN and len(addedObjects) > 1:
                        dest = Destination(addedObjects[0])
                        wingman = Wingman(addedObjects, dest.pos)
                        allSprites.add(wingman)
                        allSprites.add(dest)
                    
        allSprites.update()

    #Draw Everything
        screen.blit(image, (0, 0))
        #screen.blit(background, (0, 0))
        allSprites.draw(screen)
        pygame.display.flip()

#Game Over


#this calls the 'main' function when this script is executed
if __name__ == '__main__': main()
