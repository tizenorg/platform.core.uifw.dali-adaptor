/* eslint-env browser */
/* eslint "brace-style": [2, "1tbs"] */
/* eslint "no-console": 0 */

var dali;
var document;
var console;

var step = 0;

var testfile = {
  "metadata": {
    "version": 4.3,
    "type": "Object",
    "generator": "ObjectExporter"
  },
  "geometries": [
    {
      "uuid": "2f167add-e571-47c2-9da2-6f0e45cc1119",
       "type": "Geometry",
      "data": {
        "vertices": [
          0.5,
          0.5,
          0.5,
          0.5,
          0.5,
            -0.5,
          0.5,
            -0.5,
          0.5,
          0.5,
            -0.5,
            -0.5,
            -0.5,
          0.5,
            -0.5,
            -0.5,
          0.5,
          0.5,
            -0.5,
            -0.5,
            -0.5,
            -0.5,
            -0.5,
          0.5
        ],
        "normals": [
          1,
          0,
          0,
            -1,
          0,
          0,
          0,
          1,
          0,
          0,
            -1,
          0,
          0,
          0,
          1,
          0,
          0,
            -1
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
    }
  ],
  "materials": [
    {
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
    }
  ],
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
    "children": [
      {
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
      }
    ]
  }
};

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
dali.internalSetupActorWrapper = function(handle) {
  dali.internalSetupProperties(handle);
  dali.internalAddActorMethods(handle);
  return handle;
}
  "use strict";

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
    dali.internalSetupActorWrapper(bareActorWrapper);
    return bareActorWrapper;
  };
  actor.findChildById = function(index) {
    var bareActorWrapper = this.__findChildById(index);
    dali.internalSetupActorWrapper(bareActorWrapper);
    return bareActorWrapper;
  };
  actor.findChildByName = function(name) {
    var bareActorWrapper = this.__findChildByName(name);
    if (bareActorWrapper.ok()) {
      dali.internalSetupActorWrapper(bareActorWrapper);
      return bareActorWrapper;
    } else {
      return null;
    }
  };
  actor.getChildAt = function(index) {
    var bareActorWrapper = this.__getChildAt(index);
    dali.internalSetupActorWrapper(bareActorWrapper);
    return bareActorWrapper;
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
          dali.internalSetupActorWrapper(actorWrapper);
          callback(actorWrapper);
        };
      })(callback)
    );
  };

  actor.setImageData = function(imageData) {
    // imageData as returned from canvas.getImageData
    var uint8array = new Uint8Array(imageData.data);
    actor.__setImageRawRGBA(imageData.width, imageData.height, uint8array);
  };

  actor.setImageDataRGB = function(imageData) {
    // imageData as returned from canvas.getImageData
    var uint8array = new Uint8Array(imageData.data);
    actor.__setImageRawRGB(imageData.width, imageData.height, uint8array);
  };

  actor.registerProperty = function(name, value) {
    var ret = -1;
    if(typeof(value) != "object") // just simple types, bool, string, float
    {
      var propertyValue = new dali.DaliPropertyValue({}, name, value);
      ret = actor.__registerProperty(name, propertyValue);
      propertyValue.delete(); // wrapper
      Object.defineProperty(this, name, {
        enumerable: true,
        configurable: false,
        get: function() {
          return dali.marshallGetProperty(this, name);
        },
        set: function(newValue) {
          dali.marshallSetProperty(this, name, newValue);
        }
      });
    }
    return ret;
  };

  actor.registerAnimatedProperty = function(name, propertyValue) {
    var ret = -1;
    if(typeof(value) != "object") // just simple types, bool, string, float
    {
      var propertyValue = new dali.DaliPropertyValue({}, name, value);
      ret = actor.__registerAnimatedProperty(name, propertyValue);
      propertyValue.delete(); // wrapper
      Object.defineProperty(this, name, {
        enumerable: true,
        configurable: false,
        get: function() {
          return dali.marshallGetProperty(this, name);
        },
        set: function(newValue) {
          dali.marshallSetProperty(this, name, newValue);
        }
      });
    }
    return ret;
  };

};

dali.createSolidColorActor = function(color, border, borderColor, borderSize) {
  var a = dali.__createSolidColorActor(color, border, borderColor, borderSize);
  dali.internalSetupActorWrapper(a);
  return a;
};


function ObjectLoader(fileObject) {
  // cached
  this.self = this;
  this.meshByUUID = {};
  this.geomByUUID = {};
  this.matByUUID = {};

  this.fileObject = fileObject;
}

function __longToArray(v) {
  return [((v>>24)&0xFF)/255.0,
          ((v>>16)&0xFF)/255.0,
          ((v>>8)&0xFF)/255.0,
          (v&0xFF)/255.0];
}

function __isBitSet(value, bit)
{
  return (value&(1<<bit));
}

ObjectLoader.prototype.__getMaterial = function(uuid) {
  if( !(uuid in this.matByUUID) ) {
    for(var i=0, len = this.fileObject["materials"].length; i<len;i++ ) {
      var f_mat = this.fileObject["materials"][i];
      skewer.log(i + ":" + f_mat["uuid"] + " " + ( f_mat["uuid"] === uuid ) );
      if( f_mat["uuid"] === uuid ) {
        assert(f_mat["type"] === "MeshPhongMaterial");
        var mat = new dali.MaterialWrapper(uuid);
        mat.setDiffuseColor(__longToArray(f_mat["color"]));
        mat.setAmbientColor(__longToArray(f_mat["ambient"]));
        mat.setSpecularColor(__longToArray(f_mat["specular"]));
        mat.setEmissiveColor(__longToArray(f_mat["emmissive"]));
        mat.setShininess(f_mat["shininess"]);
        this.matByUUID[uuid] = mat;
        break;
      }
    }
  }
  return this.matByUUID[uuid];
};

ObjectLoader.prototype.__getMeshData = function(uuid, uuid_material) {
  if( !(uuid in this.meshByUUID) ) {
    for(var i=0,len= this.fileObject["geometries"].length; i<len;i++) {
      var f_geom = this.fileObject["geometries"][i];
      if( f_geom["uuid"] === uuid ) {
        var f_indices, // file data
            f_posns,
            f_norms,
            f_uvs,
            f_faces,
            f_type;

        if("formatVersion" in f_geom["metadata"]) // then version 3.1
        {
          f_indices = f_geom["indices"];
          f_posns   = f_geom["vertices"];
          f_norms   = f_geom["normals"];
          f_uvs     = f_geom["uvs"];
          f_faces   = f_geom["faces"];
          if(f_faces) {
            f_type = f_geom["faces"][0]; // ["type"];
          }
        }
        else if(f_geom["type"] === "Geometry") // V4 clara io output? not standard???
        {
          f_indices = f_geom["data"]["indices"];
          f_posns   = f_geom["data"]["vertices"];
          f_norms   = f_geom["data"]["normals"];
          f_uvs     = f_geom["data"]["uvs"];
          f_faces   = f_geom["data"]["faces"];
          if(f_faces) {
            f_type = f_geom["data"]["faces"][0]; // ["type"];
          }
        }
        else if(f_geom["metadata"]["type"] === "Geometry") // V4
        {
          f_indices = f_geom["indices"];
          f_posns   = f_geom["vertices"];
          f_norms   = f_geom["normals"];
          f_uvs     = f_geom["uvs"];
          f_faces   = f_geom["faces"];
          if(f_faces) {
            f_type = f_geom["faces"][0]; // ["type"];
          }
        }
        else if(f_geom["metadata"]["type"] === "BufferGeometry") // V4
        {
          f_posns = f_geom["data"]["attributes"]["position"]["array"];
          f_norms = f_geom["data"]["attributes"]["norms"]["array"];
          f_uvs = f_geom["data"]["attributes"]["uv"]["array"];
        }

        var isQuad              = __isBitSet( type, 0 );
        assert(!isQuad, "Not supported");
        var hasMaterial         = __isBitSet( type, 1 );
        assert(!hasMaterial, "Not Supported");
        var hasFaceUv           = __isBitSet( type, 2 );
        var hasFaceVertexUv     = __isBitSet( type, 3 );
        var hasFaceNormal       = __isBitSet( type, 4 );
        var hasFaceVertexNormal = __isBitSet( type, 5 );
        var hasFaceColor        = __isBitSet( type, 6 );
        var hasFaceVertexColor  = __isBitSet( type, 7 );

        var mesh = new dali.MeshDataWrapper();
        var verts= new dali.VectorVertex();
        var vert = [];//new dali.Vertex();
        for(var i =0, len = f_posns.length/3; i < len; i++) {
          vert.push(f_posns[(i*3)+0]);
          vert.push(f_posns[(i*3)+1]);
          vert.push(f_posns[(i*3)+2]);
          if(f_norms) {
            vert.push(f_norms[(i*3)+0]);
            vert.push(f_norms[(i*3)+1]);
            vert.push(f_norms[(i*3)+2]);
          } else {
            vert.push(0);
            vert.push(0);
            vert.push(0);
          }

          if(f_uvs) {
            vert.push(f_uvs[(i*3)+0]);
            vert.push(f_uvs[(i*3)+1]);
          } else {
            vert.push(0);
            vert.push(0);
          }
          verts.push_back(vert);
        }

        var faces = null;
        if(f_faces) {
          faces = new dali.VectorFaceIndex();
          for(var i =1, len = f_faces.length; i < len; i++) {
            faces.push_back( f_faces[i] );
          }
        }

        if(f_indices) {
          faces = new dali.VectorFaceIndex();
          for(var i =1, len = f_indices.length; i < len; i++) {
            faces.push_back( f_indices[i] );
          }
        }

        if(!faces) {
          faces = [];
          for(var i=0, len = f_posns.length; i < len; i++) {
            faces.push(i);
          }
        }

        var material = this.__getMaterial(uuid_material);
        mesh.setLineData(verts, faces, material);
        this.meshByUUID[uuid] = mesh;
        verts.delete();
        faces.delete();
        break;
      } // if uuid found
    } // for geom in geometries
  }// if uid ! in meshByUUID

  return this.meshByUUID[uuid];
};

ObjectLoader.prototype.delete = function() {
  for(var a in this.meshByUUID) {
    a.delete();
  }
  this.meshByUUID ={};
  for(var b in this.matByUUID) {
    b.delete();
  }
  this.matByUUID = {};
};

ObjectLoader.prototype.createMeshActors = function() {
  var ret = [];
  if( "object" in this.fileObject ) {
    for( var i=0, len=this.fileObject["object"]["children"].length; i < len; i++) {
      var child = this.fileObject["children"];
      if( child["type"] === "Mesh" ) {
        var meshData = this.__getMeshData( child["geometry"],
                                           child["material"] );
        ret.push( dali.__createMeshActor(meshData) );
        meshData.delete();
      }
    }
  }

  var parent;

  if(ret) {
    parent = new dali.Actor();
    for(a in ret) {
      parent.add(a);
      a.delete();
    }
  }

  return parent;
};

dali.createMeshActor = function(threeDjs_formatV4) {

  var loader = new ObjectLoader(threeDjs_formatV4);

  return loader.createMeshActor();
};

dali.hitTest = function(x,y) {
  var a = dali.__hitTest(x,y);
  if( a.ok() ) {
    dali.internalSetupActorWrapper(a);
    return a;
  } else {
    return null;
  }
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

dali.debugFindFirstActor = function(actor, predicateFunction) {
  "use strict";
  for(var i = 0, len = actor.getChildCount(); i < len; i++) {
    var a = actor.getChildAt(i);
    var found = predicateFunction(a);
    if(found) {
      return a;
    }
    var child = dali.debugFindFirstActor(a, predicateFunction);
    if(child)
    {
      return child;
    }
    a.delete();
  }
  return null;
};

dali.debugDepthVisit = function(actor, operation, dontDelete) {
  "use strict";
  for(var i = 0, len = actor.getChildCount(); i < len; i++) {
    var a = actor.getChildAt(i);
    var done = operation(a);
    if(!done) {
      return false;
    }
    if(!dali.debugDepthVisit(a, operation, dontDelete))
    {
      return false;
    }
    var doit = true;
    if ( dontDelete !== undefined )
    {
      if( dontDelete )
      {
        doit = false;
      }
    }
    if(doit)
    {
      a.delete();
    }
  }
  return true;
};

dali.operationPrintProperty = function(property, all) {
  return (function(actor) {
    if(property in actor) {
      dali.log(actor.getId() + "property:" + actor[property]);
    } else {
     dali.log( actor.getId() + "property:n/a");
    }
    return all;
  });
};

dali.predicatePropertyEquals = function(property, value) {
  return (function(actor) {
    if(property in actor) {
      if(actor[property] === value) {
        return true;
      }
    }
    return false;
  });
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
    dali.internalSetupActorWrapper(wrapper);
    return wrapper;
  };

  this.getRenderTaskList = function() {
    var wrapper = this.stageWrapper.__getRenderTaskList();
    wrapper.createTask = function()
    {
      var t = this.__createTask();
      t.getCameraActor = function()
      {
        var a = this.__getCameraActor();
        if(a.ok())
        {
          dali.internalSetupActorWrapper(a);
        }
        return a;
      }
      return t;
    }
    wrapper.getTask = function(id)
    {
      var t = this.__getTask(id);
      t.getCameraActor = function()
      {
        var a = this.__getCameraActor();
        if(a.ok())
        {
          dali.internalSetupActorWrapper(a);
        }
        return a;
      }
      return t;
    }
    return wrapper;
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
