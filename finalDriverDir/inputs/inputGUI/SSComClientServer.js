const fs = require('fs');
const {io} = require("socket.io-client");

let socket = io("https://samuelshuster.com/webcontroller");

socket.on("connection", (socket) => {
    console.log("SOCKET.IO: CONNECTED")
    socket.emit("serverDeclaration", (response) => {
        if (response.status === "ok") {
            console.log("SOCKET.IO: DECLARED SERVER")
        }
    })
})

setTimeout(() => {
    socket.emit("serverDeclaration", (response) => {
        if (response.status === "ok") {
            console.log("SOCKET.IO: DECLARED SERVER")
        }
    })
}, 1000)

// Socket.IO Server + Server Module

// const express = require("express");
// const {createServer} = require("http");
// const {Server} = require("socket.io");
// const fs = require('fs');
//
// const app = express();
// const httpServer = createServer(app);
// const io = new Server(httpServer, { /* options */});

// let joystick = [0, 0]
// let touchpad = [0, 0]
// let positon = [0, 0, 0]
// let rotation = [0, 0, 0, 0]
// let trigger = [0, false]
// let buttons = {"a": false, "b": false, "x": false, "y": false, "system": false};
let HMDData = {"positon": [0, 0, 0], "rotation": [0, 0, 0, 1]}
let controllerData = {
    r: {
        joystick: [0, 0, false],
        touchpad: [0, 0, false],
        trigger: [0, false],
        buttons: {"a": false, "b": false, "x": false, "y": false, "system": false},
        position: [0, 0, 0],
        rotation: [0, 0, 0, 1],
        grip: [0, 0]
    },
    l: {
        joystick: [0, 0, false],
        touchpad: [0, 0, false],
        trigger: [0, false],
        buttons: {"a": false, "b": false, "x": false, "y": false, "system": false},
        position: [0, 0, 0],
        rotation: [0, 0, 0, 1],
        grip: [0, 0]
    }
}

// io.on("connection", (socket) => {
    socket.on("joystick", (lr, x, y) => {
        controllerData[lr].joystick = [x, y]
        updateFile(lr);
    });

    socket.on("touchpad", (lr, x, y) => {
        controllerData[lr].touchpad = [x, y]
        updateFile(lr);
    });

    socket.on("position", (hlr, x, y, z) => {
        if (hlr === "h") {
            HMDData.position = [x, y, z]
        } else
            controllerData[hlr].position = [x, y, z]
        updateFile(hlr);
    });

    socket.on("rotation", (hlr, x, y, z, w) => {
        if (hlr === "h") {
            HMDData.rotation = [x, y, z, w]
        } else
            controllerData[hlr].rotation = [x, y, z, w]
        updateFile(hlr);
    })

    socket.on("trigger", (lr, value, pressed) => {
        controllerData[lr].trigger = [value, pressed]
        updateFile(lr);
    });

    socket.on("button", (lr, button, pressed) => {
        controllerData[lr].buttons[button] = pressed
        // console.log(button, pressed, buttons)
        updateFile(lr);
    });

    socket.on("grip", (lr, value, force) => {
        controllerData[lr].grip = [value, force]
        updateFile(lr);
    });
// });

function falseOneOrZero(value) {
    // console.log(value)
    if (value === true) {
        return 1
    } else {
        return 0
    }
}

function updateFile(hlr) {
    // // Open input.txt for writing and overwrite the first line only with the new data
    // let newLine = joystick[0] + " " + joystick[1] + " " +
    //           touchpad[0] + " " + touchpad[1] + " " +
    //           trigger[0] + " " + falseOneOrZero(trigger[1]) + " " +
    //           positon[0] + " " + positon[1] + " " + positon[2] + " " +
    //           rotation[0] + " " + rotation[1] + " " + rotation[2] + " " + rotation[3] + " " +
    //           falseOneOrZero(buttons.a) + " " + falseOneOrZero(buttons.b) + " " + falseOneOrZero(buttons.x) + " " + falseOneOrZero(buttons.y) + " " + falseOneOrZero(buttons.system);
    // console.log(newLine)
    // newLine += "\njoy tou trg pos   rot     a b x y s\n" +
    //             "x y x y v b x y z w x y z b b b b b"
    // fs.writeFile('../inputs.txt', newLine, function (err) {
    //     if (err) throw err;
    // });
    // Write the new data to the appropriate file
    if (hlr === "h") {
        let newLine = HMDData.position[0] + " " + HMDData.position[1] + " " + HMDData.position[2] + " " +
            HMDData.rotation[0] + " " + HMDData.rotation[1] + " " + HMDData.rotation[2] + " " + HMDData.rotation[3]
        console.log(newLine)
        fs.writeFile('../HMDInput.txt', newLine, function (err) {
            if (err) throw err;
        });
    } else {
        //touchpadX touchpadY touchpadClick joystickX joystickY joystickClick posX posY posZ rotX rotY rotZ rotW triggerVal triggerPressed gripVal gripForce a b x y system
        let newline = controllerData[hlr].touchpad[0] + " " + controllerData[hlr].touchpad[1] + " " + falseOneOrZero(controllerData[hlr].touchpad[2]) + " " +
            controllerData[hlr].joystick[0] + " " + controllerData[hlr].joystick[1] + " " + falseOneOrZero(controllerData[hlr].joystick[2]) + " " +
            controllerData[hlr].position[0] + " " + controllerData[hlr].position[1] + " " + controllerData[hlr].position[2] + " " +
            controllerData[hlr].rotation[0] + " " + controllerData[hlr].rotation[1] + " " + controllerData[hlr].rotation[2] + " " + controllerData[hlr].rotation[3] + " " +
            controllerData[hlr].trigger[0] + " " + falseOneOrZero(controllerData[hlr].trigger[1]) + " " +
            controllerData[hlr].grip[0] + " " + controllerData[hlr].grip[1] + " " +
            falseOneOrZero(controllerData[hlr].buttons.a) + " " + falseOneOrZero(controllerData[hlr].buttons.b) + " " + falseOneOrZero(controllerData[hlr].buttons.x) + " " + falseOneOrZero(controllerData[hlr].buttons.y) + " " + falseOneOrZero(controllerData[hlr].buttons.system);
        newline = newline.replaceAll("null", "0");
        console.log(newline)
        newline += "\ntouch joys  pos   rot     tri gri a b x y s\n" +
            "x y c x y c x y z x y z w v c v f\n" +
            "touchpadX touchpadY touchpadClick joystickX joystickY joystickClick posX posY posZ rotX rotY rotZ rotW triggerVal triggerPressed gripVal gripForce a b x y system\n"
        let filename = "../" + (hlr === "l" ? "left" : "right") + "ControllerInput" + ".txt";
        fs.writeFile(filename, newline, function (err) {
            if (err) throw err;
        });
    }
}

// app.get("*", (req, res) => {
//     res.sendFile(__dirname + "/client.html");
// })

// httpServer.listen(3000);

