/*global  */
/* eslint-env browser */
/* eslint "brace-style": [2, "1tbs"] */
/* eslint "no-console": 0 */
/* eslint "no-underscore-dangle": 0 */

var dali;
var ace;
var canvas;
var $;

var fileInput = document.getElementById("fileInput");
var fileInputEmscripten = document.getElementById("fileInputEmscripten");
var fileInputRGB = document.getElementById("fileInputRGB");
var btnTextActorAdd = document.getElementById("btnTextActorAdd");
var textInput = document.getElementById("textInput");
var textVertex = document.getElementById("textVertex");
var textFragment = document.getElementById("textFragment");
var textShaderErrors = document.getElementById("textShaderErrors");
var shaderButton = document.getElementById("shaderButton");
var shaderCheckButton = document.getElementById("shaderCheckButton");

//
//
// Shaders
//
//
var shaderSourceSelection = 0;
var shaderSources = [
  {
    name: "pass through",
    vertex: "void main(void)\n" +
      "{\n" +
      "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n",
    fragment: "precision mediump float;\n" +
      "void main()\n" +
      "{\n" +
      "  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n" +
      "}\n"
  },
  {
    name: "bouncing effect",
    vertex: "",
    fragment: "precision mediump float;\n" +
      "uniform float uProgressRate;\n" +
      "uniform vec4 uAssignedColor;\n" +
      "void main()\n" +
      "{\n" +
      "  float progressRate = abs(uProgressRate)*0.5;\n" +
      "  float amplitude = 0.15 - progressRate*0.15 ;\n" +
      "  float x1 = 7.5 * (vTexCoord.x - progressRate);\n" +
      "  float x2 = 7.5 * (vTexCoord.x - 1.0 + progressRate);\n" +
      "  float height1 = max(0.00001, 0.3 - amplitude * ( exp(x1) + exp(-x1) ) );\n" +
      "  float height2 = max(0.00001, 0.3 - amplitude * ( exp(x2) + exp(-x2) ) );\n" +
      "  float height3 = max(0.00001, 1.0 - 3.0 * amplitude * ( exp(x1*0.5) + exp(-x1*0.5) ) );\n" +
      "  float height4 = max(0.00001, 1.0 - 3.0 * amplitude * ( exp(x2*0.5) + exp(-x2*0.5) ) );\n" +
      "  vec4 fragColor = vec4(0.0);\n" +
      "  float y = vTexCoord.y/(height1+height2);\n" +
      "  float y2 = vTexCoord.y/max(height3,height4);\n" +
      "  float coef = max(height1,height2)*5.0/( 1.0+exp(y*12.0-6.0) );\n" +
      "  float alpha = pow( max(0.0,(1.0-y2))*(1.0-min(abs(x1),abs(x2))/5.0), 2.0);\n" +
      "  if( vTexCoord.y < 0.075 )\n" +
      "  {\n" +
      "    fragColor= mix(uAssignedColor, vec4(1.0), coef);\n" +
      "    fragColor += (vec4(1.0)-fragColor) * alpha;\n" +
      "  }\n" +
      "  else if (y2<1.0)\n" +
      "  {\n" +
      "    fragColor =vec4(1.0,1.0,1.0, alpha + (1.0-alpha)*coef);\n" +
      "    fragColor.rgb -= ( vec3(1.0)-uAssignedColor.rgb )*min(clamp(y*1.2-0.3, 0.0, 0.3),clamp(0.9-y*1.2,0.0,0.3));\n" +
      "  }\n" +
      "  fragColor.a *= 10.0*min(min(vTexCoord.x, 1.0-vTexCoord.x),0.1)*min(1.0, progressRate/0.2);\n" +
      "  gl_FragColor =  fragColor;\n" +
      "}"
  },
  {
    name: "emboss combine",
    vertex: "void main(void)\n" +
      "{\n" +
      "gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
      "vTexCoord = aTexCoord;\n" +
      "}\n",
    fragment: "precision mediump float;\n" +
      "const int KERNEL_SIZE = 9;\n" +
      "\n" +
      "uniform vec4 weight;\n" +
      "\n" +
      "// Gaussian kernel\n" +
      "float kernel[KERNEL_SIZE];\n" +
      "\n" +
      "float width  = 512.0;\n" +
      "float height = 512.0;\n" +
      "\n" +
      "float step_w = 1.0/width;\n" +
      "float step_h = 1.0/height;\n" +
      "\n" +
      "vec2 offset[KERNEL_SIZE];\n" +
      "\n" +
      "void main(void)\n" +
      "{\n" +
      "  precision mediump float;\n" +
      "\n" +
      "  vec4 sum = vec4(0.0);\n" +
      "\n" +
      "  offset[0] = vec2(-step_w, -step_h);\n" +
      "  offset[1] = vec2(0.0, -step_h);\n" +
      "  offset[2] = vec2(step_w, -step_h);\n" +
      "\n" +
      "  offset[3] = vec2(-step_w, 0.0);\n" +
      "  offset[4] = vec2(0.0, 0.0);\n" +
      "  offset[5] = vec2(step_w, 0.0);\n" +
      "\n" +
      "  offset[6] = vec2(-step_w, step_h);\n" +
      "  offset[7] = vec2(0.0, step_h);\n" +
      "  offset[8] = vec2(step_w, step_h);\n" +
      "\n" +
      "  // guassian blur\n" +
      "  kernel[0] = 1.0/16.0;  kernel[1] = 2.0/16.0;  kernel[2] = 1.0/16.0;\n" +
      "  kernel[3] = 2.0/16.0;  kernel[4] = 4.0/16.0;  kernel[5] = 2.0/16.0;\n" +
      "  kernel[6] = 1.0/16.0;  kernel[7] = 2.0/16.0;  kernel[8] = 1.0/16.0;\n" +
      "\n" +
      "  // laplace\n" +
      "  // kernel[0] = 0.0;  kernel[1] = 1.0;  kernel[2] = 0.0;\n" +
      "  // kernel[3] = 1.0;  kernel[4] = -4.0; kernel[5] = 1.0;\n" +
      "  // kernel[6] = 0.0;  kernel[7] = 1.0;  kernel[8] = 0.0;\n" +
      "\n" +
      "  // Embossing\n" +
      "  // 2  0  0\n" +
      "  // 0 -1  0\n" +
      "  // 0  0 -1\n" +
      "  kernel[0] = 2.0;  kernel[1] = 0.0;  kernel[2] = 0.0;\n" +
      "  kernel[3] = 0.0;  kernel[4] = -1.0; kernel[5] = 0.0;\n" +
      "  kernel[6] = 0.0;  kernel[7] = 0.0;  kernel[8] =-1.0;\n" +
      "\n" +
      "    vec4 tmp = texture2D(sTexture, vTexCoord.st + offset[0]);\n" +
      "    sum += tmp * kernel[0];\n" +
      "\n" +
      "    tmp = texture2D(sTexture, vTexCoord.st + offset[1]);\n" +
      "    sum += tmp * kernel[1];\n" +
      "\n" +
      "    tmp = texture2D(sTexture, vTexCoord.st + offset[2]);\n" +
      "    sum += tmp * kernel[2];\n" +
      "\n" +
      "    tmp = texture2D(sTexture, vTexCoord.st + offset[3]);\n" +
      "    sum += tmp * kernel[3];\n" +
      "\n" +
      "    tmp = texture2D(sTexture, vTexCoord.st + offset[4]);\n" +
      "    sum += tmp * kernel[4];\n" +
      "\n" +
      "    tmp = texture2D(sTexture, vTexCoord.st + offset[5]);\n" +
      "    sum += tmp * kernel[5];\n" +
      "\n" +
      "    tmp = texture2D(sTexture, vTexCoord.st + offset[6]);\n" +
      "    sum += tmp * kernel[6];\n" +
      "\n" +
      "    tmp = texture2D(sTexture, vTexCoord.st + offset[7]);\n" +
      "    sum += tmp * kernel[7];\n" +
      "\n" +
      "    tmp = texture2D(sTexture, vTexCoord.st + offset[8]);\n" +
      "    sum += tmp * kernel[8];\n" +
      "\n" +
      "    sum = texture2D(sTexture, vTexCoord.xy) + (sum * weight);\n" +
      "\n" +
      "  gl_FragColor = sum;\n" +
      "}\n"
    // "  dali.setShader(actor, vertex, fragment);\n" +
    // "\n" +
    // "}\n";
  },
  {
    name: "",
    vertex: "",
    fragment: ""
  },
  {
    name: "",
    vertex: "",
    fragment: ""
  },
  {
    name: "",
    vertex: "",
    fragment: ""
  },
  {
    name: "",
    vertex: "",
    fragment: ""
  },
  {
    name: "",
    vertex: "",
    fragment: ""
  },
  {
    name: "",
    vertex: "",
    fragment: ""
  },
  {
    name: "",
    vertex: "",
    fragment: ""
  },
  {
    name: "",
    vertex: "",
    fragment: ""
  }
];


//
//
// Helper functions
//
//
function createImageFromTypeBuffer(filename, typedBuffer) {
  "use strict";
  var i = new dali.ImageActor();
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
  i.name = filename.replace(/-/g, "_").replace(/\./g, "_");

  return i;
}

function createImageRGBFromTypeBuffer(filename, typedBuffer) {
  "use strict";
  var i = new dali.ImageActor();
  //
  // Setting image isn't property based as browser cannot load from a file
  //
  i.setImageDataRGB(typedBuffer);
  // i.image         = {"filename":"dali-logo.png"};

  i.visible = true;
  i.anchorPoint = [0.5, 0.5, 0.5];
  i.parentOrigin = [0.5, 0.5, 0.5];
  i.position = [0, 0, -10];
  i.size = [100, 100, 1];
  i.name = filename.replace(/-/g, "_").replace(/\./g, "_");

  return i;
}

function addImageActor(filename, typedBuffer) {
  "use strict";
  var i = createImageFromTypeBuffer(filename, typedBuffer);
  app.addActor(i);
}

function addImageActorRGB(filename, typedBuffer) {
  "use strict";
  var i = createImageRGBFromTypeBuffer(filename, typedBuffer);
  app.addActor(i);
}

function asyncFileReaderGetTypedBuffer(file, callback) {
  "use strict";
  if (!file) {
    return;
  }
  var reader = new FileReader();
  reader._theFilename = file.name;
  reader.onload = function(// e
  ) {
    var typedBuffer = new Uint8Array(reader.result); // convert ArrayBuffer into a typed array?

    callback(file.name, typedBuffer);

  };

  reader.readAsArrayBuffer(file);
}


// function fileReaderGetImage(filename, callBackImageTypedBuffer) {
//         function fileReaderGetImageaddImageActor(file) {

// function addImageActor(file) {
//     "use strict";

//     if (!file) {
//         return;
//     }
//     var reader = new FileReader();
//     reader._the_filename = file.name;
//     reader.onload = function(e) {

//         var i = new dali.ImageActor();

//         image = i;

//         var typedBuffer = new Uint8Array(reader.result); // convert ArrayBuffer into a typed array?

//         //
//         // Setting image isn't property based as browser cannot load from a file
//         //
//         i.setImage(typedBuffer);
//         // i.image         = {"filename":"dali-logo.png"};

//         i.visible = true;
//         i.anchorPoint = [0.5, 0.5, 0.5];
//         i.parentOrigin = [0.5, 0.5, 0.5];
//         i.position = [0, 0, -10];
//         i.size = [100, 100, 1];
//         i.name = this._the_filename.replace(/-/g, "_").replace(/\./g, "_");

//         image = i;

//         // i.connectSignal("touched", function(actor) {
//         //   eventHandler.onTouched(actor);
//         // });

//         if (eventHandler.touchedActor) {
//             eventHandler.touchedActor.add(i);
//         } else {
//             dali.stage.add(i);
//         }
//     };

//     reader.readAsArrayBuffer(file);
// }


// var lineObjOffsetTop = 2;

// function positionLineObj(obj, ta) {
//   "use strict";
//   obj.style.top = (ta.scrollTop * -1 + lineObjOffsetTop) + "px";
// }

// function createTextAreaWithLines(id, noHeaderLines) {
//   "use strict";
//   var el = document.createElement("DIV");
//   var ta = document.getElementById(id);
//   ta.parentNode.insertBefore(el, ta);
//   el.appendChild(ta);

//   el.className = "textAreaWithLines";
//   el.style.width = (ta.offsetWidth + 30) + "px";
//   ta.style.position = "absolute";
//   ta.style.left = "30px";
//   el.style.height = (ta.offsetHeight + 2) + "px";
//   el.style.overflow = "hidden";
//   el.style.position = "relative";
//   el.style.width = (ta.offsetWidth + 30) + "px";
//   var lineObj = document.createElement("DIV");
//   lineObj.style.position = "absolute";
//   lineObj.style.top = lineObjOffsetTop + "px";
//   lineObj.style.left = "0px";
//   lineObj.style.width = "27px";
//   el.insertBefore(lineObj, ta);
//   lineObj.style.textAlign = "right";
//   lineObj.className = "lineObj";
//   var string = "";
//   for (var no = noHeaderLines; no < 200; no++) {
//     if (string.length > 0) {
//       string = string + "<br>";
//     }
//     string = string + no;
//   }

//   ta.onkeydown = function() {
//     positionLineObj(lineObj, ta);
//   };
//   ta.onmousedown = function() {
//     positionLineObj(lineObj, ta);
//   };
//   ta.onscroll = function() {
//     positionLineObj(lineObj, ta);
//   };
//   ta.onblur = function() {
//     positionLineObj(lineObj, ta);
//   };
//   ta.onfocus = function() {
//     positionLineObj(lineObj, ta);
//   };
//   ta.onmouseover = function() {
//     positionLineObj(lineObj, ta);
//   };
//   lineObj.innerHTML = string;

// }


function onActorSelected(eventHandler) {
  "use strict";
  var actor = eventHandler.touchedActor;

  // remove childred
  var elem = document.getElementById("selected");
  var count = elem.children.length;
  for (var i = 0; i < count; i++) {
    elem.removeChild(elem.children[0]);
  }

  if (actor) {

    var p = document.createElement("div");
    p.innerHTML = "<h3><b>" + actor.name + "</b> [<b>" + actor.getId() + "</b>] (<b>" + actor.position + "</b>)</h3>";
    elem.appendChild(p);

    var parent = actor.getParent();
    if(parent) {
      p = document.createElement("div");
      p.innerHTML =
        "<p>" +
        "(Parent: <b>" + parent.name + "</b> [<b>" + parent.getId() + "</b>] (<b>" + parent.position + "</b>)" +
        " Anchored: <b>" + actor.anchorPoint + "</b>" +
        " ParentOrigin: <b>" + actor.parentOrigin + "</b> )" +
        "</p>";
      elem.appendChild(p);
    }

    var props = actor.getProperties(); // std::vector<string>

    for (var i = 0; i < props.size(); i++) {
      var name = props.get(i);

      var gridBlock = document.createElement("div");
      gridBlock.className = "row";

      var nameBlock = document.createElement("div");
      nameBlock.className = "col-md-6";
      nameBlock.innerHTML = name;
      gridBlock.appendChild(nameBlock);

      var valueBlock = document.createElement("div");
      valueBlock.className = "col-md-6";
      valueBlock.innerHTML = actor[name];
      gridBlock.appendChild(valueBlock);

      elem.appendChild(gridBlock);
    }

    props.delete();
    //console.log(eventHandler);
  }
}

function SelectActor(id) {
  "use strict";
  var root = dali.stage.rootActor;
  var actor = root.findChildById(id);
  if (actor) {
    app.selectActor(actor);
  }
}

function rebuildTree() {
  "use strict";
  // remove childred
  var elem = document.getElementById("tree");
  var count = elem.children.length;
  for (var i = 0; i < count; i++) {
    elem.removeChild(elem.children[0]);
  }

  // <ul class="nav nav-list">
  //      <li><label class="tree-toggler nav-header">Header 1</label>

  var stage = dali.stage;
  var root = stage.rootActor;

  var recurse = function(parentElement, actor) {
    var children = actor.getChildren();
    if (children) {
      var ul = null;

      for (var i = 0; i < children.length; ++i) {
        var child = children[i];

        if (child.name !== "_dali_adornment") {

          var li = document.createElement("li");

          var a = document.createElement("a");
          a.href = "javascript:SelectActor(" + child.getId() + ");";
          a.text = "(" + child.getId() + ") '" + child.name + "'";
          li.appendChild(a);

          if (ul === null) {
            ul = document.createElement("ul");
            ul.className = "nav-tree";
          }

          ul.appendChild(li);

          recurse(li, child);

          // finish with the child wrapper
          child.delete();
        }
      }

      if (ul) {
        parentElement.appendChild(ul);
      }
    }
  };

  recurse(elem, root);

}


function EventHandler() {
  // public
  this.mouse_x = 0;
  this.mouse_y = 0;
  this.mouse_buttons = 0;
  this.mouseDownPosition = [0, 0, 0];
  this.drag_dx = 0;
  this.drag_dy = 0;
  this.touchedActor = null; // set if actor is touched (left click)
  //private
  // this.selectedActor = null;
  // this.selectedActorPosition = [0, 0, 0];
  this.mouseIsDown = 0;
  this.metaKey = 0;
  this.altKey = 0;
  this.ctrlKey = 0;
  this.shiftKey = 0;
  // handlers; use register("MouseDragStart", func); where for func(eventHandler)
  this.handlersMouseDrag = []; // valid; touchedActor(can be null), drag_dx, drag_dy
  this.handlersSelect = []; // valid; touchedActor(can be null), drag_dx, drag_dy
  this.handlersDeselect = []; // valid; touchedActor(can be null), drag_dx, drag_dy
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
  "use strict";
  this.mouseIsDown = 1;
  this.updateMouseState(canvas, e);
  var hitActor = dali.hitTest(this.mouse_x, this.mouse_y);
  var tid = -1;
  var hid = -2;
  if (this.touchedActor) {
    tid = this.touchedActor.getId();
  }
  if (hitActor) {
    tid = hitActor.getId();
  }

  if (hid !== tid) {
    if (tid > 0) {
      this.handleEvent(this.handlersDeselect);
      this.touchedActor = null;
    }
  }

  if (hitActor) {

    if (hitActor.getId() === dali.stage.rootActor.getId()) {
      hitActor = null;
    }

    this.mouseDownPosition = [this.mouse_x, this.mouse_y, 0];
    this.touchedActor = hitActor;
    this.handleEvent(this.handlersSelect);
  }

};

EventHandler.prototype.mouseMove = function(canvas, e) {
  "use strict";
  this.updateMouseState(canvas, e);

  if (this.touchedActor && this.mouseIsDown) {
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

  var taskList = dali.stage.getRenderTaskList();
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
  "use strict";
  this.attachedTo = null;
  this.actors = []; // array of 5 square attaced around and to back of actor to show depth

  for (var i = 0; i < 5; i++) { //
    // ActorWrapper CreateSolidColorActor( const Vector4& color, bool border, const Vector4& borderColor, const unsigned int borderSize )
    var a = dali.createSolidColorActor([0.5, 0.5, 0.5, 1],
                                       false, [0, 0, 0, 1],
                                       0);
    a.name = "_dali_adornment";
    this.actors.push(a);
  }

  ActorList.show(this.actors, false);
}

DepthAdornment.prototype.delete = function() {
  "use strict";
  ActorList.delete(this.actors);
  this.actors = [];
};

DepthAdornment.prototype.attach = function(actor) {
  "use strict";
  var newActor = true;
  if (this.attachedTo) {
    newActor = (actor.getId() !== this.attachedTo.getId());
  }

  if (actor && newActor) {
    this.unattach(this.attachedTo);

    if(!ActorList.contains(this.actors, actor)) {
      this.__positionActors(actor);

      ActorList.add(this.actors, actor);

      ActorList.show(this.actors, true);

      this.attachedTo = actor;
    }
  }
};

DepthAdornment.prototype.unattach = function() {
  "use strict";
  if (this.attachedTo) {
    ActorList.show(this.actors, false);
    ActorList.remove(this.actors, this.attachedTo);
    this.attachedTo = null;
  }
};

DepthAdornment.prototype.reposition = function() {
  "use strict";
  if (this.attachedTo) {
    this.__positionActors(this.attachedTo);
  }
};

DepthAdornment.prototype.__positionActors = function(actor) {
  "use strict";
  var s = actor.size;
  s[2] = Math.max(s[2], 20); // pretend the actor is at least 20
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
    var a = dali.createSolidColorActor(this.lineColor, false, [0, 0, 0, 1], 0);
    a.name = "_dali_adornment";
    this.lines.push(a);
  }

  for (var i = 0; i < 8; i++) { // squares
    var a = dali.createSolidColorActor(this.resizerColor, true, [1, 1, 1, 1], 1);
    a.name = "_dali_adornment";
    this.resizers.push(a);
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
  this.rootLayerIndicator.name = "_dali_adornment";

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
      dali.stage.rootActor.rotation = self.rootRotation;
    }

  };

  this.addActor = function(actor) {
    if (eventHandler.touchedActor) {
      eventHandler.touchedActor.add(actor);
    } else {
      dali.stage.add(actor);
    }
  };

  this.selectActor = function(actor) {
    eventHandler.touchedActor = actor;
    self.__deselectActor();
    self.__selectActor(actor);
  };

  this.__selectActor = function(actor) {
    // select the actor
    self.selectedActor = actor;
    self.cursors.attach(self.selectedActor);
    dali.stage.add(self.rootLayerIndicator);
  };

  this.__deselectActor = function() {
    self.cursors.unattach(self.selectedActor);
    dali.stage.remove(self.rootLayerIndicator);
  };

  this.__handleSelect = function(eventHandler) {
    if (eventHandler.mouse_buttons === 0) { // left click only selects actor
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
  dali.stage.rootActor = new dali.Actor();
  dali.stage.rootActor.parentOrigin = [0.5, 0.5, 0.5];
  dali.stage.rootActor.anchorPoint = [0.5, 0.5, 0.5];
  dali.stage.rootActor.name = "app rotation actor";
  dali.stage.add(dali.stage.rootActor);

  dali.stage.add = function(actor) {
    dali.stage.rootActor.add(actor);
  };

  dali.stage.remove = function(actor) {
    dali.stage.rootActor.remove(actor);
  };

}

function addTextImageFromCanvas(text) {
  "use strict";
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

  var a = new dali.ImageActor();

  a.setImageData(ctx.getImageData(0, 0, m.width, pixelHeight));

  a.name = text;

  // function onTouched(actor) {
  //   // console.log("touched " + actor.$$.ptr + " " + actor.position);
  //   eventHandler.onTouched(actor);
  // }
  // a.connectSignal("touched", onTouched);

  app.addActor(a);

  return a;
}



//
// Global
//

var eventHandler = new EventHandler();
var app = new App(eventHandler);
// var cursors = new CursorsAndEvents();
var canvas = document.getElementById("canvas");

dali.stage.setBackgroundColor([0.3, 0.3, 0.3, 1]);

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

//
//
//

function runRandomAnimation(actor) {
  var a = new dali.Animation();
  a.setDuration(6);
  a.setLooping(true);
  a.animateTo(actor, "position", [300 * Math.random() - 150, 300 * Math.random() - 150, 0],
              "linear", 0, 3);
  a.animateTo(actor, "position", [0, 0, 0], "linear", 3, 3);
  a.animateTo(actor, "rotation", [0, 0, 1, 720 * Math.random() - 360], "linear", 0, 3);
  a.animateTo(actor, "rotation", [0, 0, 1, 0], "linear", 3, 3);

  a.play();
  //    a.delete(); // delete the animtion object but a.Play() keeps it running in Dali
  return anim;
}

function animate3DView() {
  var root = dali.stage.rootActor;
  var rotator = dali.stage.create("Actor");
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

  dali.stage.add(rotator);

  var anim = dali.Animation();
  var toRotation = [60, 0, 0];
  anim.setDuration(4);

  anim.animateBy(rotator, "rotation", toRotation, "linear", 0, 2);
  anim.animateBy(rotator, "rotation", actors[i].rotation, "linear", 2, 2);

  anim.animateBy(rotator, "position", [0, 0, -800], "linear", 0, 2);
  anim.animateBy(rotator, "position", [0, 0, 0], "linear", 2, 2);

  anim.play();
  anim.delete();

}

//------------------------------------------------------------------------------
// test helper functions
//------------------------------------------------------------------------------

function clear() {
  var root = dali.stage.rootActor;
  var children = root.getChildren();

  for (var i = 0, len = children.length; i < len; i++) {
    root.remove(children[i]);
    children[i].delete(); // delete the wrapper
  }
  //  root.delete(); // wrapper
}

function square(color, size) {
  var a = dali.createSolidColorActor(color, 0, [0, 0, 0, 1], 0);
  a.size = size;
  return a;
}

function threeSquares() {
  var root = dali.stage.rootActor;

  var a = square([1, 0, 0, 1], [200, 200, 0]);
  a.name = "red";
  a.position = [-100, 0, -20];
  root.add(a);
  a.delete();

  a = square([0, 1, 0, 1], [200, 200, 0]);
  a.name = "green";
  a.position = [0, -100, -10];
  root.add(a);
  a.delete();

  a = square([0, 0, 1, 1], [200, 200, 0]);
  a.name = "blue";
  a.position = [0, -100, 0];
  root.add(a);
  a.delete();

  //  root.delete();
}

function threeSquares2() {
  var root = dali.stage.rootActor;

  var red = square([1, 0, 0, 1], [200, 200, 0]);
  red.name = "red";
  red.position = [-100, 0, 20];
  red.size = [10, 10, 0];
  root.add(red);

  var green = square([0, 1, 0, 1], [200, 200, 0]);
  green.name = "green";
  green.position = [0, -100, 0];
  green.rotation = [0, 0, 20];
  green.size = [200, 200, 0];

  var blue = square([0, 0, 1, 1], [200, 200, 0]);
  blue.name = "blue";
  blue.position = [0, 0, 10];
  blue.parentOrigin = [0, 0, 0];
  blue.size = [100, 100, 0];
  green.add(blue);
  root.add(green);

  red.delete(); // wrapper
  green.delete(); // wrapper
  blue.delete(); // wrapper

  //  root.delete();
}

function collectByName(collection) {
  var root = dali.stage.rootActor;
  if (collection === undefined) {
    collection = {};
  }
  var op = function(actor) {
    if (actor.name) {
      collection[actor.name] = actor;
    }
    return true;
  };

  dali.debugDepthVisit(root, op, true);

  return collection;
}

function first() {
  return dali.stage.getRenderTaskList().getTask(0);
}

function second() {
  return dali.stage.getRenderTaskList().getTask(1);
}

function third() {
  return dali.stage.getRenderTaskList().getTask(2);
}

function firstCamera() {
  return dali.stage.getRenderTaskList().getTask(0).getCameraActor();
}

function secondCamera() {
  return dali.stage.getRenderTaskList().getTask(1).getCameraActor();
}

function thirdCamera() {
  return dali.stage.getRenderTaskList().getTask(2).getCameraActor();
}

function resize(w, h) {
  dali.setCanvasSize(w, h);
}


function listProperties(actor) {
  var props = actor.getProperties();
  for (var i = 0, len = props.size(); i < len; i++) {
    console.log(props.get(i) + "\t\t" + actor[props.get(i)]);
  }
}

function listChildren(actor) {
  for (var i = 0, len = actor.getChildCount(); i < len; i++) {
    var a = actor.getChildAt(i);
    var ti = a.getTypeInfo();
    console.log("Actor Type:" + ti.getName());
    ti.delete(); // wrapper
    a.delete(); // wrapper
  }
}

function compareProperties(a1, a2) {
  var props1 = a1.getProperties();
  var props2 = a2.getProperties();
  var props = new Set();
  for (var i = 0, len = props1.size(); i < len; i++) {
    props.add(props1.get(i));
  }

  for (i = 0, len = props2.size(); i < len; i++) {
    var n = props2.get(i);
    if (!props.has(n)) {
      console.log("A1 missing :" + n);
    }
  }

  var doit = function(item) {
    var v1 = a1[item]; // a1.getProperty(item);
    var v2 = a2[item]; // a2.getProperty(item);

    var isSame;

    if (Array.isArray(v1)) {
      isSame = (v1.length === v2.length) && v1.every(function(element, index) {
        return element === v2[index];
      });
    } else {
      isSame = v1 === v2;
    }

    if (!isSame) {
      console.log(item + ": A1= " + v1 + " A2= " + v2);
    }
  };

  props.forEach(doit);
}

var EPSILON = 0.005;

function compareArrays(a, b) {
  "use strict";
  if (Array.isArray(a) && Array.isArray(b)) {
    if (a.length === b.length) {
      for (var i = 0, len = a.length; i < len; i++) {
        if (Array.isArray(a[i])) {
          if (Array.isArray(b[i])) {
            if (!compareArrays(a[i], b[i])) {
              return false;
            }
          } else {
            return false;
          }
        } else {
          if (typeof (a[i]) === "number") {
            if (typeof (b[i]) !== "number") {
              return false;
            } else {
              if (Math.abs(a[i]) > Math.abs(b[i]) + EPSILON ||
                  Math.abs(a[i]) < Math.abs(b[i]) - EPSILON) {
                return false;
              }
            }
          } else {
            if (a[i] !== b[i]) {
              return false;
            }
          }
        }
      }
      return true;
    }
  }
  return false;
}

function countAllProperties() {
  var count = 0;
  var tr = dali.getTypeRegistry();
  var names = tr.getTypeNames();
  for (var i = 0, len = names.size(); i < len; i++) {
    var ti = tr.getTypeInfo(names.get(i));
    var props = ti.getProperties();
    count += props.size();
  }
  return count;
}

// var checkShader = function(actor) {
//     var gl = canvas.getContext('webgl');
//     var vertex = gl.createShader(gl.VERTEX_SHADER);
//     var ret = [null, null];

//     // dali uniforms
//     var vertexPrepend = //"precision highp float;\n" +
//         "attribute vec3  aPosition;\n" +
//         "attribute vec2  aTexCoord;\n" +
//         "uniform   mat4  uMvpMatrix;\n" +
//         "uniform   mat4  uModelMatrix;\n" +
//         "uniform   mat4  uViewMatrix;\n" +
//         "uniform   mat4  uModelView;\n" +
//         "uniform   mat3  uNormalMatrix;\n" +
//         "uniform   mat4  uProjection;\n" +
//         "uniform   vec4  uColor;\n" +
//         "varying   vec2  vTexCoord;\n";

//     var fragmentPrepend = //"precision mediump float;\n" +
//         "uniform   sampler2D sTexture;\n" +
//         "uniform   sampler2D sEffect;\n" +
//         "uniform   mediump vec4      uColor;\n" +
//         "varying   mediump vec2      vTexCoord;\n";


//     gl.shaderSource(vertex, vertexPrepend + textVertex.value);
//     gl.compileShader(vertex);

//     if (!gl.getShaderParameter(vertex, gl.COMPILE_STATUS)) {
//         console.log("Vertex Error:");
//         console.log(gl.getShaderInfoLog(vertex));
//     } else {
//         ret[0] = vertex;
//     }

//     var fragment = gl.createShader(gl.FRAGMENT_SHADER);
//     gl.shaderSource(fragment, fragmentPrepend + textFragment.value);
//     gl.compileShader(fragment);

//     if (!gl.getShaderParameter(fragment, gl.COMPILE_STATUS)) {
//         console.log("Fragment Error:");
//         console.log(gl.getShaderInfoLog(fragment));
//     } else {
//         ret[1] = fragment;
//     }

//     if (ret[0] === null && ret[1] == null) {
//         ret = null;
//     }

//     return ret;
// };


function native2ascii(str) {
  "use strict";
  var out = "";
  for (var i = 0; i < str.length; i++) {
    if (str.charCodeAt(i) < 0x80) {
      out += str.charAt(i);
    // } else {
    //   // pass
    //   //var u = "" + str.charCodeAt(i).toString(16);
    //   //out += "\\u" + (u.length === 2 ? "00" + u : u.length === 3 ? "0" + u : u);
    }
  }
  return out;
}

var getShadersFromUi = function() {
  "use strict";
  var e = ace.edit("editorVertex");
  var ret = {};
  ret.vertex = native2ascii(e.getSession().getValue());
  e = ace.edit("editorFragment");
  ret.fragment = native2ascii(e.getSession().getValue());
  return ret;
};

var actorIdToShaderSet = {};

var selectShader = function(index) {
  "use strict";
  var e;

  var vertex = "";
  var fragment = "";

  shaderSourceSelection = -1;

  if(index < 0) {
    if(eventHandler.touchedActor) {
      var id = eventHandler.touchedActor.getId();
      if(id in actorIdToShaderSet) {
        var options = actorIdToShaderSet[id];

        if("vertex" in options && options.vertex) {
          vertex = options.vertex;
        }

        if("fragment" in options && options.fragment) {
          fragment = options.fragment;
        }

      }
    }
  } else {
    var shader = shaderSources[index];

    vertex = shader.vertex;

    fragment = shader.fragment;

    shaderSourceSelection = index;
  }


  e = ace.edit("editorVertex");
  e.getSession().setValue(vertex);

  e = ace.edit("editorFragment");
  e.getSession().setValue(fragment);

};


var checkShader = function() {
  "use strict";
  var gl = canvas.getContext("webgl");
  var vertex = gl.createShader(gl.VERTEX_SHADER);
  var ret = true;

  // dali uniforms
  var vertexPrepend = "//precision highp float;\n" +
        "attribute vec3  aPosition;\n" +
        "attribute vec2  aTexCoord;\n" +
        "uniform   mat4  uMvpMatrix;\n" +
        "uniform   mat4  uModelMatrix;\n" +
        "uniform   mat4  uViewMatrix;\n" +
        "uniform   mat4  uModelView;\n" +
        "uniform   mat3  uNormalMatrix;\n" +
        "uniform   mat4  uProjection;\n" +
        "uniform   vec4  uColor;\n" +
        "varying   vec2  vTexCoord;\n";

  var fragmentPrepend = "//precision mediump float;\n" +
        "uniform   sampler2D sTexture;\n" +
        "uniform   sampler2D sEffect;\n" +
        "uniform   mediump vec4 uColor;\n" +
        "varying   mediump vec2 vTexCoord;\n";


  var uiShaders = getShadersFromUi();

  gl.shaderSource(vertex, vertexPrepend + uiShaders.vertex);
  gl.compileShader(vertex);

  var errors = "";

  if (!gl.getShaderParameter(vertex, gl.COMPILE_STATUS)) {
    errors = "VERTEX:\n" + gl.getShaderInfoLog(vertex);
    ret = false;
  }

  var fragment = gl.createShader(gl.FRAGMENT_SHADER);
  gl.shaderSource(fragment, fragmentPrepend + uiShaders.fragment);
  gl.compileShader(fragment);

  if (!gl.getShaderParameter(fragment, gl.COMPILE_STATUS)) {
    errors += "FRAGMENT:\n" + gl.getShaderInfoLog(fragment);
    ret = false;
  }

  textShaderErrors.value = errors;

  if(ret) {
    if(shaderSourceSelection >= 0) {
      uiShaders = getShadersFromUi();
      shaderSources[shaderSourceSelection].vertex = uiShaders.vertex;
      shaderSources[shaderSourceSelection].fragment = uiShaders.fragment;
    }
  }

  return ret;
};


var extractUniformsFromShaderSource = function(source) {
  "use strict";
  // var reg = new RegExp("\uniform ((bool|int|uint|float|[biu]?vec[234]|mat[234]x?[234]?) ([A-Za-z0-9]*));","gi");
  var reg = /uniform ((bool|int|uint|float|[biu]?vec[234]|mat[234]x?[234]?) ([A-Za-z0-9]*));/gi;
  var tmp;
  var returnvalue = [];
  while (tmp = reg.exec(source)) {
    returnvalue.push([tmp[2], tmp[3]]);
  };
  return returnvalue;
};


var setShader = function(actor) {
  "use strict";
  if (checkShader()) { // otherwise dali stops working

    if( actor.getId() in actorIdToShaderSet) {
      if("shaderEffect" in actorIdToShaderSet[actor.getId()]) {
        actorIdToShaderSet[actor.getId()].shaderEffect.delete(); // the wrapper
      }
    }

    var shaderOptions = getShadersFromUi();
    var shaderEffect = new dali.ShaderEffect(shaderOptions);
    actor.setShaderEffect(shaderEffect);
    actorIdToShaderSet[ actor.getId() ] = shaderOptions;
    actorIdToShaderSet[ actor.getId() ].shaderEffect = shaderEffect;
  } else {
    console.log("Cannot set shader");
  }
};

var getShader = function(actor) {
  "use strict";
  if( actor.getId() in actorIdToShaderSet) {
    if("shaderEffect" in actorIdToShaderSet[actor.getId()]) {
      return actorIdToShaderSet[actor.getId()].shaderEffect;
    }
  }
  return null;
};

var btnCheckShader = function() {
  "use strict";
  if(checkShader()) { // otherwise dali stops working
    var shaders = getShadersFromUi();
    console.log(extractUniformsFromShaderSource(shaders.vertex));
    console.log(extractUniformsFromShaderSource(shaders.fragment));
    console.log("shader OK");
  } else {
    console.log("shader ERROR");
  }
};

// var textSetShader = function(source) {
//   "use strict";
//   textVertex.value = source.vertex;
//   textFragment.value = source.fragment;
// };


//------------------------------------------------------------------------------
// test functions
//------------------------------------------------------------------------------

function test_hierarchy() {
  console.log("test_hierarchy...");

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
  dali.stage.add(actor);

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

  var root = dali.stage.rootActor;

  actor.remove(hello);
  assert(actor.getChildCount() === 0);

  actor.add(hello);
  assert(actor.getChildCount() === 1);

  var rootLayerCount = root.getChildCount();
  dali.stage.remove(actor); // check these don't assert
  assert(root.getChildCount() === rootLayerCount - 1);

  dali.stage.add(actor);
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

  clear();
  console.log("  -> ok test_hierarchy");
};

function test_registerPropery() {
  console.log("test_registerPropery...");
  var s = dali.stage;
  var root = s.rootActor;

  var another = new dali.TextActor();
  another.parentOrigin = [0.5, 0.5, 0.5];
  another.anchorPoint = [0.5, 0.5, 0.5];
  another.text = "peppa";
  another.name = another.text;
  another.size = [100, 100, 1];
  another.position = [-50, 100, 0];
  root.add(another);

  var imageText = addTextImageFromCanvas("Colour");
  // imageText.connectSignal("touched", onTouched);

  var c = root.getChildAt(1);
  var n = c.getChildCount();
  var p = c.getParent();
  assert(p.getId() == root.getId());

  var matrix = c.worldMatrix;

  assert(matrix.length === 16);

  another.registerProperty("data_output", true);
  assert(another.getPropertyType("data_output") === "BOOLEAN");
  assert(another.data_output === true);
  another.data_output = false;
  assert(another.data_output === false);
  another.data_output = 2.5;
  assert(another.data_output === 2.5);
  assert(another.getPropertyType("data_output") === "FLOAT");

  clear();
  console.log("  -> ok test_registerPropery");
};

function test_js_math() {
  console.log("test_js_math...");
  assert(dali.vectorLength([1, 2, 3, 4]) === Math.sqrt(1 * 1 + 2 * 2 + 3 * 3));
  assert(dali.vectorLengthSquared(dali.normalize([0, 0, 0, 1])) === 0);

  // for(var f=0; f < 6; f+=1)
  // {
  var f = 2;
  assert(1 === dali.vectorLengthSquared(dali.normalize([Math.cos(f) * 10.0,
                                                        Math.cos(f + 1.0) * 10.0,
                                                        Math.cos(f + 2.0) * 10.0,
                                                        1.0
                                                       ])));

  function assertArray(a, b, epsilon) {
    assert(a.length === b.length);
    for (var i = 0, len = a.length; i < len; ++i) {
      assert(a[i] > b[i] - epsilon && a[i] < b[i] + epsilon);
    }
  }

  assertArray(dali.axisAngleToQuaternion([1.0, 2.0, 3.0, Math.PI / 3.0, Math.PI / 2.0]), [0.189, 0.378, 0.567, 0.707], 0.001);

  assertArray(dali.quaternionToAxisAngle([1.1, 3.4, 2.7, 0.932]), [3.03, 9.38, 7.45, 0.74],
              0.01);

  assertArray(dali.vectorCross([0, 1, 0], [0, 0, 1]), [1, 0, 0], 0.001);

  assertArray(dali.vectorAdd([1, 2, 3], [2, 3, 4], [1, 1, 1]), [4, 6, 8], 0.001);

  var mq = dali.vectorAdd(dali.vectorCross([0.045, 0.443, 0.432], [0.612, 0.344, -0.144]),
                          dali.vectorByScalar([0.612, 0.344, -0.144], 0.784),
                          dali.vectorByScalar([0.045, 0.443, 0.432], 0.697));

  assertArray(dali.quatByQuat([0.045, 0.443, 0.432, 0.784], [0.612, 0.344, -0.144, 0.697]), [mq[0], mq[1], mq[2], (0.784 * 0.697) - dali.vectorDot([0.045, 0.443, 0.432], [0.612, 0.344, -0.144])],
              0.001);

  clear();
  console.log("  -> ok test_js_math");
};

function test_getset() {
  console.log("test_getset...");
  threeSquares();
  var col = {};
  collectByName(col);
  var actor = col.red;
  var p = actor.position;
  actor.position = [1, 1, 1];
  assert(compareArrays(actor.position, [1, 1, 1]));
  actor.position = [3, 3, 3];
  assert(compareArrays(actor.position, [3, 3, 3]));
  actor.position = p;

  clear();
  // console.log("-- cache --");
  // console.log(JSON.stringify(dali.internalPropertyCache));
  // console.log("-- cache --");
  console.log("  -> ok test_getset");
};

function test_animation_spline() {
  console.log("test_animation_spline...");
  threeSquares();
  var col = {};
  collectByName(col);
  var actor = col.red;

  var a = new dali.Animation();
  var path = new dali.Path();

  path.points = [
    [-150, -50, 0],
    [0.0, 70.0, 0.0],
    [190.0, -150.0, 0.0]
  ];
  path.curvature = 0.35; // generates path.controlPoints
  // or set path.controlPoints directly

  assert(compareArrays(path.points, [
    [-150, -50, 0],
    [0.0, 70.0, 0.0],
    [190.0, -150.0, 0.0]
  ]));

  assert(compareArrays(path.controlPoints, [
    [-97.5, -8, 0],
    [-66.94940948486328, 76.16658020019531, 0],
    [101.31224060058594, 60.66832733154297, 0],
    [123.5, -73, 0]
  ]));

  a.setDuration(3);
  a.animatePath(actor, path, [1, 0, 0], "linear", 0, 3);
  a.play();

  function checkPos() {
    assert(actor.position = path.points[2]);
    clear();
    actor.delete();
    path.delete();
    a.delete();
    console.log("  -> ok test_animation_spline");
  }

  window.setTimeout(checkPos, 4000);
}

function test_animation() {
  console.log("test_animation...");
  threeSquares();
  var col = {};
  collectByName(col);
  var actor = col.red;

  var a = new dali.Animation();
  a.setDuration(3);
  a.animateTo(actor, "position", [20, 0, 0], "linear", 0, 3);
  a.play();

  function checkNewPos() {
    assert(actor.position = [120, 100, 0]);
    clear();
    a.delete();
    actor.delete();

    console.log("  -> ok test_animation");
  }

  function checkPos() {
    assert(actor.position = [20, 0, 0]);
    actor.position = [100, 100, 0];

    var a = new dali.Animation();
    a.setDuration(3);
    a.animateBy(actor, "position", [20, 0, 0], "linear", 0, 3);
    a.play();

    window.setTimeout(checkNewPos, 4000);
  }

  window.setTimeout(checkPos, 4000);
}

function test_shadereffect1() {
  console.log("test_shadereffect1...");

  threeSquares();
  var col = {};
  collectByName(col);
  var actor = col.red;

  var shader = new dali.ShaderEffect({
    vertex: "void main(void)\n" +
      "{\n" +
      "gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
      "vTexCoord = aTexCoord;\n" +
      "}\n",
    fragment: "precision mediump float;\n" +
      "uniform vec4 weight;\n" +
      "void main()\n" +
      "{\n" +
      "  gl_FragColor = weight;\n" +
      "}\n"
  });
  actor.setShaderEffect(shader);

  var final = [1, 0, 0, 1];

  var a = new dali.Animation();
  a.setDuration(3);
  a.setLooping(false);
  a.animateTo(shader, "weight", final, "linear", 0, 3);
  a.play();

  a.delete();

  function checkPos() {
    console.log(shader.weight);
    console.log(final);
    assert(compareArrays(shader.weight, final));

    console.log("  -> ok test_shadereffect1");
    shader.delete(); // wrapper
    actor.delete(); // wrapper
  }

  window.setTimeout(checkPos, 4000);

}

function test_views() {
  console.log("test_views");

  var w = dali.canvas.width;
  var h = dali.canvas.height;
  var col = dali.getClearColor(0);
  console.log(col);

  function one() {
    dali.onePane();
    dali.setFrontView(0, 0, 0, w, h);
    dali.setClearColor(0, col);
  }

  function three() {
    dali.threePane();
    dali.setClearColor(0, [0.4, 0, 0, 1]);
    dali.setClearColor(1, [0, 0.4, 0, 1]);
    dali.setClearColor(2, [0, 0, 0.4, 1]);
    dali.setFrontView(0, 0, 0, w / 2 - 5, h);
    dali.setTopView(1, w / 2, 0, w / 2, h / 2 - 5);
    dali.setRightView(2, w / 2, h / 2 + 5, w / 2, h / 2 - 5);

    window.setTimeout(one, 1000);
  }

  function two() {
    dali.twoPane();
    dali.setFrontView(0, 0, 0, w / 2 - 10, h);
    dali.setTopView(1, 210, 0, w / 2 - 10, h);
    dali.setClearColor(0, [0.4, 0, 0, 1]);
    dali.setClearColor(1, [0, 0.4, 0, 1]);

    window.setTimeout(three, 1000);
  }

  one();

  window.setTimeout(two, 1000);
}

function test_blinking() {
  "use strict";
  var layer = new dali.Layer();
  layer.name = "frameLayer";
  dali.stage.add(layer);

  var a = dali.createSolidColorActor([0.5, 0.5, 0.5, 1],
                                     false, [0, 0, 0, 1],
                                     0);
  a.size = [100,100,1];

  layer.add(a);

  var camera = firstCamera();

  camera.position = [ camera.position[0]+10, camera.position[1]+20, camera.position[2] + 10 ];

  layer.delete();   // wrapper
  a.delete();       // wrapper
  camera.delete();  // wrapper
}

//------------------------------------------------------------------------------
// regression test
//------------------------------------------------------------------------------

function regression_test() {
  test_hierarchy();
  test_registerPropery();
  test_js_math();
  test_getset();
  test_animation();
  test_animation_spline();
  test_shadereffect1();
  test_views();
}

function remote_execution() {
  regression_test();
}

//------------------------------------------------------------------------------
// scratch
//------------------------------------------------------------------------------
// function animationDataShaderEffect(shaderSource)
// {
//   var uniforms = [];
//   dali.extractUniformsFromShaderSource(shaderSource.vertex).map( function(item) {uniforms.push(item); } );
//   dali.extractUniformsFromShaderSource(shaderSource.fragment).map( function(item) {uniforms.push(item); } );

//   var uniformNames = [];
//   for(var i =0; i < uniforms.length; i++)
//   {
//     var type = uniforms[i][0];
//     var name = uniforms[i][1];
//     var typeIndex = dali.supportedUniformTypes.indexOf(type);
//     if(dali.defaultUniforms.indexOf(name) < 0)
//     {
//       if(typeIndex >= 0 && uniformNames.indexOf(name) < 0 )
//       {
//         console.log("Uniform registered:", name);
//         uniformNames.push(name);
// }


function animateShaderEffect2(actor) {
  var shader = new dali.ShaderEffect({
    vertex: shaderSource2.vertex,
    fragment: shaderSource2.fragment
  });

  actor.setShaderEffect(shader);

  var final = [5, 5, 5, 1];

  var a = new dali.Animation();
  a.setDuration(3);
  a.setLooping(true);
  a.animateTo(shader, "weight", final, "linear", 0, 3);

  a.play();

  a.delete();

}


// function testit() {
//     onePane();
//     threePane();
//     clear();
//     threeSquares();

//     clear();
//     threeSquares2();

//     stage = dali.stage;
//     root = dali.stage.rootActor;
//     tasks = dali.stage.getRenderTaskList();
//     renderTask = tasks.getTask(0);
//     camera = renderTask.getCameraActor();

//     resize(800, 800);

//     threePane();

//     col = {};
//     collectByName(col);

//     test_getset(col.red);

//     col.red.position = col.blue.worldPosition;

//     worldPos = dali.worldToScreen(col.red.worldPosition);
//     worldPos = dali.worldToScreen(col.red.worldPosition, renderTask);

//     topLeftCorner = dali.worldToScreen(dali.matrixByVector(col.red.worldMatrix, [-col.red.size[0], -col.red.size[1], 0]));
//     // do function to get four corners.....


//     var atest = function(angleA, angleB) {
//         q1 = dali.axisAngleToQuaternion([0, 1, 0, radian(angleA)]); // yaw around to look at scene down -ve z
//         qIdentity = [0, 0, 0, 1];

//         q2 = dali.axisAngleToQuaternion([1, 0, 0, radian(angleB)]); // pitch down to look at scene

//         dali.quatByQuat(qIdentity, qIdentity);

//         q = dali.quaternionToAxisAngle(quatByQuat(q1, q2));

//         c.position = [0, -2000, 0]; // camera.position; // [ camera.position[2], 0, 0 ];
//         c.rotation = [q[0], q[1], q[2], degree(q[3])];



//         col = {};
//         collectByName(col);


//         actor = col.blue;


//         sPos = dali.screenSpace(col.blue);
//         sPos.centre;

//     }(180, -90);

//     c.inheritRotation = true;

//     var donttest = function() {
//         threePane();

//         resize(800, 800);
//     };


//     testGetSet();
// }

// function dont_run() {
//     loader = new dali.ObjectLoader(testfile);
//     loader;

//     mat_uuid = "14D499F1-27EF-45BF-A457-FD24DAB11205";
//     mat = loader.__getMaterial(mat_uuid);
//     loader.matByUUID;

//     mesh_uuid = "2f167add-e571-47c2-9da2-6f0e45cc1119";
//     mesh = loader.__getMeshData(mesh_uuid, mat_uuid);


//     f_geom = testfile["geometries"][0];
//     f_geom["uuid"];
//     "metadata" in f_geom;
//     md = f_geom["metadata"];
//     f_geom["type"];
//     f_type = f_geom["type"];
//     f_faces = f_geom["data"]["faces"];

//     f_indices = f_geom["data"]["indices"];
//     f_posns = f_geom["data"]["vertices"];
//     f_norms = f_geom["data"]["normals"];
//     f_uvs = f_geom["data"]["uvs"];
//     f_faces = f_geom["data"]["faces"];
//     if (f_faces) {
//         f_type = f_geom["data"]["faces"][0]; // ["type"];
//     }

//     foo = function() {
//         verts = new dali.VectorVertex();
//         vert = []; //new dali.Vertex();
//         for (var i = 0, len = f_posns.length / 3; i < len; i++) {
//             vert.push(f_posns[(i * 3) + 0]);
//             vert.push(f_posns[(i * 3) + 1]);
//             vert.push(f_posns[(i * 3) + 2]);
//             if (f_norms) {
//                 vert.push(f_norms[(i * 3) + 0]);
//                 vert.push(f_norms[(i * 3) + 1]);
//                 vert.push(f_norms[(i * 3) + 2]);
//             } else {
//                 vert.push(0);
//                 vert.push(0);
//                 vert.push(0);
//             }

//             if (f_uvs) {
//                 vert.push(f_uvs[(i * 3) + 0]);
//                 vert.push(f_uvs[(i * 3) + 1]);
//             } else {
//                 vert.push(0);
//                 vert.push(0);
//             }
//             verts.push_back(vert);
//         }
//     }();

//     f_uvs;

// }


// function dontrun_testingScreenToLocal() {

//     I = dali.matrixIdentity();
//     T = dali.matrixTransform(100, 10, 10);

//     v = dali.matrixByVector(T, [0, 0, 0, 1]);

//     im = dali.matrixByVector(dali.matrixInverse(T), v);


//     renderTask = first();
//     camera = firstCamera();
//     screenX = 200;
//     screenY = 200;
//     col = {};
//     threeSquares2();
//     collectByName(col);
//     actor = col.green;
//     col.green.rotation = [0, 0, 0];
//     col.green.position = [0, 0, 0];
//     dali.screenToLocal(screenX, screenY, col.green, renderTask);
//     col.green.worldMatrix;
//     col.green.position;
//     inverseMvp = dali.matrixInverse(
//         dali.matrixByMatrix(dali.matrixByMatrix(col.green.worldMatrix, camera.viewMatrix), camera.projectionMatrix));


//     renderTask.screenToLocal(col.green, screenX, screenY);
//     col.green.size;

//     camera.viewMatrix;
//     camera.projectionMatrix;
//     modelView = dali.matrixByMatrix(col.green.worldMatrix, camera.viewMatrix); // ok

//     // dali.matrixByMatrix( [1,1,1,1,
//     //                       1,1,1,1,
//     //                       1,1,1,1,
//     //                       1,1,1,1],
//     //                      [1,2,3,4,
//     //                       4,5,6,7,
//     //                       8,9,10,11,
//     //                       12,13,14,15]);
//     //                      );
//     camera.projectionMatrix;

//     mvp = dali.matrixByMatrix(modelView, camera.projectionMatrix);
//     inversemvp = dali.matrixInverse(mvp);

//     col.green.worldMatrix;

//     camera.viewMatrix;
//     dali.matrixInverse(camera.viewMatrix);
//     dali.matrixByVector(camera.viewMatrix, [0, 0, 0, 1]);
//     dali.matrixByVector(dali.matrixInverse(camera.viewMatrix), [0, 0, 800, 1]);

// };

// function dontrun_testing() {

//     mv = new dali.MultipleViews();
//     mv.enable;
//     mv.enable = false;
//     mv.enable = true;
//     threeSquares2();
//     mv.enableColor = false;
//     mv.enableColor = true;
//     mv.leftWidth = 50;
//     mv.leftWidth = 150;
//     mv.leftWidth = 250;
//     mv.rightTopHeight = 200;
//     mv.enable = false;
//     mv.screenWidth = 400;
//     mv.screenHeight = 400;
//     col = {};
//     collectByName(col);
//     mv.first.worldToScreen(col.red.worldPosition);
//     mv.second.worldToScreen(col.green.worldPosition);
//     mv.third.worldToScreen(col.red.worldPosition);

//     mv.first.screenToLocal(col.red, 85, 183);
//     mv.first.screenToLocal(col.green, 300, 100);
//     col.green.worldPosition;
//     col.green.position = [0, 0, 0];
//     col.green.rotation = [0, 0, 0, 0];
//     col.green.size;
//     dali.canvas.width;


//     // // near/far plane hit points
//     nearHitPoint = dali.screenToActor({}, [0, 0, 0]);
//     farHitPoint = dali.screenToActor({}, [0, 0, 1]);

//     // rayOrigin = nearHitPoint;
//     // rayDir = [farHitPoint[0] - nearHitPoint[0],
//     //           farHitPoint[1] - nearHitPoint[1],
//     // farHitPoint[2] - nearHitPoint[2]];

//     // modelMatrix = actor.matrix;//   camera.viewMatrix;

//     // localRay = dali.matrixByVector(dali.matrixInverse(modelMatrix),
//     //                                rayDir);


//     // dali.matrixByVector( camera.viewMatrix,

// }


// testingPanes = function() {
//     c = firstCamera();

//     console.log("aasfsdfsafsfsaa");

//     col = {};
//     collectByName(col);

//     dali.debugPrintTree(c);
//     dali.debugPrintRenderTask(first());
//     dali.debugPrintRenderTasks();

//     var w = dali.canvas.width;
//     var h = dali.canvas.height;

//     dali.onePane();
//     dali.setFrontView(0, 0, 0, w, h);

//     dali.twoPane();
//     dali.setFrontView(0, 0, 0, w / 2 - 10, h);
//     dali.setTopView(1, 210, 0, w / 2 - 10, h);

//     dali.threePane();
//     dali.setClearColor(0, [0.4, 0, 0, 1]);
//     dali.setClearColor(1, [0, 0.4, 0, 1]);
//     dali.setClearColor(2, [0, 0, 0.4, 1]);
//     dali.setFrontView(0, 0, 0, w / 2 - 5, h);
//     dali.setTopView(1, w / 2, 0, w / 2, h / 2 - 5);
//     dali.setRightView(2, w / 2, h / 2 + 5, w / 2, h / 2 - 5);

//     runTests();

//     second();


//     threeSquares2();
//     first();
//     firstCamera();
//     col = {};
//     collectByName(col);
//     col.green.rotation = [0, 0, 0];
//     c = first().getCameraActor();
//     c.ok();
//     v = new dali.MultipleViews();
//     v.enable = true;
//     v.enable = false;
//     v.first_cam.aspectRatio = 0.5;
//     v.first_cam.aspectRatio = 1;
//     v.first_cam.fieldOfView;
//     v.second_cam.fieldOfView;

//     firstCamera().position;

//     x = 0;
//     y = 0;
//     w = 400;
//     h = 400;
//     rendertask = first();

//     dali.setFrontView(first(), 0, 0, 400, 400);
//     dali.setTopView(first(), 0, 0, 400, 400);
//     dali.setRightView(first(), 0, 0, 400, 400);

//     // var c = rendertask.getCameraActor();

//     x = 0;
//     y = 0;
//     w = 400;
//     h = 400;
//     rendertask = first();

//     c = new dali.CameraActor();
//     c = rendertask.getCameraActor();
//     layer0 = dali.stage.getLayer(0);
//     layer0.add(c);

//     rendertask.setViewportSize([w, h]);
//     rendertask.setViewportPosition([x, y]);

//     // //second_ca.parentOrigin = [0.5, 0.5, 0.5];
//     // c.inheritRotation = true;
//     q1 = dali.axisAngleToQuaternion([0, 1, 0, dali.radian(180)]); // yaw around to look at scene down -ve z
//     q2 = dali.axisAngleToQuaternion([1, 0, 0, dali.radian(-90)]); // pitch to look at scene
//     q = dali.quaternionToAxisAngle(dali.quatByQuat(q1, q2));
//     //  console.log("" + [q[0] + ":" + q[1] + ":" + q[2] + ":" + dali.degree(q[3])]);
//     c.position = [w / 2, -800, 0]; // camera.position; // [ camera.position[2], 0, 0 ];
//     c.rotation = [q[0], q[1], q[2], dali.degree(q[3])]; // tbd; should really all be in radians
//     rendertask.setCameraActor(c);
//     //c.aspectRatio=w/h;

//     layer0.delete(); // wrapper
//     c.delete(); // wrapper


//     col = {};
//     collectByName(col);
//     col.green.position;
//     var x = 0;
//     testPropertySet = function() {
//         col.green.position = [x, 0, 0];
//         console.log(col.green.position, "x should be ", x);
//         x += 1;
//     };

//     testPropertySet();

// };

var testfile = { // output from clara.io "Threejs scene output"
  "metadata": {
    "version": 4.3, // This isnt as its documented on threejs website
    "type": "Object", // and the general format looks more like format V3.
    "generator": "ObjectExporter"
  },
  "geometries": [{
    "uuid": "2f167add-e571-47c2-9da2-6f0e45cc1119",
    "type": "Geometry",
    "data": {
      "vertices": [
        0.5,
        0.5,
        0.5,
        0.5,
        0.5, -0.5,
        0.5, -0.5,
        0.5,
        0.5, -0.5, -0.5, -0.5,
        0.5, -0.5, -0.5,
        0.5,
        0.5, -0.5, -0.5, -0.5, -0.5, -0.5,
        0.5
      ],
      "normals": [
        1,
        0,
        0, -1,
        0,
        0,
        0,
        1,
        0,
        0, -1,
        0,
        0,
        0,
        1,
        0,
        0, -1
      ],
      "uvs": [
        [
          0,
          1,
          0,
          0,
          1,
          0,
          1,
          1
        ]
      ],
      "faces": [
        56,

        0,
        2,
        3,

        0,
        1,
        2,

        0,
        0,
        0,

        0,

        56,
        0,
        3,
        1,
        0,
        2,
        3,
        0,
        0,
        0,
        0,
        56,
        4,
        6,
        7,
        0,
        1,
        2,
        1,
        1,
        1,
        1,
        56,
        4,
        7,
        5,
        0,
        2,
        3,
        1,
        1,
        1,
        1,
        56,
        4,
        5,
        0,
        0,
        1,
        2,
        2,
        2,
        2,
        2,
        56,
        4,
        0,
        1,
        0,
        2,
        3,
        2,
        2,
        2,
        2,
        56,
        7,
        6,
        3,
        0,
        1,
        2,
        3,
        3,
        3,
        3,
        56,
        7,
        3,
        2,
        0,
        2,
        3,
        3,
        3,
        3,
        3,
        56,
        5,
        7,
        2,
        0,
        1,
        2,
        4,
        4,
        4,
        4,
        56,
        5,
        2,
        0,
        0,
        2,
        3,
        4,
        4,
        4,
        4,
        56,
        1,
        3,
        6,
        0,
        1,
        2,
        5,
        5,
        5,
        5,
        56,
        1,
        6,
        4,
        0,
        2,
        3,
        5,
        5,
        5,
        5
      ]
    }
  }],
  "materials": [{
    "uuid": "14D499F1-27EF-45BF-A457-FD24DAB11205",
    "type": "MeshPhongMaterial",
    "color": 11579568,
    "ambient": 11579568,
    "emissive": 0,
    "specular": 0,
    "shininess": 50,
    "opacity": 1,
    "transparent": false,
    "wireframe": false
  }],
  "object": {
    "uuid": "BFEFB48D-0E6E-46A6-8568-5E258BA17078",
    "type": "Scene",
    "matrix": [
      1,
      0,
      0,
      0,
      0,
      1,
      0,
      0,
      0,
      0,
      1,
      0,
      0,
      0,
      0,
      1
    ],
    "children": [{
      "uuid": "aa901bec-9e47-4b3b-bf3c-4efb0fe5d298",
      "name": "Box",
      "type": "Mesh",
      "geometry": "2f167add-e571-47c2-9da2-6f0e45cc1119",
      "material": "14D499F1-27EF-45BF-A457-FD24DAB11205",
      "castShadow": true,
      "receiveShadow": true,
      "matrix": [
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1
      ]
    }]
  }
};



//
//
// Event/widget setup
//
//

fileInput.addEventListener("change", function(// e
) {
  "use strict";

  var file = fileInput.files[0];

  asyncFileReaderGetTypedBuffer(file, addImageActor);

});

fileInputRGB.addEventListener("change", function(// e
) {
  "use strict";

  var file = fileInputRGB.files[0];

  asyncFileReaderGetTypedBuffer(file, addImageActorRGB);
});

fileInputEmscripten.addEventListener("change", function() {
  "use strict";

  var file = fileInputEmscripten.files[0];

  var i = new dali.ImageActor();

  i.image = {
    "filename": file.name
  };

  i.visible = true;
  i.anchorPoint = [0.5, 0.5, 0.5];
  i.parentOrigin = [0.5, 0.5, 0.5];
  i.position = [0, 0, -10];
  i.size = [100, 100, 1];
  i.name = file.name.replace(/-/g, "_").replace(/\./g, "_");

  app.addActor(i);

  // addImageActor(file);
});

btnTextActorAdd.addEventListener("click", function(// e
) {
  "use strict";

  var text = textInput.value;
  if (text) {
    console.log(text);

    addTextImageFromCanvas(text);
  }

});

shaderButton.addEventListener("click", function(// e
) {
  "use strict";

  var actor = eventHandler.touchedActor;

  if (actor) {
    setShader(actor);
  } else {
    console.log("No actor to add shaders to");
  }
});

shaderCheckButton.addEventListener("click", function(// e
) {
  "use strict";

  btnCheckShader();

});


$("#btnCreateSolidActor")[0].addEventListener("click", function(// e
) {
  "use strict";

  var c = getColorForElement("solidActorColor");
  var cb = getColorForElement("solidActorBorderColor");
  var a = dali.createSolidColorActor( c,
                                      Number($("#solidActorBorder")[0].value),
                                      cb,
                                      Number($("#solidActorBorderSize")[0].value) );
  a.size = [100,100,1];
  app.addActor(a);
});

window.setTimeout(init, 1000);

window.setInterval(checkShader, 3000);


eventHandler.handlersSelect.push(onActorSelected);
// this.handlersSelect = []; // valid; touchedActor(can be null), drag_dx, drag_dy

$(document).on("shown.bs.tab", "a[data-toggle=\"tab\"]", rebuildTree);


function init() {
  "use strict";

  // $("#shaderTab").click();

  // createTextAreaWithLines("textVertex", 13);
  // createTextAreaWithLines("textFragment", 7);


  //
  // shader source drop down
  //
  var elem = document.getElementById("shaderDropDown");
  var count = elem.children.length;
  for (var i = 0; i < count; i++) {
    elem.removeChild(elem.children[0]);
  }
  var li;
  var a;
  for(i = 0; i < shaderSources.length; i++) {
    li = document.createElement("li");
    a = document.createElement("a");
    a.href = "javascript:selectShader(" + i + ")";
    a.text = shaderSources[i].name;
    li.appendChild(a);
    elem.appendChild(li);
  }

  li = document.createElement("li");
  li.className = "divider";
  elem.appendChild(li);

  li = document.createElement("li");
  a = document.createElement("a");
  a.href = "javascript:selectShader(-1)";
  a.text = "Shader for current actor"; //shaderSources[0].name;
  li.appendChild(a);
  elem.appendChild(li);


  //
  //
  //
  // $("#custom").spectrum({ color: "#f00",
	//                         preferredFormat: "rgb",
	//                         showPalette: true,
	// 	                      showInput: true,
	// 	                      showAlpha: true
  //                       });



  // var editor = ace.edit("editor");
  // editor.setTheme("ace/theme/monokai");
  // editor.getSession().setMode("ace/mode/javascript");

  selectShader(0);

}

