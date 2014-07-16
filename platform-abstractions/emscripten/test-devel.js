/*global  */
/* eslint-env browser */
/* eslint "brace-style": [2, "1tbs"] */
/* eslint "no-console": 0 */

var dali;

var stage = dali.getStage();

var fileInput = document.getElementById("fileInput");
var fileDisplayArea = document.getElementById("fileDisplayArea");
var textInput = document.getElementById("textInput");

var BAKED = true;
var NOT_BAKED = false;

var image;
var anim;

function addImageActor(file) {
  "use strict";

  if (!file) {
    return;
  }
  var reader = new FileReader();

  reader.onload = function(e) {

    var i = new dali.ImageActor();

    image = i;

    var typedBuffer = new Uint8Array(reader.result); // convert ArrayBuffer into a typed array?

    //
    // Setting image isn't property based as browser cannot load from a file
    //
    i.setImage(typedBuffer);
    // i.image         = {"filename":"dali-logo.png"};

    i.visible = true;
    i.anchorPoint = [0.5, 0.5, 0.5];
    i.parentOrigin = [0.5, 0.5, 0.5];
    i.position = [0, 0, -10];
    i.size = [100, 100, 1];
    image = i;

    // i.connectSignal("touched", function(actor) {
    //   eventHandler.onTouched(actor);
    // });

    dali.getStage().add(i);

  };

  reader.readAsArrayBuffer(file);
}

fileInput.addEventListener('change', function(e) {
  "use strict";

  var file = fileInput.files[0];
  console.log(file);

  addImageActor(file);
});

textInput.addEventListener('change', function(e) {
  "use strict";

  var text = textInput.value;
  console.log(text);

  addTextImage(text);

});


function writeChar(buffer, c, x, y) {
  "use strict";

  var ctx = buffer.getContext("2d");

  ctx.font = "Normal 10px Arial";

  ctx.fillText(c, x, y);

}

//
// Class EventHandler
//
// Manages mouse state changes, passes on to dali
// Manages dali's ontouched as a way to select an actor
// Use:
//   From browser window or canvas events call mouseDown/Move/Up
//   From every actor that can be selected set "touched" signal to OnTouched
//   register app logic to the handler events ie register("MouseMove", funcApp)
//
// var EventState = {
//   STOPPED: {
//     value: 0,
//     name: "Stopped"
//   },
//   TRIGGER_START: {
//     value: 1,
//     name: "TriggerStart"
//   },
//   ACTIVE: {
//     value: 2,
//     name: "Active"
//   },
// };

// function EventHandler() {
//   // public
//   this.mouse_x = 0;
//   this.mouse_y = 0;
//   this.mouse_buttons = 0;
//   this.mouseDownPosition = [0, 0, 0];
//   this.drag_dx = 0;
//   this.drag_dy = 0;
//   this.touchedActor = null; // set if actor is touched (left click)
//   this.touchedActorPosition = [0, 0, 0];
//   this.touchedActorSize = [0, 0, 0];
//   //private
//   this.firstTouch = false;
//   // this.selectedActor = null;
//   // this.selectedActorPosition = [0, 0, 0];
//   this.mouseIsDown = 0;
//   this.mouseDragState = EventState.STOPPED;
//   this.metaKey = 0;
//   this.altKey = 0;
//   this.ctrlKey = 0;
//   this.shiftKey = 0;
//   // handlers; use register("MouseDragStart", func); where for func(eventHandler)
//   this.handlersMouseDragStart = []; // valid; touchedActor(can be null), touchedActorPosition
//   this.handlersMouseDragStop = []; // valid; touchedActor(can be null), touchedActorPosition
//   this.handlersMouseDrag = []; // valid; touchedActor(can be null), touchedActorPosition, drag_dx, drag_dy
//   this.handlersMouseMove = []; // valid; touchedActor(can be null), touchedActorPosition, drag_dx, drag_dy
//   this.handlersTouchSelect = []; // valid; touchedActor, touchedActorPosition
//   this.handlersTouchDeSelect = []; // valid; touchedActor, touchedActorPosition
// }

// EventHandler.prototype.register = function(name, handler) {
//   var handlers = this["handlers" + name];
//   if (!(handler in handlers)) {
//     handlers.push(handler);
//   }
// };

// EventHandler.prototype.unRegister = function(name, handler) {
//   var handlers = this["handlers" + name];
//   var index = handlers.index(handler);
//   if (index >= 0) {
//     handlers.splice(index, 1);
//   }
// };

// EventHandler.prototype.onTouched = function(actor) {
//   // on touched seems to run for all moves,ups,downs... in c++ we have access to mode parameter
//   // here we have to track it
//   // console.log("onTouched " + this.firstTouch + " " + this.mouseIsDown + " " + this.mouseDragState.name + " " + this.touchedActor);
//   if (this.firstTouch && this.mouseIsDown && this.mouseDragState === EventState.TRIGGER_START) {
//     this.touchedActor = actor;
//     this.touchedActorPosition = actor.position;
//     this.touchedActorSize = actor.size;
//     this.handleEvent(this.handlersTouchSelect);
//     this.firstTouch = false;
//   }
// };

// EventHandler.prototype.resizerOnTouched = function(actor) {
//   // on touched seems to run for all moves,ups,downs... in c++ we have access to mode parameter
//   // here we have to track it
//   // console.log("onTouched " + this.firstTouch + " " + this.mouseIsDown + " " + this.mouseDragState.name + " " + this.touchedActor);
//   if (this.firstTouch && this.mouseIsDown && this.mouseDragState === EventState.TRIGGER_START) {
//     this.touchedActor = actor;
//     this.touchedActorPosition = actor.position;
//     this.touchedActorSize = actor.size;
//     this.firstTouch = false;
//     this.handleEvent(this.handlersTouchSelect);
//   }
// };

// EventHandler.prototype.mouseDown = function(canvas, e) {
//   this.mouseIsDown = 1;
//   var rect = canvas.getBoundingClientRect(); // in window
//   this.mouse_x = e.clientX - rect.left; // from window
//   this.mouse_y = e.clientY - (rect.bottom - rect.height);
//   this.mouse_buttons = e.button; //  0|1|2 left|middle|rigtht
//   dali.sendMouseEvent(this.mouse_x, this.mouse_y, 0); // 0 == down, 1 == up, 2 == motion
//   // this.mouseDownX = x;
//   // this.mouseDownY = y;
//   this.mouseDownPosition = [this.mouse_x, this.mouse_y, 0];
//   // if (this.mouseDragState === EventState.STOPPED) {
//   this.firstTouch = true;
//   this.mouseDragState = EventState.TRIGGER_START;
//   // }
//   // console.log("mouse down " + this.mouseIsDown + " " + this.mouseDragState.name + " " + this.touchedActor + " " + this.firstTouch);
//   if (this.touchedActor) {
//     var p = this.touchedActor.position;
//     var s = this.touchedActor.size;
//     var w2 = s[0] / 2;
//     var h2 = s[1] / 2;
//     // click outside and we deselect actor
//     var screenToLocal = this.touchedActor.__screenToLocal(this.mouse_x, this.mouse_y);
//     var localX = screenToLocal.get(0);
//     var localY = screenToLocal.get(1);
//     var ok = screenToLocal.get(2) === 1.0;
//     if (ok && (localX < 0 || localX > s[0] || localY < 0 || localY > s[1])) {
//       // if (this.mouse_x < (p[0] - w2) || this.mouse_x > (p[0] + w2) ||
//       //   this.mouse_y < (p[1] - h2) || this.mouse_y > (p[1] + h2)) {
//       // if (this.mouse_buttons === 0) {
//       //   this.handleEvent(this.handlersTouchDeSelect);
//       // }
//       if (this.mouseDragState === EventState.ACTIVE) {
//         this.handleEvent(this.handlersMouseDragStop);
//         this.mouseDragState = EventState.STOPPED;
//         this.touchedActorPosition = this.touchedActor.position;
//         this.touchedActorSize = this.touchedActor.size;
//       }
//       if (this.mouse_buttons === 0) {
//         this.handleEvent(this.handlersTouchDeSelect);
//         this.touchedActor = null;
//       }
//     }
//     // console.log("local: " + localX + "," + localX + "outside? " + (localX < 0 || localX > s[0] || localY < 0 || localY > s[1]).toString() + " selected?" + this.touchedActor);
//   }
// };

// EventHandler.prototype.mouseMove = function(canvas, e) {
//   var rect = canvas.getBoundingClientRect(); // in window
//   this.mouse_x = e.clientX - rect.left; // from window
//   this.mouse_y = e.clientY - (rect.bottom - rect.height);
//   dali.sendMouseEvent(this.mouse_x, this.mouse_y, 2); // 0 == down, 1 == up, 2 == motion
//   this.drag_dx = (this.mouse_x - this.mouseDownPosition[0]);
//   this.drag_dy = (this.mouse_y - this.mouseDownPosition[1]);

//   var hover = dali.hitTest(this.mouse_x, this.mouse_y);
//   if(hover)
//   {
//     console.log(hover.getId());
//   }

//   // do this before setting mouseDragState so we actually drag on 2nd mouseMove where mouseIsDown
//   if (this.mouseIsDown && this.mouseDragState === EventState.ACTIVE) {
//     //console.log("mouse active move " + this.mouseIsDown + " " + this.shiftKey);
//     this.handleEvent(this.handlersMouseDrag);
//   }

//   if (this.mouseIsDown && this.mouseDragState === EventState.TRIGGER_START) {
//     this.handleEvent(this.handlersMouseDragStart);
//     this.mouseDragState = EventState.ACTIVE;
//   }

//   this.mouse_buttons = e.button; //  0|1|2 left|middle|rigtht
//   this.handleEvent(this.handlersMouseMove);

// };

// EventHandler.prototype.mouseUp = function(canvas, e) {
//   this.mouseIsDown = 0;
//   var rect = canvas.getBoundingClientRect(); // in window
//   this.mouse_x = e.clientX - rect.left; // from window
//   this.mouse_y = e.clientY - (rect.bottom - rect.height);
//   this.drag_dx = (this.mouse_x - this.mouseDownPosition[0]);
//   this.drag_dy = (this.mouse_y - this.mouseDownPosition[1]);
//   dali.sendMouseEvent(this.mouse_x, this.mouse_y, 1); // 0 == down, 1 == up, 2 == motion
//   if (this.mouseDragState == EventState.ACTIVE) {
//     this.handleEvent(this.handlersMouseDragStop);
//     // if (this.touchedActor) {
//     //   this.handleEvent(this.handlersTouchDeSelect);
//     //   this.touchedActor = null;
//     // }
//     this.mouseDragState = EventState.STOPPED;
//   }

//   this.mouse_buttons = e.button;
//   // this.handleEvent(this.handlersMouseEvent);
// };

// EventHandler.prototype.mouseWheel = function(canvas, e) {
//   // multiples of +120 for up, -120 for down
//   var clicks = e.wheelDelta / 120;

//   var stage = dali.getStage();
//   var taskList = stage.getRenderTaskList();
//   var task = taskList.getTask(0);
//   var cameraActor = task.getCameraActor();

//   var fov = cameraActor.getFieldOfView();
//   cameraActor.setFieldOfView(fov + (clicks / 180.0 * Math.PI));

//   taskList.delete();
//   task.delete();
//   cameraActor.delete();
//   // console.log(e.wheelDelta);
// }

// EventHandler.prototype.handleEvent = function(handlers) {
//   for (var i = 0, len = handlers.length; i < len; i++) {
//     var handler = handlers[i];
//     handler(this);
//   }
// };

// EventHandler.prototype.keyDown = function(canvas, e) {
//   this.metaKey = e.metaKey;
//   this.altKey = e.altKey;
//   this.ctrlKey = e.ctrlKey;
//   this.shiftKey = e.shiftKey;
// };

// EventHandler.prototype.keyUp = function(canvas, e) {
//   this.metaKey = e.metaKey;
//   this.altKey = e.altKey;
//   this.ctrlKey = e.ctrlKey;
//   this.shiftKey = e.shiftKey;
// };

function EventHandler() {
  // public
  this.mouse_x = 0;
  this.mouse_y = 0;
  this.mouse_buttons = 0;
  this.mouseDownPosition = [0, 0, 0];
  this.drag_dx = 0;
  this.drag_dy = 0;
  this.touchedActor = null; // set if actor is touched (left click)
  this.touchedActorPosition = [0, 0, 0];
  this.touchedActorSize = [0, 0, 0];
  //private
  // this.selectedActor = null;
  // this.selectedActorPosition = [0, 0, 0];
  this.mouseIsDown = 0;
  this.metaKey = 0;
  this.altKey = 0;
  this.ctrlKey = 0;
  this.shiftKey = 0;
  // handlers; use register("MouseDragStart", func); where for func(eventHandler)
  this.handlersMouseDrag = []; // valid; touchedActor(can be null), touchedActorPosition, drag_dx, drag_dy
  this.handlersSelect = []; // valid; touchedActor(can be null), touchedActorPosition, drag_dx, drag_dy
  this.handlersDeselect = []; // valid; touchedActor(can be null), touchedActorPosition, drag_dx, drag_dy
}

EventHandler.prototype.register = function(name, handler) {
  var handlers = this["handlers" + name];
  if (!(handler in handlers)) {
    handlers.push(handler);
  }
};

EventHandler.prototype.unRegister = function(name, handler) {
  var handlers = this["handlers" + name];
  var index = handlers.index(handler);
  if (index >= 0) {
    handlers.splice(index, 1);
  }
};

EventHandler.prototype.updateMouseState = function(canvas, e) {
  var rect = canvas.getBoundingClientRect(); // in window
  this.mouse_x = e.clientX - rect.left; // from window
  this.mouse_y = e.clientY - (rect.bottom - rect.height);
  this.mouse_buttons = e.button; //  0|1|2 left|middle|rigtht
};

EventHandler.prototype.mouseDown = function(canvas, e) {
  this.mouseIsDown = 1;
  this.updateMouseState(canvas, e);
  var hitActor = dali.hitTest(this.mouse_x, this.mouse_y);
  var tid = -1;
  var hid = -2;
  if( this.touchedActor ) { tid = this.touchedActor.getId(); }
  if( hitActor ) { tid = hitActor.getId(); }

  if( hid != tid )
  {
    if( tid > 0 ) {
      this.handleEvent(this.handlersDeselect);
      this.touchedActor = null;
    }
  }

  if( hitActor.getId() == dali.__theStage.rootActor.getId() )
  {
    hitActor = null;
  }

  if( hitActor )
  {
    this.mouseDownPosition = [this.mouse_x, this.mouse_y, 0];
    this.touchedActor = hitActor;
    this.touchedActorPosition = hitActor.position;
    this.touchedActorSize = hitActor.size;

    this.handleEvent(this.handlersSelect);
  }

};

EventHandler.prototype.mouseMove = function(canvas, e) {
  this.updateMouseState(canvas, e);

  if(this.touchedActor && this.mouseIsDown)
  {
    this.drag_dx = (this.mouse_x - this.mouseDownPosition[0]);
    this.drag_dy = (this.mouse_y - this.mouseDownPosition[1]);
    this.handleEvent(this.handlersMouseDrag);
  }
};

EventHandler.prototype.mouseUp = function(canvas, e) {
  this.mouseIsDown = 0;
  this.updateMouseState(canvas, e);
  this.drag_dx = (this.mouse_x - this.mouseDownPosition[0]);
  this.drag_dy = (this.mouse_y - this.mouseDownPosition[1]);
};

EventHandler.prototype.mouseWheel = function(canvas, e) {
  // multiples of +120 for up, -120 for down
  var clicks = e.wheelDelta / 120;

  var stage = dali.getStage();
  var taskList = stage.getRenderTaskList();
  var task = taskList.getTask(0);
  var cameraActor = task.getCameraActor();

  var fov = cameraActor.fieldOfView;
  cameraActor.fieldOfView = fov + (clicks / 180.0 * Math.PI);

  taskList.delete();
  task.delete();
  cameraActor.delete();
  // console.log(e.wheelDelta);
};

EventHandler.prototype.handleEvent = function(handlers) {
  for (var i = 0, len = handlers.length; i < len; i++) {
    var handler = handlers[i];
    handler(this);
  }
};

EventHandler.prototype.keyDown = function(canvas, e) {
  this.metaKey = e.metaKey;
  this.altKey = e.altKey;
  this.ctrlKey = e.ctrlKey;
  this.shiftKey = e.shiftKey;
};

EventHandler.prototype.keyUp = function(canvas, e) {
  this.metaKey = e.metaKey;
  this.altKey = e.altKey;
  this.ctrlKey = e.ctrlKey;
  this.shiftKey = e.shiftKey;
};

//
// Actor list helpers
//
var ActorList = {
  doAll: function(actors, func) {
    for (var i = 0, len = actors.length; i < len; i++) {
      func(actors[i]);
    }
  },
  show: function(actors, visible) {
    this.doAll(actors, function(actor) {
      actor.visible = visible;
    });
  },
  delete: function(actors, visible) {
    this.doAll(actors, function(actor) {
      actor.delete = visible;
    });
  },
  add: function(actors, actor) {
    this.doAll(actors, function(a) {
      actor.add(a);
    });
  },
  remove: function(actors, actor) {
    this.doAll(actors, function(a) {
      actor.remove(a);
    });
  },
  contains: function(actors, actor) {
    if (!actor) {
      return false;
    }
    for (var i = 0, len = actors.length; i < len; i++) {
      if (actors[i].getId() == actor.getId()) {
        return true;
      }
    }
    return false;
  }
};


//
// Depth visualization adornments
//
function DepthAdornment() {
  this.attachedTo = null;
  this.actors = []; // array of 5 square attaced around and to back of actor to show depth

  for (var i = 0; i < 5; i++) { //
    // ActorWrapper CreateSolidColorActor( const Vector4& color, bool border, const Vector4& borderColor, const unsigned int borderSize )
    this.actors.push(dali.createSolidColorActor(
      [0.5, 0.5, 0.5, 1],
      false, [0, 0, 0, 1],
      0));
  }

  ActorList.show(this.actors, false);
}

DepthAdornment.prototype.delete = function() {
  ActorList.delete(this.actors);
  this.actors = [];
};

DepthAdornment.prototype.attach = function(actor) {
  var newActor = true;
  if (this.attachedTo) {
    newActor = (actor.getId() !== this.attachedTo.getId());
  }

  if (actor && newActor) {
    this.unattach(this.attachedTo);

    this.__positionActors(actor);

    ActorList.add(this.actors, actor);

    ActorList.show(this.actors, true);

    this.attachedTo = actor;
  }
};

DepthAdornment.prototype.unattach = function() {
  if (this.attachedTo) {
    ActorList.show(this.actors, false);
    ActorList.remove(this.actors, this.attachedTo);
    this.attachedTo = null;
  }
};

DepthAdornment.prototype.reposition = function() {
  if (this.attachedTo) {
    this.__positionActors(this.attachedTo);
  }
};

DepthAdornment.prototype.__positionActors = function(actor) {
  var s = actor.size;
  s[2] = Math.max(s[2], 20); // pretend the actor is at least 20
  var p = actor.position;
  var halfw = s[0] / 2;
  var halfh = s[1] / 2;
  var halfd = s[2] / 2;

  this.actors[0].position = [-halfw, 0, -halfd];
  this.actors[0].rotation = [0, -90, 0];
  this.actors[0].size = [s[2], s[1], 1];

  this.actors[1].position = [+halfw, 0, -halfd];
  this.actors[1].rotation = [0, 90, 0];
  this.actors[1].size = [s[2], s[1], 1];

  this.actors[2].position = [0, -halfh, -halfd];
  this.actors[2].rotation = [90, 0, 0];
  this.actors[2].size = [s[0], s[2], 1];

  this.actors[3].position = [0, halfh, -halfd];
  this.actors[3].rotation = [-90, 0, 0];
  this.actors[3].size = [s[0], s[2], 1];
  // back
  // this.actors[4].position = [0, 0, -s[2]];
  // this.actors[4].rotation = [0, 0, 0];
  // this.actors[4].size = s;

};



//
// Cursor adornments
//
function Cursors() {
  this.attachedTo = null;
  this.resizers = []; // array of 8 small square resizers
  this.lines = []; // array of 4 bounding box lines
  this.lineColor = [0.5, 0.5, 0.5, 1];
  this.resizerColor = [0, 0, 0, 1];
  this.depthAdornments = new DepthAdornment();

  // this.onTouched = function(actor) {
  //   eventHandler.onTouched(actor);
  //   console.log("resizer touched");
  // };

  // ActorWrapper CreateSolidColorActor( const Vector4& color, bool border, const Vector4& borderColor, const unsigned int borderSize )
  for (var i = 0; i < 4; i++) { // lines
    this.lines.push(dali.createSolidColorActor(this.lineColor, false, [0, 0, 0, 1], 0));
  }

  for (var i = 0; i < 8; i++) { // squares
    this.resizers.push(dali.createSolidColorActor(this.resizerColor, true, [1, 1, 1, 1], 1));
    this.resizers[i].size = [10, 10, 1];
    //    this.resizers[i].__mouseDrag = this.__mouseDragPositionZ;
    // this.resizers[i].connectSignal("touched", this.onTouched);
  }

  ActorList.show(this.lines, false);
  ActorList.show(this.resizers, false);

}


Cursors.prototype.attach = function(actor) {
  if (actor) {

    if (this.attachedTo) {
      this.unattach(this.attachedTo);
    }

    this.__positionActors(actor);

    ActorList.add(this.lines, actor);
    ActorList.add(this.resizers, actor);

    ActorList.show(this.lines, true);
    ActorList.show(this.resizers, true);

    this.depthAdornments.attach(actor);

    this.attachedTo = actor;
  }
};

Cursors.prototype.unattach = function(actor) {
  if (actor && this.attachedTo) {
    ActorList.show(this.lines, false);
    ActorList.show(this.resizers, false);

    ActorList.remove(this.lines, actor);
    ActorList.remove(this.resizers, actor);

    this.depthAdornments.unattach(actor);

    this.attachedTo = null;
  }
};

Cursors.prototype.reposition = function() {
  if (this.attachedTo) {
    this.__positionActors(this.attachedTo);
    this.depthAdornments.reposition();
  }
};

Cursors.prototype.__positionActors = function(actor) {
  var s = actor.size;
  var p = actor.position;
  var halfw = s[0] / 2;
  var halfh = s[1] / 2;

  this.lines[0].position = [0 - halfw, 0, 0];
  this.lines[0].size = [1, halfh * 2, 1];

  this.lines[1].position = [0, 0 - halfh, 0];
  this.lines[1].size = [halfw * 2, 1, 1];

  this.lines[2].position = [0 + halfw, 0, 0];
  this.lines[2].size = [1, halfh * 2, 1];

  this.lines[3].position = [0, 0 + halfh, 0];
  this.lines[3].size = [halfw * 2, 1, 1];

  // drag functions presumes order here is clockwise from top left
  var offsetZ = 5;
  this.resizers[0].position = [-halfw, +halfh, offsetZ];
  this.resizers[1].position = [0, +halfh, offsetZ];
  this.resizers[2].position = [+halfw, +halfh, offsetZ];
  this.resizers[3].position = [+halfw, 0, offsetZ];

  this.resizers[4].position = [+halfw, -halfh, offsetZ];
  this.resizers[5].position = [0, -halfh, offsetZ];
  this.resizers[6].position = [-halfw, -halfh, offsetZ];
  this.resizers[7].position = [-halfw, 0, offsetZ];
};



function App(theEventHandler) {
  var self = this;
  this.rootRotation = [0, 0, 0]; // in degrees
  this.cursors = new Cursors();
  // ActorWrapper CreateSolidColorActor(const Vector4 & color, bool border,
  //   const Vector4 & borderColor,
  //     const unsigned int borderSize)
  this.rootLayerIndicator = dali.createSolidColorActor(
    [0, 0, 0, 0.0],
    true, [0, 0, 0, 1],
    1);
  this.rootLayerIndicator.parentOrigin = [0.5, 0.5, 0.5];
  this.rootLayerIndicator.anchorPoint = [0.5, 0.5, 0.5];
  this.rootLayerIndicator.size = [250, 250, 1];
  this.rootLayerIndicator.position = [0, 0, 0];

  this.selectedActor = null;
  this.selectedActorPosition = [0, 0, 0];
  this.selectedActorSize = [0, 0, 0];

  this.__handleMouseDrag = function(eventHandler) {
    if (self.resizing) {
      if (self.selectedActor) {
        self.selectedActor.size = [
          self.selectedActorSize[0] + eventHandler.drag_dx,
          self.selectedActorSize[1] + eventHandler.drag_dy,
          self.selectedActorSize[2]
        ];
        self.cursors.reposition();
      }
    } else {
      // move the touched actor
      if (self.selectedActor) {
        if (eventHandler.mouse_buttons === 0) {
          self.selectedActor.position = [
            self.selectedActorPosition[0] + eventHandler.drag_dx,
            self.selectedActorPosition[1] + eventHandler.drag_dy,
            self.selectedActorPosition[2]
          ];
        } else if (eventHandler.mouse_buttons === 2) {
          self.selectedActor.position = [
            self.selectedActorPosition[0],
            self.selectedActorPosition[1], -(self.selectedActorPosition[2] + eventHandler.drag_dy)
          ];
        }
      }

    }
    // rotate on middle button
    if (eventHandler.mouse_buttons === 1) {
      self.rootRotation = [eventHandler.mouseDownPosition[1] - eventHandler.mouse_y, -(eventHandler.mouseDownPosition[0] - eventHandler.mouse_x),
        self.rootRotation[2]
      ];
      dali.__theStage.rootActor.rotation = self.rootRotation;
    }

  };

  this.__selectActor = function(actor) {
    // select the actor
    self.selectedActor = eventHandler.touchedActor;
    self.selectedActorPosition = eventHandler.touchedActorPosition;
    self.selectedActorSize = eventHandler.touchedActorSize;
    self.cursors.attach(self.selectedActor);
    var stage = dali.getStage();
    stage.add(self.rootLayerIndicator);
  };

  this.__deselectActor = function() {
    self.cursors.unattach(self.selectedActor);
    var stage = dali.getStage();
    stage.remove(self.rootLayerIndicator);
  };

  this.__handleSelect = function(eventHandler) {
    if( eventHandler.mouse_buttons == 0 ) // left click only selects actor
    {
      self.__selectActor(eventHandler.touchedActor);
    }
  };

  this.__handleDeselect = function(eventHandler) {
    self.__deselectActor();
  };

  // initialize
  eventHandler.register("MouseDrag", this.__handleMouseDrag);
  eventHandler.register("Select", this.__handleSelect);
  eventHandler.register("Deselect", this.__handleDeselect);

  //
  // Monkey patch the stage object (to add to root object not the root layer for rotations)
  //
  dali.__theStage.rootActor = new dali.Actor();
  dali.__theStage.rootActor.parentOrigin = [0.5, 0.5, 0.5];
  dali.__theStage.rootActor.anchorPoint = [0.5, 0.5, 0.5];
  dali.__theStage.add(dali.__theStage.rootActor);
  dali.__theStage.add = function(actor) {
    this.rootActor.add(actor);
  };

  dali.__theStage.remove = function(actor) {
    this.rootActor.remove(actor);
  };

  dali.__theStage.getRootLayer = function() {
    return dali.__theStage.rootActor;
  };

}

function addTextImage(text) {
  // console.log(sFontStyle + " " + fFontSize + "px " + sFontFamily + " char:" + iChar);
  // console.log("GetGlyphImage" + imageData + " size " + imageData.width + "," + imageData.height);

  var testCanvas = document.getElementById("testCanvas");
  var ctx = testCanvas.getContext("2d");
  var pixelHeight = 30;

  ctx.clearRect(0, 0, testCanvas.width, testCanvas.height);

  ctx.font = pixelHeight + "px Verdana";
  // Create gradient
  var gradient = ctx.createLinearGradient(0, 0, testCanvas.width, 0);
  gradient.addColorStop("0", "magenta");
  gradient.addColorStop("0.5", "blue");
  gradient.addColorStop("1.0", "red");
  // Fill with gradient
  ctx.fillStyle = gradient;
  ctx.fillText(text, 0, pixelHeight);

  var m = ctx.measureText(text);

  var a = dali.ImageActor();

  a.setImageData(ctx.getImageData(0, 0, m.width, pixelHeight));

  a.name = text;

  // function onTouched(actor) {
  //   // console.log("touched " + actor.$$.ptr + " " + actor.position);
  //   eventHandler.onTouched(actor);
  // }
  // a.connectSignal("touched", onTouched);

  stage.add(a);

  return a;
}



//
// Global
//

var eventHandler = new EventHandler();
var app = new App(eventHandler);
// var cursors = new CursorsAndEvents();
var canvas = document.getElementById("canvas");

stage.setBackgroundColor([0.3, 0.3, 0.3, 1]);

function mouseDown(e) {
  eventHandler.mouseDown(canvas, e);
}

function mouseUp(e) {
  eventHandler.mouseUp(canvas, e);
}

function mouseMove(e) {
  eventHandler.mouseMove(canvas, e);
}

function mouseWheel(e) {
  eventHandler.mouseWheel(canvas, e);
}

function keyDown(e) {
  eventHandler.keyDown(canvas, e);
}

function keyUp(e) {
  eventHandler.keyUp(canvas, e);
}

canvas.onmousemove = mouseMove;
canvas.onmousedown = mouseDown;
canvas.onmouseup = mouseUp;
canvas.onwheel = mouseWheel;

canvas.onkeydown = keyDown;
canvas.onkeyup = keyUp;

function runTests() {
  var stage = dali.getStage();

  // function onTouched(actor) {
  //   // console.log("touched " + actor.$$.ptr + " " + actor.position);
  //   eventHandler.onTouched(actor);
  // }

  var actor = new dali.TextActor();
  actor.parentOrigin = [0.5, 0.5, 0.5];
  actor.anchorPoint = [0.5, 0.5, 0.5];
  actor.text = "actor";
  actor.name = actor.text;
  actor.size = [100, 100, 1];
  actor.position = [0, 0, 10];
  stage.add(actor);

  var hello = new dali.TextActor();
  // hello.connectSignal("touched", onTouched);
  hello.text = "hello";
  hello.name = hello.text;
  actor.add(hello);

  var hellochild = new dali.TextActor();
  // hellochild.connectSignal("touched", onTouched);
  hellochild.text = "hello-child";
  hellochild.name = hellochild.text;
  hello.add(hellochild);

  var hellochild2 = new dali.TextActor();
  // hellochild2.connectSignal("touched", onTouched);
  hellochild2.text = "hello-child2";
  hellochild2.name = hellochild2.text;
  hello.add(hellochild2);

  var hellochildchild = new dali.TextActor();
  // hellochildchild.connectSignal("touched", onTouched);
  hellochildchild.text = "hello-child-child1";
  hellochildchild.name = "hello-child-child1";
  hellochildchild.name = hellochildchild.text;
  hellochild.add(hellochildchild);


  var depthfirst = actor.findAllChildren();

  assert(actor.getChildCount() === 1);
  assert(actor.getChildAt(0).text === "hello");
  assert(actor.findChildByName("hello-child-child1").text = "hello-child-child1");
  assert(hello.getParent().text === "actor");
  assert(depthfirst[depthfirst.length - 1].text === "hello-child2");

  var directChildren = actor.directChildren();

  assert(directChildren.length === 1);
  assert(directChildren[0].getId() == hello.getId());

  actor.position = [100, 100, 0];

  var root = stage.getRootLayer();

  actor.remove(hello);
  assert(actor.getChildCount() === 0);

  actor.add(hello);
  assert(actor.getChildCount() === 1);

  var rootLayerCount = root.getChildCount();
  stage.remove(actor); // check these don't assert
  assert(root.getChildCount() === rootLayerCount - 1);

  stage.add(actor);
  assert(root.getChildCount() === rootLayerCount);

  assert(root.findChildByName("none") === null);

  // actor.connectSignal("touched", onTouched);

  var inserted = new dali.TextActor();
  inserted.parentOrigin = [0.5, 0.5, 0.5];
  inserted.anchorPoint = [0.5, 0.5, 0.5];
  inserted.text = "inserted";
  inserted.name = inserted.text;
  inserted.size = [100, 100, 1];
  inserted.position = [0, 0, 50];
  actor.insert(1, inserted);
  assert(actor.getChildAt(1).text === "inserted");

  var another = new dali.TextActor();
  another.parentOrigin = [0.5, 0.5, 0.5];
  another.anchorPoint = [0.5, 0.5, 0.5];
  another.text = "peppa";
  another.name = another.text;
  another.size = [100, 100, 1];
  another.position = [-50, 100, 0];
  root.add(another);

  var imageText = addTextImage("Colour");
  // imageText.connectSignal("touched", onTouched);

  var s = dali.getStage();
  var root = s.getRootLayer();
  var c = root.getChildAt(1);
  var n = c.getChildCount();
  var p = c.getParent();
  assert( p.getId() == root.getId() );

  var matrix = c.worldMatrix;

  assert( matrix.length === 16 );

  another.registerProperty("data_output", true);
  assert( another.getPropertyType("data_output") === "BOOLEAN");
  assert( another.data_output === true);
  another.data_output = false;
  assert( another.data_output === false);
  another.data_output = 2.5;
  assert( another.data_output === 2.5);
  assert( another.getPropertyType("data_output") === "FLOAT");

  assert( dali.vectorLength( [1,2,3,4] ) === Math.sqrt( 1*1 + 2*2 + 3*3) );
  assert( dali.vectorLengthSquared(dali.normalize( [0,0,0,1]) ) === 0);

  // for(var f=0; f < 6; f+=1)
  // {
  var f = 2;
  assert( 1 === dali.vectorLengthSquared( dali.normalize( [Math.cos(f)*10.0,
                                                           Math.cos(f+1.0)*10.0,
                                                           Math.cos(f+2.0)*10.0,
                                                           1.0] )) );

  function assertArray(a,b,epsilon)
  {
    assert(a.length === b.length);
    for(var i =0,len=a.length;i < len; ++i)
    {
      assert( a[i] > b[i]-epsilon && a[i] < b[i]+epsilon );
    }
  }

  assertArray( dali.axisAngleToQuaternion( [1.0, 2.0, 3.0, Math.PI/3.0, Math.PI/2.0]),
               [0.189, 0.378, 0.567, 0.707], 0.001);

  assertArray( dali.quaternionToAxisAngle( [1.1, 3.4, 2.7, 0.932] ),
               [3.03, 9.38, 7.45, 0.74],
               0.01 );

  assertArray( dali.vectorCross( [0,1,0], [0,0,1] ),
               [1,0,0], 0.001 );

  assertArray( dali.vectorAdd( [1,2,3], [2,3,4], [1,1,1] ),
               [4, 6, 8], 0.001 );


  console.log( dali.quatByQuat( [0.045, 0.443, 0.432, 0.784],
                                [0.612, 0.344, -0.144, 0.697] ) );

  var mq = dali.vectorAdd( dali.vectorCross( [0.045, 0.443, 0.432], [0.612, 0.344, -0.144] ),
                           dali.vectorByScalar( [0.612, 0.344, -0.144], 0.784 ),
                           dali.vectorByScalar( [0.045, 0.443, 0.432], 0.697 ));
  mq;
  assertArray( dali.quatByQuat([0.045, 0.443, 0.432, 0.784],
                               [0.612, 0.344, -0.144, 0.697] ),
               [mq[0], mq[1], mq[2],
                (0.784*0.697) - dali.vectorDot([0.045, 0.443, 0.432], [0.612, 0.344, -0.144])],
               0.001);

} // runTests

function runAnimation(actor) {
  var a = new dali.Animation();
  a.setDuration(3);
  dali.animateTo(a, actor, "position", [20, 0, 0], "linear", 0, 3);
  a.play();

  function checkNewPos() {
    assert(actor.position = [120, 100, 0]);
  }

  function checkPos() {
    assert(actor.position = [20, 0, 0]);
    actor.position = [100, 100, 0];

    var a = new dali.Animation();
    a.setDuration(3);
    dali.animateBy(a, actor, "position", [20, 0, 0], "linear", 0, 3);
    a.play();

    window.setTimeout(checkNewPos, 4000);
  }

  window.setTimeout(checkPos, 4000);
}

function runRandomAnimation(actor) {
  var a = new dali.Animation();
  a.setDuration(6);
  a.setLooping(true);
  dali.animateTo(a, actor, "position", [300 * Math.random() - 150, 300 * Math.random() - 150, 0],
    "linear", 0, 3);
  dali.animateTo(a, actor, "position", [0, 0, 0], "linear", 3, 3);
  dali.animateTo(a, actor, "rotation", [0, 0, 1, 720 * Math.random() - 360], "linear", 0, 3);
  dali.animateTo(a, actor, "rotation", [0, 0, 1, 0], "linear", 3, 3);

  a.play();
  //    a.delete(); // delete the animtion object but a.Play() keeps it running in Dali
  return anim;
}

function animate3DView() {
  var stage = dali.getStage();
  var root = stage.getRootLayer();
  var rotator = stage.create("Actor");
  var actors = [];
  var a;
  // first child is the camera
  for (var i = 1, len = root.getChildCount(); i < len; i++) {
    a = root.getChildAt(i);
    actors.push(a);
    root.remove(a);
    rotator.add(a);
    a.delete();
  }

  stage.add(rotator);

  var anim = dali.Animation();
  var toRotation = [60, 0, 0];
  anim.setDuration(4);

  dali.animateBy(anim, rotator, "rotation", toRotation, "linear", 0, 2);
  dali.animateBy(anim, rotator, "rotation", actors[i].rotation, "linear", 2, 2);

  dali.animateBy(anim, rotator, "position", [0, 0, -800], "linear", 0, 2);
  dali.animateBy(anim, rotator, "position", [0, 0, 0], "linear", 2, 2);

  anim.play();
  anim.delete();

}

runTests();

//------------------------------------------------------------------------------
// scratch
//------------------------------------------------------------------------------

function clear()
{
  var stage = dali.getStage();
  var root = stage.getRootLayer();
  var children = root.getChildren();

  for(var i =0, len = children.length; i < len; i++)
  {
    root.remove(children[i]);
    children[i].delete(); // delete the wrapper
  }
//  root.delete(); // wrapper
}

function square(color, size)
{
  var a = dali.createSolidColorActor(color, 0, [0,0,0,1], 0);
  a.size = size;
  return a;
}

function threeSquares()
{
  var stage = dali.getStage();
  var root = stage.getRootLayer();

  var a = square([1,0,0,1], [200,200,0]);
  a.name = "red";
  a.position = [-100,0,-20];
  root.add(a);
  a.delete();

  a = square([0,1,0,1], [200,200,0]);
  a.name = "green";
  a.position = [0,-100,-10];
  root.add(a);
  a.delete();

  a = square([0,0,1,1], [200,200,0]);
  a.name = "blue";
  a.position = [0,-100,0];
  root.add(a);
  a.delete();

//  root.delete();
}

function threeSquares2()
{
  var stage = dali.getStage();
  var root = stage.getRootLayer();

  var red = square([1,0,0,1], [200,200,0]);
  red.name = "red";
  red.position = [-100,0,20];
  red.size = [10,10,0];
  root.add(red);

  var green = square([0,1,0,1], [200,200,0]);
  green.name="green";
  green.position = [0,-100,0];
  green.rotation = [0,0,20];
  green.size = [200,200,0];

  var blue = square([0,0,1,1], [200,200,0]);
  blue.name="blue";
  blue.position = [0,0,10];
  blue.parentOrigin = [0,0,0];
  blue.size = [100,100,0];
  green.add(blue);
  root.add(green);

  red.delete(); // wrapper
  green.delete(); // wrapper
  blue.delete(); // wrapper

  //  root.delete();
}

function collectByName(collection)
{
  var stage = dali.getStage();
  var root = stage.getRootLayer();

  var op = function(actor)
  {
    if(actor.name)
    {
      collection[actor.name] = actor;
    }
    return true;
  };

  dali.debugDepthVisit(root, op, true);
}



// threePane();resize(800,800);

function tt()
{
  var r = c.rotation;

  while( r[3] < 350)
  {
    r[3] += 1;
    console.log(r[3]);
    c.rotation = r;
    setTimeout( tt, 250 );
  }
}


function first()
{
  return dali.getStage().getRenderTaskList().getTask(0);
}
function second()
{
  return dali.getStage().getRenderTaskList().getTask(1);
}
function third()
{
  return dali.getStage().getRenderTaskList().getTask(2);
}
function firstCamera()
{
  return dali.getStage().getRenderTaskList().getTask(0).getCameraActor();
}
function secondCamera()
{
  return dali.getStage().getRenderTaskList().getTask(1).getCameraActor();
}
function thirdCamera()
{
  return dali.getStage().getRenderTaskList().getTask(2).getCameraActor();
}

function resize(w,h)
{
  dali.setCanvasSize(w,h);
}


function listProperties(actor)
{
  var props = actor.getProperties();
  for(var i=0, len = props.size(); i< len; i++)
  {
    console.log(props.get(i) + "\t\t" + actor[props.get(i)]);
  }
}

function compareProperties(a1, a2)
{
  var props1 = a1.getProperties();
  var props2 = a2.getProperties();
  props = new Set();
  for(var i=0, len = props1.size(); i< len; i++)
  {
    props.add(props1.get(i));
  }

  for(var i=0, len = props2.size(); i< len; i++)
  {
    var n = props2.get(i);
    if(!props.has(n))
    {
      console.log("A1 missing :" + n);
    }
  }

  var sameProps = [];
  var doit = function(item) {

    globalitem = item;
    var v1 = a1[item] ; // a1.getProperty(item);
    var v2 = a2[item];  // a2.getProperty(item);

    var is_same;

    if( Array.isArray(v1) )
    {
      is_same = (v1.length == v2.length) && v1.every(function(element, index) {
        return element === v2[index]; });
    } else {
      is_same = v1 === v2;
    }

    if(!is_same)
    {
      console.log(item + ": A1= " + v1 + " A2= " + v2);
    }
  };

  props.forEach(doit);
}

function countAllProperties()
{
  var count = 0;
  var tr = dali.getTypeRegistry();
  var names = tr.getTypeNames();
  for(var i=0, len=names.size(); i < len; i++) {
    var ti = tr.getTypeInfo( names.get(i) );
    var props = ti.getProperties();
    count += props.size();
  }
  return count;
}

function testit()
{
  onePane();
  threePane();
  clear();
  threeSquares();

  clear();
  threeSquares2();

  stage = dali.getStage();
  root = dali.getStage().getRootLayer();
  tasks = stage.getRenderTaskList();
  renderTask = tasks.getTask(0);
  camera = renderTask.getCameraActor();

  resize(800,800);

  threePane();

  col = {};
  collectByName(col);

  col.red.position = col.blue.worldPosition;

  worldPos = dali.worldToScreen(col.red.worldPosition);
  worldPos = dali.worldToScreen(col.red.worldPosition, renderTask);

  topLeftCorner = dali.worldToScreen( dali.matrixByVector(col.red.worldMatrix, [-col.red.size[0], -col.red.size[1], 0]) );
  // do function to get four corners.....

}



var atest = function (angleA, angleB)
{
  q1 = dali.axisAngleToQuaternion([0,1,0, radian(angleA)]) ; // yaw around to look at scene down -ve z
  qIdentity = [0,0,0,1];

  q2 = dali.axisAngleToQuaternion([1,0,0, radian(angleB)]); // pitch down to look at scene

  dali.quatByQuat(qIdentity,qIdentity);

  q = dali.quaternionToAxisAngle( quatByQuat(q1, q2) );

  c.position = [0,-2000,0]; // camera.position; // [ camera.position[2], 0, 0 ];
  c.rotation = [q[0], q[1], q[2], degree(q[3])];

}(180, -90);

c.inheritRotation = true;

var donttest = function()
{
  threePane();

  resize(800,800);
};
