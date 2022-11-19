const fs = require('fs');
const {io} = require("socket.io-client");



let joystick = [0, 0]
let touchpad = [0, 0]
let positon = [0, 0, 0]
let rotation = [0, 0, 0, 0]
let trigger = [0, false]
let buttons = {"a": false, "b": false, "x": false, "y": false, "system": false};
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

socket.on("joystick", (x, y) => {
    joystick = [x, y];
    updateFile();
});

socket.on("touchpad", (x, y) => {
    touchpad = [x, y];
    updateFile();
});

socket.on("position", (x, y, z) => {
    positon = [x, y, z];
    updateFile();
});

socket.on("rotation", (x, y, z, w) => {
    rotation = [x, y, z, w];
    updateFile();
});

socket.on("trigger", (value, pressed) => {
    trigger = [value, pressed];
    updateFile();
});

socket.on("button", (button, pressed) => {
    buttons[button] = pressed;
    updateFile();
});

function falseOneOrZero(value) {
    if (value === true) {
        return 1
    } else {
        return 0
    }
}

function updateFile() {
    // Open input.txt for writing and overwrite the first line only with the new data
    let newLine = joystick[0] + " " + joystick[1] + " " +
        touchpad[0] + " " + touchpad[1] + " " +
        trigger[0] + " " + falseOneOrZero(trigger[1]) + " " +
        positon[0] + " " + positon[1] + " " + positon[2] + " " +
        rotation[0] + " " + rotation[1] + " " + rotation[2] + " " + rotation[3] + " " +
        falseOneOrZero(buttons.a) + " " + falseOneOrZero(buttons.b) + " " + falseOneOrZero(buttons.x) + " " + falseOneOrZero(buttons.y) + " " + falseOneOrZero(buttons.system);
    console.log(newLine)
    newLine += "\njoy tou trg pos   rot     a b x y s\n" +
        "x y x y v b x y z w x y z b b b b b"
    fs.writeFile('../inputs.txt', newLine, function (err) {
        if (err) throw err;
    });
}


