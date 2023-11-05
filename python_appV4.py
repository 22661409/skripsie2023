from flask import Flask, render_template
import socket
import time as t
import math

arduino_ip = "192.168.0.143"  # Replace with your Arduino's IP address
arduino_port = 2390

udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

app = Flask(__name__)

message = ""

xVal = 0
yVal = 0
rVal = 0

linSpeed = 50
rotSpeed = 50

Charge = False


@app.route('/')
def index():
    return render_template('index.html')


@app.route('/forward_pressed')
def execute_function_forward_pressed():
    update_vector(0, 1, 0)
    return ""


@app.route('/forward_released')
def execute_function_forward_released():
    update_vector(0, -1, 0)
    return ""


@app.route('/reverse_pressed')
def execute_function_reverse_pressed():
    update_vector(0, -1, 0)
    return ""


@app.route('/reverse_released')
def execute_function_reverse_released():
    update_vector(0, 1, 0)
    return ""


@app.route('/rotate_left_pressed')
def execute_function_left_pressed():
    update_vector(0, 0, -1)
    return ""


@app.route('/rotate_left_released')
def execute_function_left_released():
    update_vector(0, 0, 1)
    return ""


@app.route('/rotate_right_pressed')
def execute_function_right_pressed():
    update_vector(0, 0, 1)
    return ""


@app.route('/rotate_right_released')
def execute_function_right_released():
    update_vector(0, 0, -1)
    return ""


@app.route('/toggle')
def execute_function_toggle():
    global Charge
    Charge = not Charge
    print(Charge)
    return ""


def send_data(data):
    udp_socket.sendto(data.encode(), (arduino_ip, arduino_port))


def receive_data():
    while True:
        data, address = udp_socket.recvfrom(256)
        received_data = data.decode().strip()
        return received_data


def update_vector(x, y, r, linSpeed=50, rotSpeed=50):
    global xVal
    global yVal
    global rVal
    xVal = xVal + x*linSpeed
    yVal = yVal + y*linSpeed
    rVal = rVal + r*rotSpeed
    print(f'\n{xVal}\n{yVal}\n{rVal}\n')

    SendControl(mecanum_drive(xVal, yVal, rVal))


def mecanum_drive(x, y, r):
    # magnitude = math.sqrt(x**2 + y**2)
    # angle = math.degrees(math.atan2(y, x))

    # m1_speed = magnitude * math.cos(math.radians(angle + 45)) + r
    # m2_speed = magnitude * math.cos(math.radians(angle - 45)) - r
    # m3_speed = magnitude * math.cos(math.radians(angle + 135)) + r
    # m4_speed = magnitude * math.cos(math.radians(angle - 135)) - r

    m1_speed = y+x+r
    m2_speed = y-x-r
    m3_speed = y-x+r
    m4_speed = y+x-r

    return m1_speed, m2_speed, m3_speed, m4_speed


def SendControl(m):
    control = ""
    if m[0] > 0:
        control += "0"
    elif m[0] <= 0:
        control += "1"
    if m[1] > 0:
        control += "0"
    elif m[1] <= 0:
        control += "1"
    if m[2] > 0:
        control += "0"
    elif m[2] <= 0:
        control += "1"
    if m[3] > 0:
        control += "0"
    elif m[3] <= 0:
        control += "1"
    if abs(m[0]) <= 255:
        if int(abs(m[0])) < 10:
            control += "00"
        elif int(abs(m[0])) < 100:
            control += "0"
        control += str(int(abs(m[0])))
    else:
        control += "255"
    if abs(m[1]) <= 255:
        if int(abs(m[1])) < 10:
            control += "00"
        elif int(abs(m[1])) < 100:
            control += "0"
        control += str(int(abs(m[1])))
    else:
        control += "255"
    if abs(m[2]) <= 255:
        if int(abs(m[2])) < 10:
            control += "00"
        elif int(abs(m[2])) < 100:
            control += "0"
        control += str(int(abs(m[2])))
    else:
        control += "255"
    if abs(m[3]) <= 255:
        if int(abs(m[3])) < 10:
            control += "00"
        elif int(abs(m[3])) < 100:
            control += "0"
        control += str(int(abs(m[3])))
    else:
        control += "255"
    print("\n")
    print(control)
    print("\n")
    send_data(control)
    print("Sent:", control)
    received_message = receive_data()
    print("Received:", received_message)


if __name__ == '__main__':
    try:
        app.run()
    except:
        SendControl(mecanum_drive(0, 0, 0))
        udp_socket.close()  # Close the socket on Ctrl+C
