#/usr/bin/env python
"""
This simple example is used for the line-by-line tutorial
that comes with pygame. It is based on a 'popular' web banner.
Note there are comments here, but for the full explanation, 
follow along in the tutorial.
"""


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

DEFENDER_INDEX = 0
SUPPORTER_INDEX = 1
WINGER_INDEX = 2

robotTextures = [
    'data/defender.png',
    'data/supporter.png',
    'data/winger.png'
]

class Vector:
    def __init__(self, x, y):
        self.x = x;
        self.y = y;

    def length(self):
        return math.sqrt(self.x*self.x + self.y*self.y)

    def normalise(self):
        l = self.length()
        self.x /= l
        self.y /= l

    def minus(self, var):
        return Vector(self.x - var.x, self.y - var.y)

    def plus(self, var):
        return Vector(self.x + var.x, self.y + var.y)

    def multiply(self, s):
        self.x *= s;
        self.y *= s


def convertFieldToScreenCoords(coord):
    x = (-coord.y + OFFNAO_FULL_FIELD_WIDTH / 2.0) / 10.0
    y = (-coord.x + OFFNAO_FULL_FIELD_LENGTH / 2.0) / 10.0
    return Vector(x, y)

def convertScreenToFieldCoords(coord):
    x = -(coord.y * 10.0 - OFFNAO_FULL_FIELD_LENGTH / 2.0)
    y = -(coord.x * 10.0 - OFFNAO_FULL_FIELD_WIDTH / 2.0)
    return Vector(x, y)


class WingerRolePositioning:
    def getPosition(self, ballPosition):
        result = self.getStartPosition()
        result.y = -ballPosition.y / 2.0
        return result

    def getStartPosition(self):
        return Vector(1.0/3.0 * FIELD_LENGTH, 0.0)


class SupporterRolePositioning:
    def getPosition(self, ballPosition):
        return self.getStartPosition()

    def getStartPosition(self):
        return Vector(0.0, 0.0)


class DefenderRolePositioning:
    def getPosition(self, ballPosition):
        result = self.getStartPosition()
        result.y = ballPosition.y
        return result

    def getStartPosition(self):
        return Vector(-1.0/3.0 * FIELD_LENGTH, 0.0)


#classes for our game objects
class Ball(pygame.sprite.Sprite):
    """moves a ball on the screen, following the mouse"""
    def __init__(self):
        pygame.sprite.Sprite.__init__(self) #call Sprite initializer
        self.image = pygame.image.load('data/ball.png')
        self.rect = self.image.get_rect()
        self.position = Vector(0.0, 0.0);

    def getWorldPos(self):
        return self.position

    def update(self):
        "move the ball based on the mouse position"
        cursorPos = pygame.mouse.get_pos()
        self.position = convertScreenToFieldCoords(Vector(cursorPos[0], cursorPos[1]))
        self.rect.midtop = (cursorPos[0], cursorPos[1] - self.rect.height/2)


class Robot(pygame.sprite.Sprite):
    def __init__(self, positioningStrategy, ball, texturePath):
        pygame.sprite.Sprite.__init__(self) #call Sprite intializer
        self.image = pygame.image.load(texturePath)
        self.rect = self.image.get_rect()

        self.positioningStrategy = positioningStrategy
        self.position = positioningStrategy.getStartPosition()
        self.ball = ball;

        self.updatePosition(self.position);

    def update(self):
        ballPos = self.ball.getWorldPos()
        self.updatePosition(self.positioningStrategy.getPosition(ballPos))

    def updatePosition(self, newPosition):
        screenCoords = convertFieldToScreenCoords(newPosition);

        self.rect.midtop = (screenCoords.x, screenCoords.y - self.rect.height / 2)


def main():
#Initialize Everything
    pygame.init()
    screen = pygame.display.set_mode((536, 735))
    pygame.display.set_caption('Role Positioning')
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
    
    ball = Ball()
    robots = [
        Robot(DefenderRolePositioning(), ball, robotTextures[DEFENDER_INDEX]),
        Robot(SupporterRolePositioning(), ball, robotTextures[SUPPORTER_INDEX]),
        Robot(WingerRolePositioning(), ball, robotTextures[WINGER_INDEX])
    ]

    allSprites = pygame.sprite.RenderPlain()
    allSprites.add(ball)
    for robot in robots:
        allSprites.add(robot)

#Main Loop
    while 1:
        clock.tick(60)

    #Handle Input Events
        for event in pygame.event.get():
            if event.type == QUIT:
                return
            elif event.type == KEYDOWN and event.key == K_ESCAPE:
                return
        
        allSprites.update()

    #Draw Everything
        screen.blit(image, (0, 0))
        #screen.blit(background, (0, 0))
        allSprites.draw(screen)
        pygame.display.flip()

#Game Over


#this calls the 'main' function when this script is executed
if __name__ == '__main__': main()

