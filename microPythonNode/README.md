
Three files:

head.py
body.py
tail.py

head.py will contain any import etc
body.py contains the code
tail.py contains startup and parameters



Modify the file app.json:

{
  "tstApp": {
    "mqtt": "192.168.10.10",
    "password": "something"
  }
}

tstApp is the name of the router.
mqtt is the name/address of the MQTT server
password is the router password.

then, to use thje example above:

./build tstApp

It will use the roure defined and output app.py


