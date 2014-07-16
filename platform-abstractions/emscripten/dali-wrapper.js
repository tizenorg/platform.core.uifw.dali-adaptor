/* eslint-env browser */
/* eslint "brace-style": [2, "1tbs"] */
/* eslint "no-console": 0 */

var dali;
var document;
var console;

var step = 0;

/** @private */
dali.internalGetGlyphImage = function(sFontFamily, sFontStyle, fFontSize, iChar) {
  "use strict";

  var buffer = document.createElement("canvas");
  buffer.width = fFontSize;
  buffer.height = fFontSize;
  var ctx = buffer.getContext("2d");

  ctx.font = sFontStyle + " " + fFontSize + "px " + sFontFamily;
  ctx.fillText(String.fromCharCode(iChar), 0, 0 + fFontSize);

  var imageData = ctx.getImageData(0, 0, fFontSize, fFontSize);

  // emscripten checks only for this type if array in converting to strings
  // (getImageData() returns Uint8CheckedArray or some such)
  //  var uint8array = new Uint8Array( imageData.data );

  if (0) // write data onto a test canvas to check its correct
  {
    // console.log(sFontStyle + " " + fFontSize + "px " + sFontFamily + " char:" + iChar);
    // console.log("GetGlyphImage" + imageData + " size " + imageData.width + "," + imageData.height);

    var testCanvas = document.getElementById("testCanvas");
    var testCtx = testCanvas.getContext("2d");

    step = step + fFontSize;
    testCtx.putImageData(imageData, fFontSize, step);
  }

  return imageData.data; // return uint8array; //
};

/** @private */
dali.internalSetupProperties = function(handle) {
  "use strict";

  var props = handle.getProperties();

  // console.log("props", props.size());

  for (var i = 0; i < props.size(); i++) {
    // anon function because of closure with defineProperty
    // (if just variable in loop then the variable 'address' is captured, not the value
    //  so it becomes last value set)
    (function(name, object) {
      // @todo Dali error?? name lengths should never be zero
      if (name.length) {
        Object.defineProperty(handle, name, {
          enumerable: true,
          configurable: false,
          get: function() {
            return dali.marshallGetProperty(handle, name);
          },
          set: function(newValue) {
            dali.marshallSetProperty(handle, name, newValue);
          }
        });
      }
    })(props.get(i), handle);
  }
  return handle;
};

/** @private */
dali.internalGetImage = function() {
  "use strict";

  console.log("GetImage called:");

  // return
  // v["data"]
  // v["w"]
  // v["h"]
};

/** @private */
dali.internalGetImageMetadata = function() {
  "use strict";

  console.log("GetImageMetadata called");

  // return
  // v["w"] = 100;
  // v["h"] = 100;
};

dali.setCallbackGetGlyphImage(dali.internalGetGlyphImage);
dali.setCallbackGetImage(dali.internalGetImage);
dali.setCallbackGetImageMetadata(dali.internalGetImageMetadata);

//
//
// Takes a JS value and sets the property. Generally does the right thing without needing dali.PropertyXXX.
//
dali.DaliPropertyValue = function(object, name, value) {
  "use strict";

  var setProperty = false;
  if (typeof(value) === "boolean") {
    setProperty = new dali.PropertyBoolean(value);
  } else if (typeof(value) === "number") {
    setProperty = new dali.PropertyFloat(value);
  } else if (typeof(value) === "string") {
    setProperty = new dali.PropertyString(value);
  } else if (typeof(value) === "object") {
    if (value.constructor === dali.PropertyValue) {
      setProperty = value;
    } else {
      var p = object.getProperty(name);
      var type = p.getType();
      p.delete();
      // This shouldnt need .value on the end? Maybe because I'm currently in emscritpen devel branch and it will change
      if (type === dali.PropertyType.ROTATION.value) {
        if (value.length === 3) {
          setProperty = new dali.PropertyEuler(value);
        } else {
          setProperty = new dali.PropertyAxisAngle(value);
        }
      } else if (value.length) {
        if (value.length === 2) {
          setProperty = new dali.PropertyVector2(value);
        } else if (value.length === 3) {
          setProperty = new dali.PropertyVector3(value);
        } else if (value.length === 4) {
          setProperty = new dali.PropertyVector4(value);
        } else {
          // wo betide those who want length 2,3,4 arrays
          setProperty = new dali.PropertyArray(value);
        }
      } else {
        setProperty = new dali.PropertyMap(value);
      }
    }
  } else {
    throw object.toString() + " " + name.toString() + " " + value.toString();
  }

  return setProperty;
};


/** @private */
dali.marshallSetProperty = function(object, name, value) {
  "use strict";

  var setProperty = new dali.DaliPropertyValue(object, name, value);

  if (setProperty) {
    object.setProperty(name, setProperty);
    setProperty.delete();
  }
};


dali.getPropertyType = function(object, name) {
  "use strict";

  var p = object.getProperty(name);
  var type = p.getType();
  p.delete();
  return type;
};

//
// GetProperty.
// Needs to get the property value which needs to be manually deleted which
// this function does for you.
//

//1  BOOLEAN,         ///< A boolean type
//2  FLOAT,           ///< A float type
//3  INTEGER,         ///< An integer type
//4  UNSIGNED_INTEGER,///< An unsigned integer type
//5  VECTOR2,         ///< a vector array of size=2 with float precision
//6  VECTOR3,         ///< a vector array of size=3 with float precision
//7  VECTOR4,         ///< a vector array of size=4 with float precision
//8  MATRIX3,         ///< a 3x3 matrix
//9  MATRIX,          ///< a 4x4 matrix
//10 RECTANGLE,       ///< an integer array of size=4
//11 ROTATION,        ///< either a quaternion or an axis angle rotation
//12 STRING,          ///< A string type
//13 ARRAY,           ///< an array of Property::Value
//14 MAP,             ///< a string key to Property:value mapping
//   TYPE_COUNT       ///< The number of supported property types
/** @private */
dali.marshallGetProperty = function(object, name) {
  "use strict";

  var ret;
  var p = object.getProperty(name);
  if (!p) {
    throw new Error("Property doesnt exist?");
  }
  var type = p.getType();

  if (type === 0) {
    // raise?
    throw new Error("Property has no type?");
  } else if (type === 1) {
    ret = p.getBoolean();
  } else if (type === 2) {
    ret = p.getFloat();
  } else if (type === 3) {
    ret = p.getInteger();
  } else if (type === 4) {
    ret = p.getInteger();
  } else if (type === 5) {
    ret = p.getVector2();
  } else if (type === 6) {
    ret = p.getVector3();
  } else if (type === 7) {
    // VECTOR4
    ret = p.getVector4();
  } else if (type === 8) {
    // MATRIX3
    ret = p.getMatrix3();
  } else if (type === 9) {
    // MATRIX
    ret = p.getMatrix();
  }
  // else if( type === 10 ) // RECT<INT>
  // {
  //   ret = p.getMatrix();
  // }
  else if (type === 11) {
    // ROTATION
    ret = p.getRotation();
  } else if (type === 12) {
    ret = p.getString();
  } else if (type === 13) {
    // array
    ret = p.getArray();
  } else if (type === 14) {
    // map
    ret = p.getMap();
  }
  p.delete();
  return ret;
};

/** @private */
dali.internalDepthFirstCollection = function(actor, collection) {
  for (var i = 0; i < actor.getChildCount(); i++) {
    var a = actor.getChildAt(i); // adds properties in dotted
    dali.internalAddActorMethods(a); // add extra methods
    collection.push(a);
    dali.internalDepthFirstCollection(a, collection);
  }
}

/** @private */
dali.internalAddActorMethods = function(actor) {
  "use strict";
  // some functions need to be adorned with properties that we add in the javascript layer
  actor.getParent = function() {
    var bareActorWrapper = this.__getParent();
    // add properties to the bare ActorWrapper
    return dali.internalSetupProperties(bareActorWrapper);
  };
  actor.findChildById = function(index) {
    var bareActorWrapper = this.__findChildById(index);
    return dali.internalSetupProperties(bareActorWrapper);
  };
  actor.findChildByName = function(name) {
    var bareActorWrapper = this.__findChildByName(name);
    if (bareActorWrapper.ok()) {
      return dali.internalSetupProperties(bareActorWrapper);
    } else {
      return null;
    }
  };
  actor.getChildAt = function(index) {
    var bareActorWrapper = this.__getChildAt(index);
    return dali.internalSetupProperties(bareActorWrapper);
  };
  actor.findAllChildren = function() {
    var col = [];
    dali.internalDepthFirstCollection(this, col);
    return col;
  };
  actor.getChildren = function() {
    var col = [];
    for (var i = 0, len = this.getChildCount(); i < len; i++) {
      var c = this.getChildAt(i);
      dali.internalAddActorMethods(c);
      col.push(c);
    }
    return col;
  };
  actor.directChildren = actor.getChildren;

  actor.connectSignal = function(signalName, callback) {
    // wrap in closure so we can setup properties in . notation
    // and add actor methods to c++ raw ActorWrapper
    this.__connectSignal(
      signalName, (function(callback) {
        return function(actorWrapper) {
          dali.internalSetupProperties(actorWrapper);
          dali.internalAddActorMethods(actorWrapper);
          callback(actorWrapper);
        };
      })(callback)
    );
  };

  actor.setImageData = function(imageData) {
    // imageData as returned from canvas.getImageData
    var uint8array = new Uint8Array(imageData.data);

    actor.__setImageData(imageData.width, imageData.height, uint8array);
  };

};

dali.internalGetRenderTask = function(index) {
  return stage.getRootLayer().getRenderTaskList().getTask(index)
};

dali.createSolidColorActor = function(color, border, borderColor, borderSize) {
  var a = dali.__createSolidColorActor(color, border, borderColor, borderSize);
  dali.internalAddActorMethods(a);
  dali.internalSetupProperties(a);
  return a;
};

dali.create = function(name) {
  "use strict";

  var handle = dali.createActor(name);

  if (handle.ok()) {
    dali.internalAddActorMethods(handle);
  } else {
    handle = dali.createAny(name);
  }

  dali.internalSetupProperties(handle);

  return handle;
};

//
// Adds and animateTo( ...property... ) to an animation object set.
//
dali.animateTo = function(animation, object, propertyName, value, alphaFunction, delay, duration) {
  "use strict";

  var propertyValue = new dali.DaliPropertyValue(object, propertyName, value);
  if (propertyValue) {
    animation.animateTo(object, propertyName, propertyValue, alphaFunction, delay, duration);
    propertyValue.delete();
  } else {
    throw new Error("Unknown property?");
  }

};

dali.animateBy = function(animation, object, propertyName, value, alphaFunction, delay, duration) {
  "use strict";

  var propertyValue = new dali.DaliPropertyValue(object, propertyName, value);
  if (propertyValue) {
    animation.animateBy(object, propertyName, propertyValue, alphaFunction, delay, duration);
    propertyValue.delete();
  } else {
    throw new Error("Unknown property?");
  }
};

dali.debugPrintProperties = function(typeName) {
  "use strict";

  var t = dali.getTypeRegistry();
  // var types = t.getTypeNames()
  var info = t.getTypeInfo(typeName);
  var props = info.getProperties();
  for (var i = 0; i < props.size(); i++) {
    console.log(i + ":" + props.get(i));
  }
};

dali.debugPrintTypes = function() {
  "use strict";

  var t = dali.getTypeRegistry();
  var types = t.getTypeNames();
  for (var i = 0; i < types.size(); i++) {
    console.log(types.get(i));
  }

};


dali.__theStage = (function() {
  this.stageWrapper = dali.__getStage();

  this.add = function(actor) {
    this.stageWrapper.__add(actor);
  };

  this.remove = function(actor) {
    this.stageWrapper.__remove(actor);
  };

  this.getRootLayer = function() {
    var wrapper = this.stageWrapper.__getRootLayer();
    dali.internalSetupProperties(wrapper);
    dali.internalAddActorMethods(wrapper);
    return wrapper;
  };

  this.getRenderTaskList = function() {
    return this.stageWrapper.__getRenderTaskList();
  };

  this.setBackgroundColor = function(vector4) {
    return this.stageWrapper.__setBackgroundColor(vector4);
  };

  return this;
})();

dali.getStage = function() {
  return dali.__theStage;
}

dali.init = function() {
  "use strict";

  var t = dali.getTypeRegistry();
  var types = t.getTypeNames();

  //
  // Add creation functions so "var image = new dali.ImageActor()" works
  //
  for (var i = 0; i < types.size(); i++) {
    // anon function because of closure with defineProperty
    // (if just variable in loop then the variable 'address' is captured, not the value
    //  so it becomes last value set)
    (function(name) {
      // @todo Dali error?? name lengths should never be zero
      if (name.length && name != "Animation") {
        Object.defineProperty(dali, name, {
          enumerable: true,
          configurable: false,
          get: function() {
            return function() {
              // console.log(name);
              return dali.create(name);
            };
          }
        });
      }
    })(types.get(i));
  }

  //
  // Special wrappings overriding the dali.Create() which just returns a HandleWrapper
  //
  dali.Animation = dali.AnimationWrapper;

}(); // call init

//
// chain load main.js
//
var s = document.createElement("script");
s.src = "test-devel.js";
document.body.appendChild(s);
