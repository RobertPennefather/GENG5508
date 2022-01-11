import sys
import urx
import cv2
import socket
import requests
import colorsys
import numpy as np
from PIL import Image
from time import sleep
from io import BytesIO
from operator import add
from urx.robotiq_two_finger_gripper import Robotiq_Two_Finger_Gripper

sys.setrecursionlimit(50000)

PORT = 30002
HOST = "192.168.1.6"
URL = "http://"+HOST+":4242/current.jpg?annotations=off"

IMG_NAME = "imageOriginal.jpg"
IMG_MOD = "imageBinary.jpg"
IMG_TARGET = "imageTarget.jpg"

COLORS = [ 
	((53, 0, 50), (65, 255,255)),    #GREEN
    ((33, 0, 50), (45, 255, 255)),   #YELLOW
    ((173, 0, 50), (185, 255,255)),  #ORANGE
]

TARGET_COLOR = [100,220,255]
OBJECT_THRESHOLD = 3000

X_MIN = 0.4245
X_MAX = 0.678
Y_MIN = -0.097
Y_MAX = 0.2651

def floodFill(img, col, row):

    cols = len(img)
    rows = len(img[0])

    if not np.all(img[col][row] == 255):
        return [0,0,0]

    img[col][row] = [0,0,0]

    #24 neighbourhood
    col_neighbour = [-2,-1,0,1,2,-2,-1,0,1,2,-2,-1,1,2,-2,-1,0,1,2,-2,-1,0,1,2]
    row_neighbour = [-2,-2,-2,-2,-2,-1,-1,-1,-1,-1,0,0,0,0,1,1,1,1,1,2,2,2,2,2]
    
    pixelsFlooded = [1,col,row]
    for index in range(0,24):
        col_new = col+col_neighbour[index]
        row_new = row+row_neighbour[index]

        if 0 <= row_new and row_new < rows and 0 <= col_new and col_new < cols:
            if np.all(img[col_new][row_new] == 255):
                pixelsFlooded = list( map(add, pixelsFlooded, floodFill(img, col_new, row_new)))

    return pixelsFlooded

def targetCamera():

    print("Finding target")
    response = requests.get(URL)
    img = Image.open(BytesIO(response.content))
    img.save(IMG_NAME)

    breakOut = False
    target = [0,0]
    colorIndex = 0

    for color in COLORS:
        
        # Read img to hsv
        img = cv2.imread(IMG_NAME)
        hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        mask = cv2.inRange(hsv, color[0], color[1])
        cols = len(img)
        rows = len(img[0])

        #Save binary image
        imask = mask>0
        binary = np.zeros_like(img, np.uint8)
        binary[imask] = (255,255,255)
        cv2.imwrite(IMG_MOD, binary)

        #Find most left object
        for indexCol in range(0, cols-1):
            for indexRow in range(0, rows-1):

                objectFound = floodFill(binary, indexCol, indexRow)
                if objectFound[0] > OBJECT_THRESHOLD:
                    target = [objectFound[1]*1.0/objectFound[0]*1.0,objectFound[2]*1.0/objectFound[0]*1.0]
                    breakOut = True
                    break
            if breakOut:
                break
        if breakOut:
            break
        colorIndex += 1

    #Check target
    if target == [0,0]:
        print("No objects found")
        return [target, colorIndex]

    #Target object
    for indexCol in range(0, cols-1):
        img[indexCol,round(target[1])] = TARGET_COLOR
    for indexRow in range(0, rows-1):
        img[round(target[0]), indexRow] = TARGET_COLOR

    #Save the modified pixels
    cv2.imwrite(IMG_TARGET, img)

    return [target, colorIndex]

def learnColor(robot):

    learnPos(robot)
    pictureSuccesful = False

    while not pictureSuccesful:

        cmd = input("Press enter when ready to take picture! ")

        response = requests.get(URL)
        img = Image.open(BytesIO(response.content))
        img.save(IMG_NAME)
            
        # Read img to hsv
        img = cv2.imread(IMG_NAME)
        hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
        cols = len(img)
        rows = len(img[0])
        target = [round(cols/2.0), round(rows/2.0)]

        #Target object
        for indexCol in range(0, cols-1):
            img[indexCol,round(target[1])] = TARGET_COLOR
        for indexRow in range(0, rows-1):
            img[round(target[0]), indexRow] = TARGET_COLOR

        #Save the modified pixels
        cv2.imwrite(IMG_TARGET, img)

        while True:
            cmd = input("Is image target correct? (y/n): ")
            if (cmd == "y"):
                hue = int(hsv[target[0]][target[1]][0])
                hue = min(max(hue, 5), 250)
                colorTuple = ((hue-5, 0, 50), (hue+5, 255,255))
                COLORS.append(colorTuple)
                pictureSuccesful = True
                break
            elif (cmd == "n"):
                break
            else:
                print("ERROR: Invalid response")

    print("Colour succesfully added!")
    print(COLORS)

def openGripper(gripper):
    print("Open Gripper")
    gripper.open_gripper()
    sleep(0.5)

def closeGripper(gripper):
    print("Close Gripper")
    gripper.close_gripper()
    sleep(0.5)

def default(robot):
    print("Going to default")
    robot.movej((0, -1.57, 0, -1.57, 0, 0), acc=0.5, vel=0.8)
    sleep(0.5)

def learnPos(robot):
    print("Going to learn position")
    robot.movej((3.081, -1.462586, 1.47166, -3.83658, -1.57254, 0), acc=0.5, vel=0.8)
    sleep(0.5)

def prelockpos(robot):
    print("Going to prelockpos")
    robot.movep((0.62632, 0.0727, 0.60328, 2.3018, -2.4414, 2.4458), acc=0.5, vel=0.8)
    sleep(0.5)

def lockpos(robot):
    print("Going to lockpos :)")
    robot.movep((0.619, 0.0729, 0.4008, 2.35, -2.50, 0.67), acc=0.5, vel=0.8)
    sleep(0.5)

def toBucket1(robot):
    print("Going to bucket")
    robot.movej((4.24, -0.6, 0.30, -2.66, -1.57, 0), acc=0.5, vel=0.8)
    sleep(0.5)

def toBucket2(robot):
    print("Going to bucket")
    robot.movej((4.54, -0.6, 0.30, -2.66, -1.57, 0), acc=0.5, vel=0.8)
    sleep(0.5)

def toBucket3(robot):
    print("Going to bucket")
    robot.movej((4.80, -0.6, 0.30, -2.66, -1.57, 0), acc=0.5, vel=0.8)
    sleep(0.5)

def middle(robot):
    print("Going to middle")
    robot.movep((0.546, 0.0771, 0.1768, 2.165, -2.302, 0.04), acc=0.5, vel=0.8)
    sleep(0.5)

def corner1(robot):
    print("Corner 1")
    robot.movep((0.678, -0.097, 0.1768, 2.165, -2.302, 0.04), acc=0.5, vel=0.8)
    sleep(0.5)

def corner2(robot):
    print("Corner 2")
    robot.movep((0.4245, 0.2651, 0.1768, 2.165, -2.302, 0.04), acc=0.5, vel=0.8)
    sleep(0.5)

def pickup(robot, gripper):
    print("Pickup Object")
    learnPos(robot)
    openGripper(gripper)

    while True:
        prelockpos(robot)
        lockpos(robot)
        sleep(1)
        objTarget = targetCamera()
        if objTarget[0][0] == 0 and objTarget[0][1] == 0:
            break

        x_val = X_MIN + ((X_MAX-X_MIN)*objTarget[0][0]/480.0)
        y_val = Y_MIN + ((Y_MAX-Y_MIN)*objTarget[0][1]/640.0)
        robot.movep((x_val, y_val, 0.1768, 2.165, -2.302, 0.04), acc=0.5, vel=0.8)
        # print(objTarget)
        # print(x_val)
        # print(y_val)
        sleep(0.5)
        closeGripper(gripper)
        lockpos(robot)
        if objTarget[1] == 0:
            toBucket1(robot)
        elif objTarget[1] == 1:
            toBucket2(robot)
        elif objTarget[1] == 2:
            toBucket3(robot)
        openGripper(gripper)
    print("Finished picking up objects")

def printUserGuide():
    print("THIS IS WHERE HELP WILL GO")

#Initialise connection to robot and gripper
robot = urx.Robot(HOST)
gripper = Robotiq_Two_Finger_Gripper(robot,force=0)
robot.set_tcp((0, 0, 0.1, 0, 0, 0))
robot.set_payload(2, (0, 0, 0.1))
sleep(0.2)  #leave some time to robotot to process the setup commands

while True:

    cmd = input("\nInput Command: ")
    if  (cmd == "close"):
        closeGripper(gripper)
    elif(cmd == "open"):
        openGripper(gripper)
    elif(cmd == "learn"):
        learnColor(robot)
    elif(cmd == "camera"):
        targetCamera()
    elif(cmd == "default"):
        default(robot)
    elif(cmd == "lockpos"):
        lockpos(robot)
    elif(cmd == "bucket1"):
        toBucket1(robot)
    elif(cmd == "bucket2"):
        toBucket2(robot)
    elif(cmd == "bucket3"):
        toBucket3(robot)
    elif(cmd == "c1"):
        corner1(robot)
    elif(cmd == "c2"):
        corner2(robot)
    elif(cmd == "middle"):
        middle(robot)
    elif(cmd == "pickup"):
        pickup(robot, gripper)
    elif(cmd == "help"):
        printUserGuide()
    elif(cmd == "exit"):
        break
    else:
        print("ERROR: Invalid command please check our guide")
        printUserGuide()

robot.close()
print("Connection Closed\n")
sys.exit()

#TODO pick up and drop one function
#TODO user guide