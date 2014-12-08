/*global  */
/* eslint-env browser */
/* eslint "brace-style": [2, "1tbs"] */
/* eslint "no-console": 0 */
/* eslint "no-underscore-dangle": 0 */

var $;
var ace;
var dali;
var canvas;
var app;
var eventHandler;
var assert;



var fileInput = document.getElementById("fileInput");
var fileInputCanvasDecorder = document.getElementById("fileInputCanvasDecorder");
var btnTextActorAdd = document.getElementById("btnTextActorAdd");
var textInput = document.getElementById("textInput");
var textShaderErrors = document.getElementById("textShaderErrors");
var btnShaderSourceC = document.getElementById("btnShaderSourceC");
var btnShaderSourceJS = document.getElementById("btnShaderSourceJS");
var btnShaderSourceJSON = document.getElementById("btnShaderSourceJSON");

//
// Global state
//
var actorIdToShaderSet = {};    // actor id to shader
var geometryType = "image";     // last set geometry type

var animationList = [];         // list of animations that have been added [ { animation: new dali.Animation(), properties: [] } ]
var animationSelectionIndex;    // selected animation

var lastActorSelectedId = -1;
var userEditingShader = true;
var modalSaveOldShader;

//
//
// Shaders
//
//
// precision mediump float;
// uniform float uTimeDelta;
// float rand(vec2 co){
//     return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
// }

// void main()
// {
// 	precision mediump float;
// 	vec2 uv = vTexCoord.xy ; // / vec2(sTextureRect.z, sTextureRect.w);
// 	float r = rand(uv*(uTimeDelta));
// 	vec4 VI = texture2D(sTexture,uv);
// 	VI*=r; 
// 	gl_FragColor = vec4(VI);
// }

//////////// high frequency noise
// float snoise(in vec2 co){
//     return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
// }

// void mainImage( out vec4 fragColor, in vec2 fragCoord )
// {
//     float n = snoise(vec2(fragCoord.x*cos(fragCoord.x),fragCoord.y*sin(fragCoord.y))); 
// 	fragColor = vec4(n, n, n, 1.0 );
// }

//////////
// blur but needs mipmapping
// precision mediump float;

// uniform float factor;

// float perspective = 0.3;

// void main()
// {
// 	vec2 p = vTexCoord.xy; // / iResolution.xy;
// 	float focus = 0.5; // sin(factor*2.)*.35+.5;
// 	float blur = 7.*sqrt(abs(p.y - focus));
	
// 	/* perpective version */	
// 	//vec2 p2 = vec2(p.x-(1.-p.y)*perspective*(p.x*2. - 1.), -p.y);
	
// 	/* simple vesion */
// 	vec2 p2 = -p;	
	
// 	gl_FragColor = vec4(blur,blur,blur,1.0); //texture2D(sTexture, p2, blur);
// }


var shaderSourceSelection = 0;
var shaderSources = [
  {
    name: "pass through",
    geometryType: "image",
    geometryHints: "",
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
    name: "sRGB Correction",
    geometryType: "image",
    geometryHints: "",
    vertex: "void main(void)\n" +
      "{\n" +
      "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n",
    fragment: "precision mediump float;\n" +
      "void main()\n" +
      "{\n" +
      "vec4 col = texture2D( sTexture, vTexCoord ) * uColor;\n" +
      "vec3 mask = vec3(greaterThan(col.rgb, vec3(0.0031308)));\n" +
      "vec3 o = mix(col.rgb * 12.92, \n" +
      "             pow(col.rgb, vec3(1.0/2.4)) * 1.055 - 0.055, \n" +
      "             mask);\n" +
      "gl_FragColor = vec4(o.r, o.g, o.b, col.a);\n" +
      "}\n"
  },
  {
    name: "adjust brightness",
    geometryType: "image",
    geometryHints: "",
    animateTo: [ ["uGain", 1.0, "Linear", 0, 3],
                        ["uOffset", 0.2, "Linear", 0, 3] ],
    vertex: "void main(void)\n" +
      "{\n" +
      "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n",
    fragment: "precision mediump float;\n" +
      "uniform float uGain;\n" +
      "uniform float uOffset;\n" +
      "void main()\n" +
      "{\n" +
      "    vec4 t = texture2D( sTexture, vTexCoord );\n" +
      "    \n" +
      "    float y = 0.0 + (0.299 *t.r) + 0.587*t.g + 0.114*t.b;\n" +
      "    float cb= 128.0-(0.168736*t.r)-0.331264*t.g+0.5*t.b;\n" +
      "    float cr= 128.0+(0.5*t.r)-(0.418688*t.g)-0.081312*t.b;\n" +
      "    \n" +
      "    y = (y*uGain) + uOffset;\n" +
      "    \n" +
      "    vec4 col = vec4(t.a);\n" +
      "    \n" +
      "    col.r = y + 1.402*(cr-128.0);\n" +
      "    col.g = y - 0.34414*(cb-128.0)-0.71414*(cr-128.0);\n" +
      "    col.b = y + 1.722*(cb-128.0);\n" +
      "    \n" +
      "    \n" +
      "    gl_FragColor = col * uColor;\n" +
      "}\n"
  },
  {
    name: "wavey",
    geometryType: "image",
    geometryHints: "",
    animateTo: [ ["uAmplitude", 0.2, "Linear", 0, 3],
                        ["uFrequency", 4, "Linear", 0, 3] ],
    vertex: "void main(void)\n" +
      "{\n" +
      "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n",
    fragment: "precision mediump float;\n" +
      "uniform float uAmplitude;  //  amplitude in x ;0.4\n" +
      "uniform float uFrequency;  // frequence in y; 4\n" +
      "void main()\n" +
      "{\n" +
      "  vec2 uv = vTexCoord.xy;\n" +
      "\n" +
      "  uv.x += sin(uv.y * 3.14 * uFrequency) * uAmplitude;\n" +
      "  \n" +
      "  vec4 color = texture2D(sTexture, uv);\n" +
      "  \n" +
      "  gl_FragColor = color;\n" +
      "}\n"
  },
  {
    name: "melt",
    geometryType: "image",
    geometryHints: "",
    animateTo: [ ["uFactor", -4.0, "Linear", 0, 3] ],
    vertex: "void main(void)\n" +
      "{\n" +
      "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n",
    fragment:
    "precision mediump float;\n" +
      "uniform float uFactor; // drip factor\n" +
      "\n" +
      "    \n" +
      "float random( vec2 p )\n" +
      "{\n" +
      "    float q = p.x * 269.5 + p.y * 183.3;\n" +
      "  return fract( sin( q ) * 43758.5453 );\n" +
      "}\n" +
      "\n" +
      "void main()\n" +
      "{\n" +
      "  vec2 uv = vTexCoord.xy;\n" +
      "  \n" +
      "  float kindaRandom = (texture2D(sTexture, vec2(uv.x,0.5)).r + texture2D(sTexture, vec2(0.5,uv.x)).r) / 2.0;\n" +
      "  \n" +
      "  //kindaRandom = random( vec2(texture2D(sTexture, vec2(uv.x,0.5)).r, texture2D(sTexture, vec2(0.5,uv.y)).g) );\n" +
      "  \n" +
      "  uv.y +=  uFactor * kindaRandom;\n" +
      "  \n" +
      "\n" +
      "  gl_FragColor = texture2D( sTexture, uv ) * uColor;\n" +
      "}\n" +
      "\n"
  },
  {
    name: "round window",
    geometryType: "image",
    geometryHints: "",
    animateTo: [ ["uRadius", 0.8, "Linear", 0, 3] ],
    vertex: "void main(void)\n" +
      "{\n" +
      "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n",
    fragment: "precision lowp float;\n" +
      "uniform lowp float uRadius;\n" +
      "void main()\n" +
      "{\n" +
      "  precision lowp float;\n" +
      "  lowp vec2 pos= vec2(vTexCoord.x-0.5,vTexCoord.y-0.5);\n" +
      "  lowp float radius = dot(pos, pos ) ;\n" +
      "  if( radius > (uRadius*uRadius) )\n" +
      "    discard;\n" +
      "  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor ;\n" +
      "}\n"
  },
  {
    name: "mosaic",
    geometryType: "image",
    geometryHints: "",
    animateTo: [ ["uPixelFactor", 0.3, "Linear", 0, 3] ],
    vertex: "void main(void)\n" +
      "{\n" +
      "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n",
    fragment: "precision mediump float;\n" +
      "  \n" +
      "uniform float uPixelFactor; //  0.0< uPixelFactor < ~0.3 \n" +
      "\n" +
      "\n" +
      "float smooth(float f) {\n" +
      "  return 32.0*f*f*(0.25*f*f-0.5*f+0.25)+0.5;\n" +
      "}\n" +
      "\n" +
      "void main()\n" +
      "{\n" +
      "  vec2 resolution = vec2(1,1); // uniform vec2 resolution;\n" +
      "  //uPixelFactor = 8.0 + 8.0 * (0.5 + 0.5 * sin(globaltime * 0.25));\n" +
      "  vec2 chunkCoord = floor(vTexCoord.xy / uPixelFactor) * uPixelFactor;\n" +
      "  vec2 locCoord = (vTexCoord.xy - chunkCoord) / uPixelFactor;\n" +
      "  vec4 color = vec4(floor(5.0 * texture2D(sTexture, chunkCoord / resolution.xy).xyz) / 5.0, 1.0);\n" +
      "  float grey = (color.x + color.y + color.z) / 3.0;\n" +
      "  gl_FragColor = color * smooth(locCoord.x) * smooth(locCoord.y);\n" +
      "}\n"
  },
  {name: "burn",
   geometryType: "image",
   geometryHints: "",
   animateTo: [ ["uThresh", 0.8, "Linear", 0, 3] ],
   vertex:
   "void main(void)\n" +
   "{\n" +
   "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
   "  vTexCoord = aTexCoord;\n" +
   "}\n" +
   "\n" +
   "\n" +
   "",
   fragment:
   "precision mediump float;\n" +
   "\n" +
   "float uScale = 2.0;  // noise texture scaling; 10.0\n" +
   "uniform float uThresh;  // threshold of noise for burn; 1.1\n" +
   "\n" +
   "float random( vec2 p )\n" +
   "{\n" +
   "    float q = p.x * 269.5 + p.y * 183.3;\n" +
   "  return fract( sin( q ) * 43758.5453 );\n" +
   "}\n" +
   "\n" +
   "float noise( vec2 point )\n" +
   "{\n" +
   "  vec2 p = floor( point );\n" +
   "  vec2 f = fract( point );\n" +
   "  return mix(\n" +
   "    mix( random( p + vec2( 0.0, 0.0 ) ), random( p + vec2( 1.0, 0.0 ) ), f.x ),\n" +
   "    mix( random( p + vec2( 0.0, 1.0 ) ), random( p + vec2( 1.0, 1.0 ) ), f.x ),\n" +
   "    f.y\n" +
   "  );\n" +
   "}\n" +
   "\n" +
   "float fractal( vec2 point )\n" +
   "{\n" +
   "    float sum = 0.0;\n" +
   "    float scale = 0.5;\n" +
   "    for ( int i = 0; i < 5; i++ )\n" +
   "  {\n" +
   "    sum += noise( point ) * scale;\n" +
   "    point *= 2.0;\n" +
   "        scale /= 2.0;\n" +
   "  }\n" +
   "    \n" +
   "  return sum;\n" +
   "}\n" +
   "\n" +
   "\n" +
   "void main( )\n" +
   "{\n" +
   "  vec2 point = vTexCoord.xy / uScale;\n" +
   "    //point.x += iGlobalTime * 0.1;\n" +
   "    float noise    = fractal( point * 20.0 );\n" +
   "    \n" +
   "    vec2 uv = vTexCoord.xy; // iResolution.xy;\n" +
   "    //uv.y = 1.0-uv.y;\n" +
   "    gl_FragColor = texture2D(sTexture, uv);\n" +
   "    \n" +
   "    if(noise < uThresh)\n" +
   "    {\n" +
   "        gl_FragColor = vec4(1.0, 0.5, 0.0, 1.0);\n" +
   "    }\n" +
   "    if(noise < uThresh - 0.05)\n" +
   "    {\n" +
   "        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n" +
   "    }\n" +
   "    if(noise < uThresh - 0.1)\n" +
   "    {\n" +
   "    \n" +
   "        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0); // use discard?\n" +
   "    }\n" +
   "}\n" +
   "\n"
  },
  {
    name: "ripple 2D",
    geometryType: "image",
    geometryHints: "",
    animateTo: [ ["uAmplitude", 0.1, "Linear", 0, 3],
                        ["uTime", 6, "Linear", 0, 3] ],
    vertex:
    "void main(void)\n" +
      "{\n" +
      "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n" +
      "\n" +
      "",
    fragment:
    "precision mediump float;\n" +
      "uniform float uAmplitude; // 0.02; (< 1)\n" +
      "uniform float uTime;\n" +
      "void main()\n" +
      "{\n" +
      "  highp vec2 textureSize = sTextureRect.zw - sTextureRect.xy;\n" +
      "  highp vec2 pos = -1.0 + 2.0 * vTexCoord.st/textureSize;\n" +
      "  highp float len = length(pos);\n" +
      "  highp vec2 texCoord = vTexCoord.st/textureSize + pos/len * sin( len * 12.0 - uTime * 4.0 ) * uAmplitude; \n" +
      "  gl_FragColor = texture2D(sTexture, texCoord) * uColor;\n" +
      "}\n" +
      "\n" +
      "\n" +
      ""
  },
  {
    name: "emboss combine",
    geometryType: "image",
    geometryHints: "",
    animateTo: [ ["uWeight", [10.0, 10.0, 10.0, 10.0], "Linear", 0, 3] ],
    vertex: "void main(void)\n" +
      "{\n" +
      "gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
      "vTexCoord = aTexCoord;\n" +
      "}\n",
    fragment: "precision mediump float;\n" +
      "const int KERNEL_SIZE = 9;\n" +
      "\n" +
      "uniform vec4 uWeight;\n" +
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
      "  // kernel[0] = 1.0/16.0;  kernel[1] = 2.0/16.0;  kernel[2] = 1.0/16.0;\n" +
      "  // kernel[3] = 2.0/16.0;  kernel[4] = 4.0/16.0;  kernel[5] = 2.0/16.0;\n" +
      "  // kernel[6] = 1.0/16.0;  kernel[7] = 2.0/16.0;  kernel[8] = 1.0/16.0;\n" +
      "\n" +
      "  // laplace\n" +
      "  // kernel[0] = 0.0;  kernel[1] = 1.0;  kernel[2] = 0.0;\n" +
      "  // kernel[3] = 1.0;  kernel[4] = -4.0; kernel[5] = 1.0;\n" +
      "  // kernel[6] = 0.0;  kernel[7] = 1.0;  kernel[8] = 0.0;\n" +
      "\n" +
      "  // edge\n" +
      "  // kernel[0] = -1.0;  kernel[1] = -1.0; kernel[2] = -1.0;\n" +
      "  // kernel[3] = -1.0;  kernel[4] = +9.0; kernel[5] = -1.0;\n" +
      "  // kernel[6] = -1.0;  kernel[7] = -1.0; kernel[8] = -1.0;\n" +
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
      "    sum = texture2D(sTexture, vTexCoord.xy) + (sum * uWeight);\n" +
      "\n" +
      "  gl_FragColor = sum;\n" +
      "}\n"
  },
  {
    name: "blur",
    geometryType: "image",
    geometryHints: "",
    vertex: "void main(void)\n" +
      "{\n" +
      "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n" +
      "\n" +
      "",
    fragment:
    "\n" +
      "precision mediump float;\n" +
      "\n" +
      "#define PI2 6.283184\n" +
      "\n" +
      "#define CV 0.1\n" +
      "#define ST 0.05\n" +
      "\n" +
      "uniform float uFactor; // 0.0 - 1.0\n" +
      "\n" +
      "vec4 colorat(vec2 uv) {\n" +
      "  return texture2D(sTexture, uv);\n" +
      "}\n" +
      "vec4 blur(vec2 uv) { // convolve\n" +
      "  vec4 col = vec4(0.0);\n" +
      "  for(float r0 = 0.0; r0 < 1.0; r0 += ST) {\n" +
      "    float r = r0 * CV;\n" +
      "    for(float a0 = 0.0; a0 < 1.0; a0 += ST) {\n" +
      "      float a = a0 * PI2;\n" +
      "      col += colorat(uv + vec2(cos(a), sin(a)) * r);\n" +
      "    }\n" +
      "  }\n" +
      "  col *= ST * ST;\n" +
      "  return col;\n" +
      "}\n" +
      "\n" +
      "\n" +
      "\n" +
      "void main() {\n" +
      "  vec2 uv = vTexCoord.xy;\n" +
      "  gl_FragColor = blur(uv) * uFactor + ((1.0-uFactor) * texture2D(sTexture, uv));\n" +
      "}\n" +
      "\n"
  },
  {name: "image-click",
   geometryType: "image",
   geometryHints: "",
   animateTo: [ ["uRadius", 0.3, "Linear", 0, 0.3] ],
   vertex:
   "void main(void)\n" +
   "{\n" +
   "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
   "  vTexCoord = aTexCoord;\n" +
   "}\n" +
   "\n" +
   "",
   fragment:
   "precision lowp float;\n" +
   "uniform lowp float uRadius; // 0 -> 0.3 touch radius and brightness mix\n" +
   "vec2 offset = vec2(0.3,-0.2); // offset from center; for touch point (make uniform)\n" +
   "\n" +
   "void main()\n" +
   "{\n" +
   "  precision lowp float;\n" +
   "  lowp vec2 pos= vec2(vTexCoord.x-0.5-offset.x,vTexCoord.y-0.5-offset.y);\n" +
   "  lowp float radius = dot(pos, pos ) ;\n" +
   "\n" +
   " //  use sRGB correction to brighten image\n" +
   "    vec4 col = texture2D( sTexture, vTexCoord ) * uColor;\n" +
   "    vec3 mask = vec3(greaterThan(col.rgb, vec3(0.0031308)));\n" +
   "    vec3 o = mix(col.rgb * 12.92, \n" +
   "             pow(col.rgb, vec3(1.0/2.4)) * 1.055 - 0.055, \n" +
   "             mask);\n" +
   "    \n" +
   "    vec3 diff = o - col.rgb;\n" +
   "    diff *= uRadius * 3.0;\n" +
   "    o = col.rgb + diff;\n" +
   "    \n" +
   "    gl_FragColor = vec4(o.r, o.g, o.b, col.a);\n" +
   "\n" +
   "  if( radius <= (uRadius*uRadius) )\n" +
   "  {\n" +
   "    gl_FragColor += vec4(0.1);\n" +
   "  }\n" +
   "  \n" +
   "}\n" +
   "\n" +
   ""
  },
  {name: "iris effect",
   geometryType: "image",
   geometryHints: "",
   animateTo: [ ["uRadius", 1.0, "Linear", 0, 0.3],
                ["uCenter", [0.2, 0.2], "Linear", 0, 0.0],
                ["uBlendFactor", 1.0, "Linear", 0, 0.0]
              ],
   vertex:
   "uniform mediump vec2 uCenter;\n" +
   "varying mediump vec2 vRelativePosition;\n" +
   "\n" +
   "void main()\n" +
   "{\n" +
   "    mediump vec4 world = uModelView * vec4(aPosition,1.0);\n" +
   "    gl_Position = uProjection * world;\n" +
   "\n" +
   "    vTexCoord = aTexCoord;\n" +
   "    vRelativePosition = aTexCoord - uCenter;\n" +
   "}\n" +
   "\n" +
   "",
   fragment:
   "\n" +
   "\n" +
   "uniform mediump float uRadius;\n" +
   "uniform mediump float uBlendFactor;\n" +
   "varying mediump vec2 vRelativePosition;\n" +
   "void main()\n" +
   "{\n" +
   "   mediump float delta = (length(vRelativePosition) - uRadius);\n" +
   "   delta = clamp(0.0 - delta * uBlendFactor, 0.0, 1.0);\n" +
   "   gl_FragColor = texture2D(sTexture, vTexCoord) * uColor;\n" +
   "   gl_FragColor.a *= delta;\n" +
   "}\n" +
   ""
  },
  {name: "mirror effect",
   geometryType: "image",
   geometryHints: "",
   animateTo: [ ["uDepth", 0.5, "Linear", 0, 0.0],
                ["uAlpha", 1.0, "Linear", 0, 0.0]
              ],
   vertex:
   "void main()\n" +
   "{\n" +
   "  mediump vec3 pos = aPosition;\n" +
   "  pos.y = pos.y * 3.0;\n" +
   "  mediump vec4 world = uModelView * vec4(pos,1.0);\n" +
   "  gl_Position = uProjection * world;\n" +
   "  vTexCoord = aTexCoord;\n" +
   "}\n" +
   "",
   fragment:
   "uniform  mediump float  uDepth;\n" +
   "uniform  mediump float  uAlpha;\n" +
   "void main()\n" +
   "{\n" +
   " if(vTexCoord.y < 1.0 / 3.0)\n" +
   " {\n" +
   "   gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);\n" +
   " }\n" +
   " else if(vTexCoord.y < 2.0 / 3.0)\n" +
   " {\n" +
   "   gl_FragColor = texture2D(sTexture, vec2(vTexCoord.x, vTexCoord.y * 3.0 - 1.0)) * uColor;\n" +
   "   gl_FragColor.a *= uAlpha;\n" +
   " }\n" +
   " else\n" +
   " {\n" +
   "   highp float darkness = 3.0 - vTexCoord.y * 3.0;\n" +
   "   darkness = (1.0 - 1.0 / uDepth + darkness * 1.0/ uDepth) * 0.65;\n" +
   "   highp vec4 color = texture2D(sTexture, vec2(vTexCoord.x, -vTexCoord.y *3.0 + 3.0)) * uColor;\n" +
   "   color.a *= uAlpha;\n" +
   "   gl_FragColor = color * vec4(darkness, darkness, darkness, darkness);\n" +
   " }\n" +
   "}\n" +
   ""
  },
  {name: "sheer",
   geometryType: "image",
   geometryHints: "grid depthbuffer",
   animateTo: [ ["uAngleXAxis", 15.0, "Linear", 0, 3.0],
                ["uAngleYAxis", 25.0, "Linear", 0, 3.0]
              ],
   vertex:
   "uniform mediump  vec2  uCenter;\n" +
   "uniform mediump  float uAngleXAxis;\n" +
   "uniform mediump  float uAngleYAxis;\n" +
   "void main()\n" +
   "{\n" +
   "  mediump vec4 world = uModelView * vec4(aPosition,1.0);\n" +
   "\n" +
   "  world.x = world.x + tan(radians(uAngleXAxis)) * (world.y - uCenter.y * world.w);\n" +
   "  world.y = world.y + tan(radians(uAngleYAxis)) * (world.x - uCenter.x * world.w);\n" +
   "\n" +
   "  gl_Position = uProjection * world;\n" +
   "\n" +
   "  vTexCoord = aTexCoord;\n" +
   "}\n" +
   "",
   fragment:
   ""
  },
  {name: "square dissolve",
   animateTo: [ ["uRows", 0.4, "Linear", 0, 0.0],
                ["uColumns", 0.4, "Linear", 0, 0.0],
                ["texSize", [100, 100], "Linear", 0, 0.0],
                ["uStep", 1, "Linear", 0, 3.0]
              ],
   geometryType: "image",
   geometryHints: " grid blending",
   vertex:
   "void main(void)\n" +
   "{\n" +
   "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
   "  vTexCoord = aTexCoord;\n" +
   "}\n" +
   "\n" +
   "",
   fragment:
   "uniform  mediump vec2   texSize;\n" +
   "uniform  mediump float  uStep;\n" +
   "uniform  mediump float  uRows;\n" +
   "uniform  mediump float  uColumns;\n" +
   "void main()\n" +
   "{\n" +
   "  mediump vec2 mosaicSize = vec2(1.0 / uRows, 1.0 / uColumns);\n" +
   "  mediump vec2 intXY = vec2(vTexCoord.x * texSize.x, vTexCoord.y * texSize.y);\n" +
   "  mediump vec2 XYMosaic = vec2(floor(intXY.x / mosaicSize.x) * mosaicSize.x, floor(intXY.y / mosaicSize.y) * mosaicSize.y);\n" +
   "  mediump vec2 UVMosaic = vec2(XYMosaic.x /texSize.x, XYMosaic.y / texSize.y);\n" +
   "  mediump vec4 noiseVec = texture2D(sEffect, UVMosaic);\n" +
   "  mediump float intensity = (noiseVec[0] + noiseVec[1] + noiseVec[2] + noiseVec[3]) / 4.0;\n" +
   "  if(intensity < uStep)\n" +
   "    gl_FragColor = vec4(0.1, 0.1, 0.1, 1.0);\n" +
   "  else\n" +
   "    gl_FragColor = texture2D(sTexture, vTexCoord);\n" +
   "  gl_FragColor *= uColor;\n" +
   "}\n" +
   "\n" +
   ""
  },
  {name: "swirl",
   geometryType: "image",
   geometryHints: " grid blending",
   animateTo: [ ["uCenter", [0.5, 0.5], "Linear", 0, 0.0],
                ["uTextureSize", [100, 100], "Linear", 0, 0.0],
                ["uRadius", 1.0, "Linear", 0, 3.0],
                ["uAngle", 3.0, "Linear", 0, 3.0]
              ],
   vertex:
   "void main(void)\n" +
   "{\n" +
   "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
   "  vTexCoord = aTexCoord;\n" +
   "}\n" +
   "\n" +
   "",
   fragment:
   "uniform mediump vec2  uTextureSize;\n" +
   "uniform highp float uRadius;\n" +
   "uniform highp float uAngle;\n" +
   "uniform mediump vec2  uCenter;\n" +
   "void main()\n" +
   "{\n" +
   "  highp vec2 textureCenter = (sTextureRect.xy + sTextureRect.zw) * 0.5;\n" +
   "  textureCenter = vTexCoord.st - textureCenter;\n" +
   "  highp float distance = length(textureCenter);\n" +
   "  if (distance >= uRadius)\n" +
   "    discard;\n" +
   "  highp float percent = (uRadius - distance) / uRadius;\n" +
   "  highp float theta = percent * percent * uAngle * 4.0;\n" +
   "  highp float sinTheta = sin(theta);\n" +
   "  highp float cosTheta = cos(theta);\n" +
   "// if warp, loose the sign from sin\n" +
   "  bool warp = true;\n" +
   "  if( warp )\n" +
   "  {\n" +
   "    textureCenter = vec2( dot( textureCenter, vec2(cosTheta, sinTheta) ),\n" +
   "                          dot( textureCenter, vec2(sinTheta, cosTheta) ) );\n" +
   "  }\n" +
   "  else\n" +
   "  {\n" +
   "    textureCenter = vec2( dot( textureCenter, vec2(cosTheta, -sinTheta) ),\n" +
   "                        dot( textureCenter, vec2(sinTheta, cosTheta) ) );\n" +
   "  }\n" +
   "  textureCenter += uCenter;\n" +
   "  gl_FragColor = texture2D( sTexture, textureCenter ) * uColor;\n" +
   "}\n" +
   "\n" +
   "\n" +
   ""
  },
  {name: "carousel (broken?)",
   geometryType: "image",
   geometryHints: " grid depthbuffer",
   vertex:
   "\n" +
   "uniform float uRadius;\n" +
   "uniform mediump vec2 uCenter;\n" +
   "uniform mediump vec2 uAnglePerUnit;\n" +
   "\n" +
   "void main()\n" +
   "{\n" +
   "    mediump vec4 world = uModelView * vec4(aPosition,1.0);\n" +
   "    mediump vec2 d = (world.xy - uCenter) * uAnglePerUnit;\n" +
   "    mediump float a = length(d);\n" +
   "    mediump float cs = cos(radians(a));\n" +
   "    world.z -= cs * uRadius;\n" +
   "    gl_Position = uProjection * world;\n" +
   "    \n" +
   "    vTexCoord = aTexCoord; \n" +
   "}\n" +
   "\n" +
   "\n" +
   "",
   fragment:
   "precision mediump float;\n" +
   "void main()\n" +
   "{\n" +
   "  gl_FragColor = texture2D( sTexture, vTexCoord ) * uColor;\n" +
   "}\n" +
   "\n" +
   ""
  },
  {
    name: "twist (broken)",
    geometryType: "image",
    geometryHints: "",
    vertex: "varying lowp vec4  vcolor;\n" +
      "uniform lowp float uOffset;\n" +
      "\n" +
      "void main()\n" +
      "{\n" +
      "    vec4 pos = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
      "    pos.x= 3.0*pos.z*(sin(1.57+uOffset+pos.y/1000.0));\n" +
      "    gl_Position =pos;\n" +
      "    vcolor = pos/500.0;\n" +
      "    vTexCoord = aTexCoord;\n" +
      "}\n",
    fragment: "varying lowp vec4 vcolor;\n" +
      "void main()\n" +
      "{\n" +
      "    gl_FragColor = texture2D( sTexture, vTexCoord ) *uColor + vcolor*0.2;\n" +
      "}\n"
  },
  {
    name: "refraction example (broken - needs a mesh actor)",
    geometryType: "texturedMesh",
    geometryHints: "",
    vertex: "varying mediump vec2 vTextureOffset;\n" +
      "void main()\n" +
      "{\n" +
      "  gl_Position = uMvpMatrix * vec4( aPosition.xy, 0.0, 1.0 );\n" +
      "  vTexCoord = aTexCoord.xy;\n" +
      "\n" +
      "  vNormal = aNormal;\n" +
      "  vVertex = vec4( aPosition, 1.0 );\n" +
      "  float length = max(0.01, length(aNormal.xy)) * 40.0;\n" +
      "  vTextureOffset = vec2(aNormal.xy / length);\n" +
      "}\n",
    fragment: "precision mediump float;\n" +
      "uniform mediump float uEffectStrength;\n" +
      "uniform mediump vec3 uLightPosition;\n" +
      "uniform mediump vec2 uLightXYOffset;\n" +
      "uniform mediump vec2 uLightSpinOffset;\n" +
      "uniform mediump float uLightIntensity;\n" +
      "varying mediump vec2 vTextureOffset;\n" +
      "\n" +
      "vec3 rgb2hsl(vec3 rgb)\n" +
      "{\n" +
      "  float epsilon = 1.0e-10;\n" +
      "  vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);\n" +
      "  vec4 P = mix(vec4(rgb.bg, K.wz), vec4(rgb.gb, K.xy), step(rgb.b, rgb.g));\n" +
      "  vec4 Q = mix(vec4(P.xyw, rgb.r), vec4(rgb.r, P.yzx), step(P.x, rgb.r));\n" +
      "  // RGB -> HCV\n" +
      "  float value = Q.x;\n" +
      "  float chroma = Q.x - min(Q.w, Q.y);\n" +
      "  float hue = abs(Q.z + (Q.w-Q.y) / (6.0*chroma+epsilon));\n" +
      " // HCV -> HSL\n" +
      "float lightness = value - chroma*0.5;\n" +
      "  return vec3( hue, chroma/max( 1.0-abs(lightness*2.0-1.0), 1.0e-1 ), lightness );\n" +
      "}\n" +

      "vec3 hsl2rgb( vec3 hsl )\n" +
      "{\n" +
      "  // pure hue->RGB\n" +
      "  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);\n" +
      "  vec3 p = abs(fract(hsl.xxx + K.xyz) * 6.0 - K.www);\n" +
      "  vec3 RGB = clamp(p - K.xxx, 0.0, 1.0);\n" +
      "  float chroma = ( 1.0 - abs( hsl.z*2.0-1.0 ) ) * hsl.y;\n" +
      "  return ( RGB - 0.5 ) * chroma + hsl.z;\n" +
      "}\n" +

      "void main()\n" +
      "{\n" +
      "  vec3 normal = normalize( vNormal);\n" +
      "\n" +
      "  vec3 lightPosition = uLightPosition + vec3(uLightXYOffset+uLightSpinOffset, 0.0);\n" +
      "  mediump vec3 vecToLight = normalize( (lightPosition - vVertex.xyz) * 0.01 );\n" +
      "  mediump float spotEffect = pow( max(0.05, vecToLight.z ) - 0.05, 8.0);\n" +
      "\n" +
      "  spotEffect = spotEffect * uEffectStrength;\n" +
      "  mediump float lightDiffuse = ( ( dot( vecToLight, normal )-0.75 ) *uLightIntensity  ) * spotEffect;\n" +
      "\n" +
      "  lowp vec4 color = texture2D( sTexture, vTexCoord + vTextureOffset * spotEffect );\n" +
      "  vec3 lightedColor =  hsl2rgb( rgb2hsl(color.rgb) + vec3(0.0,0.0,lightDiffuse) );\n" +
      "\n" +
      "  gl_FragColor = vec4( lightedColor, color.a ) * uColor;\n" +
      "  }"
  },
  {
    name: "bouncing effect (broken ? )",
    geometryType: "image",
    geometryHints: "",
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
    name: "ripple (broken)",
    geometryType: "image",
    geometryHints: " grid",
    vertex: "precision mediump float;\n" +
      "uniform mediump   vec2  ucenter;\n" +
      "uniform mediump   float uTime;\n" +
      "uniform mediump   float uAmplitude;\n" +
      "//uniform mediump   float uLighting;\n" +
      "//uniform mediump   float uwaveLength;\n" +
      "varying mediump   float vLight;\n" +
      "varying mediump   float vShade;\n" +
      "void main()\n" +
      "{\n" +
      "  float lighting = uAmplitude * 0.02;\n" +
      "  float waveLength = uAmplitude * 0.0016;\n" +
      "  vec4 world = uModelView * vec4(aPosition,1.0);\n" +
      "  vec2 d = vec2(world.x - ucenter.x, world.y - ucenter.y);\n" +
      "  float dist = length(d);\n" +
      "  float amplitude = cos(uTime - dist*waveLength);\n" +
      "  float slope     = sin(uTime - dist*waveLength);\n" +
      "  world.z += amplitude * uAmplitude;\n" +
      "  gl_Position = uProjection * world;\n" +
      "  vec2 lightDirection = vec2(-0.707,0.707);\n" +
      "  float dd = 0.0;\n" +
      "  if(dist > 0.0)\n" +
      "  {\n" +
      "    dd = dot(normalize(d),lightDirection) * lighting;\n" +
      "  }\n" +
      "  vShade = 1.0 - (dd * slope);\n" +
      "  vLight = max(0.0, dd * -slope);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n" +
      "\n" +
      "",
    fragment:
    "\n" +
      "precision mediump float;\n" +
      "varying mediump float  vLight;\n" +
      "varying mediump float  vShade;\n" +
      "void main()\n" +
      "{\n" +
      "  gl_FragColor = texture2D(sTexture, vTexCoord) * uColor * vec4(vShade,vShade,vShade,1.0) + vec4(vLight, vLight, vLight,0.0);\n" +
      "}\n" +
      "\n" +
      "\n" +
      ""
  },
  {
    name: "noise (broken)",
    geometryType: "image",
    geometryHints: "",
    vertex: "void main(void)\n" +
      "{\n" +
      "  gl_Position = uProjection * uModelView * vec4(aPosition, 1.0);\n" +
      "  vTexCoord = aTexCoord;\n" +
      "}\n",
    fragment: "precision mediump float;\n" +
      "\n" +
      "// noise\n" +
      "float noise(vec2 pos)\n" +
      "{\n" +
      " return fract( sin( dot(pos*0.001 ,vec2(24.12357, 36.789) ) ) * 12345.123);\n" +
      "}\n" +
      "\n" +
      "\n" +
      "// blur noise\n" +
      "float smooth_noise(vec2 pos)\n" +
      "{\n" +
      " return   ( noise(pos + vec2(1,1)) + noise(pos + vec2(1,1)) + noise(pos + vec2(1,1)) + noise(pos + vec2(1,1)) ) / 16.0\n" +
      "     + ( noise(pos + vec2(1,0)) + noise(pos + vec2(-1,0)) + noise(pos + vec2(0,1)) + noise(pos + vec2(0,-1)) ) / 8.0\n" +
      "       + noise(pos) / 4.0;\n" +
      "}\n" +
      "\n" +
      "\n" +
      "// linear interpolation\n" +
      "float interpolate_noise(vec2 pos)\n" +
      "{\n" +
      "float a, b, c, d;\n" +
      " \n" +
      " a = smooth_noise(floor(pos));\n" +
      " b = smooth_noise(vec2(floor(pos.x+1.0), floor(pos.y)));\n" +
      " c = smooth_noise(vec2(floor(pos.x), floor(pos.y+1.0)));\n" +
      " d = smooth_noise(vec2(floor(pos.x+1.0), floor(pos.y+1.0)));\n" +
      "\n" +
      " a = mix(a, b, fract(pos.x));\n" +
      " b = mix(c, d, fract(pos.x));\n" +
      " a = mix(a, b, fract(pos.y));\n" +
      " \n" +
      " return a;\n" +
      "}\n" +
      "\n" +
      "\n" +
      "float perlin_noise(vec2 pos)\n" +
      "{\n" +
      " float n;\n" +
      " \n" +
      " n = interpolate_noise(pos*0.0625)*0.5;\n" +
      " n += interpolate_noise(pos*0.125)*0.25;\n" +
      " n += interpolate_noise(pos*0.025)*0.225;\n" +
      " n += interpolate_noise(pos*0.05)*0.0625;\n" +
      " n += interpolate_noise(pos)*0.03125;\n" +
      " return n;\n" +
      "}\n" +
      "\n" +
      "\n" +
      "\n" +
      "void main()\n" +
      "{\n" +
      " vec2 pos = vTexCoord.xy;\n" +
      " float c, n;\n" +
      "\n" +
      "\n" +
      " n = perlin_noise(pos);\n" +
      "\n" +
      "\n" +
      " vec2 p = vTexCoord.xy; // / iResolution.xy;\n" +
      "\n" +
      " if(p.y < 0.333) // last row\n" +
      "{\n" +
      "  \n" +
      "  if(p.x < 0.333)\n" +
      "    c = abs(cos(n*10.0));\n" +
      "  else if(p.x < 0.666)\n" +
      "    c = cos(pos.x*0.02 + n*10.0);//*0.5+0.5;\n" +
      "  else\n" +
      "  {\n" +
      "    pos *= 0.05;\n" +
      "    c = abs(sin(pos.x+n*5.0)*cos(pos.y+n*5.0));\n" +
      "  }\n" +
      " }\n" +
      " else if(p.y < 0.666) // middle row\n" +
      " {\n" +
      "   \n" +
      "   if(p.x < 0.333)\n" +
      "   {\n" +
      "     pos *= 0.05;\n" +
      "     pos += vec2(10.0, 10.0);\n" +
      "     c = sqrt(pos.x * pos.x + pos.y * pos.y);\n" +
      "    c = fract(c+n);\n" +
      " }\n" +
      " else if(p.x < 0.666)\n" +
      " {\n" +
      "  c = max(1.0 - mod(pos.x*0.5, 80.3+n*4.0)*0.5, 1.0 -  mod(pos.y*0.5, 80.3+n*4.0)*0.5);\n" +
      "  c = max(c, 0.5*max(1.0 - mod(pos.x*0.5+40.0, 80.3+n*4.0)*0.5, 1.0 -  mod(pos.y*0.5+40.0, 80.3+n*4.0)*0.5));\n" +
      " }\n" +
      " else\n" +
      "  c = abs(cos(pos.x*0.1 + n*20.0));// mod(pos.x*0.1, cos(pos.x));\n" +
      " }\n" +
      " else // first row\n" +
      " {\n" +
      "   if(p.x < 0.333)\n" +
      "     c = noise(pos);\n" +
      "   else if(p.x < 0.666)\n" +
      "     c = n;\n" +
      "   else\n" +
      "     c =max(fract(n*20.0), max(fract(n*5.0), fract(n*10.0)));\n" +
      " }\n" +
      "\n" +
      " gl_FragColor = vec4(c, c, c, 1.0);\n" +
      "}\n"
  },
  {
    name: "blank",
    geometryType: "image",
    geometryHints: "",
    vertex: "",
    fragment: ""
  }
];

//
//
// Helper functions
//
//
function defaultImageSetup(imageActor, filename) {
  "use strict";
  var i = imageActor;
  i.visible = true;
  i.anchorPoint = [0.5, 0.5, 0.5];
  i.parentOrigin = [0.5, 0.5, 0.5];
  i.position = [0, 0, -10];
  i.size = [100, 100, 1];
  i.name = filename.replace(/-/g, "_").replace(/\./g, "_");
}

function createImageFromTypeBuffer(filename, typedBuffer) {
  "use strict";
  var i = new dali.ImageActor();
  //
  // Setting image isn't property based as browser cannot load from a file
  //
  // i.image         = {"filename":"dali-logo.png"};
  i.setImage(typedBuffer); // encoded image data
  defaultImageSetup(i, filename);
  return i;
}

function createImageRGBFromTypeBuffer(filename, typedBuffer) {
  "use strict";
  var i = new dali.ImageActor();
  i.setImageDataRGB(typedBuffer);
  defaultImageSetup(i, filename);
  return i;
}

function createImageRGBFromImageData(filename, imageData)
{
  "use strict";
  var i = new dali.ImageActor();
  i.setImageDataRGB(imageData); // decoded RGB data
  defaultImageSetup(i, filename);
  return i;
}

function addImageActor(filename, typedBuffer) {
  "use strict";
  var i = createImageFromTypeBuffer(filename, typedBuffer);
  app.addActor(i);
}

function addImageActorRGB(filename, imageData) {
  "use strict";
  var i = createImageRGBFromImageData(filename, imageData);
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

function asyncFileReaderGetImageData(file, callback) {
  "use strict";
  if (!file) {
    return;
  }

  var img = new Image();

  var objectUrl = window.URL.createObjectURL(file);

  img.onload = function( //e
    ) {
      var canvas = document.createElement("canvas");
      canvas.width = img.width; // naturalWidth;
      canvas.height = img.height; // naturalHeight;
      var context = canvas.getContext("2d");
      context.drawImage(img, 0, 0 );
      var imageData = context.getImageData(0, 0, img.naturalWidth, img.naturalHeight); // <-ImageData
      callback(file.name, imageData);
      window.URL.revokeObjectURL(objectUrl);
    };

  img.src = objectUrl;

}

function addTestImage(index) {
  "use strict";
  var name = "testImage" + index;
  // need to draw it off screen first? @todo
  var canvas = document.createElement("canvas");
  var img = document.getElementById(name);
  canvas.width = img.naturalWidth;
  canvas.height= img.naturalHeight;
  var context = canvas.getContext("2d");
  context.drawImage(img, 0, 0 );
  var imageData = context.getImageData(0, 0, img.naturalWidth, img.naturalHeight); // <-ImageData

  // var typedBuffer = new Uint8Array(imageData.data.buffer); // convert ArrayBuffer into a typed array?

  var i = new dali.ImageActor();
  //
  // Setting image isn't property based as browser cannot load from a file
  //
  i.setImageData(imageData);
  // i.image         = {"filename":"dali-logo.png"};

  i.visible = true;
  i.anchorPoint = [0.5, 0.5, 0.5];
  i.parentOrigin = [0.5, 0.5, 0.5];
  i.position = [0, 0, -10];
  i.size = [100, 100, 1];
  i.name = name.replace(/-/g, "_").replace(/\./g, "_");

  app.addActor(i);

  return i;
}

function add1ColElement(elem, value)
{
  var e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = value;
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = "";
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = "";
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = "";
  elem.appendChild(e);

  return e;
}

function add2ColElement(elem, value)
{
  var e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = value[0];
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = value[1];
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = "";
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = "";
  elem.appendChild(e);

  return e;
}

function add3ColElement(elem, value)
{
  var e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = value[0];
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = value[1];
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = value[2];
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = "";
  elem.appendChild(e);
  return e;
}

function add4ColElement(elem, value)
{
  var e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = value[0];
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = value[1];
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = value[2];
  elem.appendChild(e);

  e = document.createElement("div");
  e.className = "col-md-3";
  e.innerHTML = value[3];
  elem.appendChild(e);
  return e;
}

function createElementForActorProperty(actor, name) {
  "use strict";
  var elem = document.createElement("div");

  var value = actor[name];
  var type = typeof value;

  if(type === "string" || type === "number" || type === "boolean") {
    elem.className = "row";
    add1ColElement(elem, value);
  } else if(value === undefined) {
    elem.className = "row";
    add1ColElement(elem, "???undefined???");
  } else {
    var length = value.length;
    if(length === 2) {
      elem.className = "row";
      add2ColElement(elem, value);
    } else if(length === 3) {
      elem.className = "row";
      add3ColElement(elem, value);
    } else if(length === 4) {
      elem.className = "row";
      add4ColElement(elem, value);
    } else if(length === 9) {
      var r = document.createElement("div");
      r.className = "row";
      add3ColElement(r, value.slice(0, 3));
      elem.appendChild(r);

      r = document.createElement("div");
      r.className = "row";
      add3ColElement(r, value.slice(3, 6));
      elem.appendChild(r);

      r = document.createElement("div");
      r.className = "row";
      add3ColElement(r, value.slice(6, 9));
      elem.appendChild(r);
    } else if(length === 16) {
      var r = document.createElement("div");
      r.className = "row";
      add4ColElement(r, value.slice(0, 4));
      elem.appendChild(r);

      r = document.createElement("div");
      r.className = "row";
      add4ColElement(r, value.slice(4, 8));
      elem.appendChild(r);

      r = document.createElement("div");
      r.className = "row";
      add4ColElement(r, value.slice(8, 12));
      elem.appendChild(r);

      r = document.createElement("div");
      r.className = "row";
      add4ColElement(r, value.slice(12, 16));
      elem.appendChild(r);
    }
  }

  return elem;
}

function onActorSelected(actor) {
  "use strict";
  var i;
  var gridBlock;
  var nameBlock;
  var valueBlock;
  var actorId;

  //
  // recreate property tab
  //
  var elem = document.getElementById("selected");

  removeAllChildren(elem);

  //
  // setup property view
  //
  if (actor) {
    actorId = actor.getId();

    var p = document.createElement("div");
    p.innerHTML = "<h3><b>" + actor.name + "</b> [<b>" + actorId + "</b>] (<b>" + actor.position + "</b>)</h3>";
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

    if( actorId in actorIdToShaderSet && 
        "shaderEffect" in actorIdToShaderSet[actorId]) { // if has compiled
      // if(actorId in actorIdToShaderSet) {
      var shaderOptions = actorIdToShaderSet[actorId];
      var uniforms = dali.extractUniformsFromShaderSource(shaderOptions.vertex);
      uniforms = uniforms.concat(dali.extractUniformsFromShaderSource(shaderOptions.fragment));

      var shader = getShader(actor);

      if(uniforms.length) {
        gridBlock = document.createElement("div");
        gridBlock.className = "row";

        nameBlock = document.createElement("div");
        nameBlock.className = "col-md-5";
        nameBlock.innerHTML = "<u>Shader Uniforms:</u>";
        gridBlock.appendChild(nameBlock);

        valueBlock = document.createElement("div");
        valueBlock.className = "col-md-7";
        valueBlock.innerHTML = "";
        gridBlock.appendChild(valueBlock);

        elem.appendChild(gridBlock);

        for(i = 0; i < uniforms.length; i++) {
          var type = uniforms[i][0];
          var name = uniforms[i][1];
          gridBlock = document.createElement("div");
          gridBlock.className = "row";

          nameBlock = document.createElement("div");
          nameBlock.className = "col-md-5";
          nameBlock.innerHTML = type + " " + name +
            "<a class=\"btn btn-default btn-xs\" href=\"javascript:addPropertyToAnimation(" + actorId + ",'" + name + "')\"" + " role=\"button\">Animate</a>";

          gridBlock.appendChild(nameBlock);

          valueBlock = document.createElement("div");
          valueBlock.className = "col-md-7";
          valueBlock.innerHTML = shader[ name ];
          gridBlock.appendChild(valueBlock);

          elem.appendChild(gridBlock);

        }
      }

    }

    var props = actor.getProperties(); // std::vector<string>

    gridBlock = document.createElement("div");
    gridBlock.className = "row";

    nameBlock = document.createElement("div");
    nameBlock.className = "col-md-5";
    nameBlock.innerHTML = "<u>Properties:</ul>";
    gridBlock.appendChild(nameBlock);

    valueBlock = document.createElement("div");
    valueBlock.className = "col-md-7";
    valueBlock.innerHTML = "";
    gridBlock.appendChild(valueBlock);

    elem.appendChild(gridBlock);


    for (i = 0; i < props.size(); i++) {
      var name = props.get(i);

      gridBlock = document.createElement("div");
      gridBlock.className = "row";

      var animatable = actor.isPropertyAnimatable(actor.getPropertyIndex(name));

      nameBlock = document.createElement("div");
      nameBlock.className = "col-md-5";
      if(animatable) {
        nameBlock.innerHTML = name + "<a class=\"btn btn-default btn-xs\" href=\"javascript:addPropertyToAnimation(" + actorId + ",'" + name + "')\"" + " role=\"button\">Animate</a>";
      } else {
        nameBlock.innerHTML = name;
      }
      gridBlock.appendChild(nameBlock);

      valueBlock = document.createElement("div");
      valueBlock.className = "col-md-7";

      valueBlock.appendChild( createElementForActorProperty(actor, name) );

      gridBlock.appendChild(valueBlock);

      elem.appendChild(gridBlock);
    }

    props.delete();


    //
    // load current actors shaders
    //
    userEditingShader = false; // ie dont do anything on ui onChange()

    if(actorId !== lastActorSelectedId) {
      if(actorId in actorIdToShaderSet) {
        var options = actorIdToShaderSet[actorId];
        setShaderUI(options);
      } else {
        setShaderUI({name: "",
                     geometryType: "image",
                     geometryHints: "",
                     vertex: "",
                     fragment: ""});
      }
    }

    userEditingShader = true;

    lastActorSelectedId = actorId;

  }

}


function createElement(elementName, namesValues) {
  "use strict";
  var e = document.createElement(elementName);
  for(var name in namesValues) {
    e[name] = namesValues[name];
  }
  return e;
}

function createElementInnerHTML(elementName, innerHTML) {
  "use strict";
  return createElement(elementName, {"innerHTML": innerHTML});
}

function removeAllChildren(elem) {
  "use strict";
  var count = elem.children.length;
  for (var i = 0; i < count; i++) {
    elem.removeChild(elem.children[0]);
  }
}

function onChangeSetDataChanged(e) {
  e.currentTarget["data-changed"] = true;
}

function inputElem(type, propObject, property, val, arrayIndex) {
  "use strict";
  var e = document.createElement("input");
  e.type = type;
  // for number then step of any means it can be a real number not just integer
  e.step = "any";
  e.value = val;
  e.className = "form-control";
  e["data-value"] = val;
  e["data-arrayIndex"] = arrayIndex;
  e["data-changed"] = false;
  e.addEventListener("changed", onChangeSetDataChanged);
  return e;
}

function inputElem2(type, propObject, property, val, startIndex) {
  "use strict";
  var d = document.createElement("div");
  var a = inputElem("number", propObject, property, val[startIndex], startIndex);
  a.className = "col-md-3";
  d.appendChild(a);
  a = inputElem("number", propObject, property, val[startIndex + 1], startIndex + 1);
  a.className = "col-md-3";
  d.appendChild(a);
  a.className = "col-md-3";
  d.appendChild(a);
  a = createElement("div");
  a.className = "col-md-3";
  d.appendChild(a);
  return d;
}

function inputElem3(type, propObject, property, val, startIndex) {
  "use strict";
  var d = document.createElement("div");
  var a = inputElem("number", propObject, property, val[startIndex], startIndex);
  a.className = "col-md-3";
  d.appendChild(a);
  a = inputElem("number", propObject, property, val[startIndex + 1], startIndex + 1);
  a.className = "col-md-3";
  d.appendChild(a);
  a = inputElem("number", propObject, property, val[startIndex + 2], startIndex + 2);
  a.className = "col-md-3";
  d.appendChild(a);
  a = createElement("div");
  a.className = "col-md-3";
  d.appendChild(a);
  return d;
}

function inputElem4(type, propObject, property, val, startIndex) {
  "use strict";
  var d = document.createElement("div");
  var a = inputElem("number", propObject, property, val[startIndex], startIndex);
  a.className = "col-md-3";
  d.appendChild(a);
  a = inputElem("number", propObject, property, val[startIndex + 1], startIndex + 1);
  a.className = "col-md-3";
  d.appendChild(a);
  a = inputElem("number", propObject, property, val[startIndex + 2], startIndex + 2);
  a.className = "col-md-3";
  d.appendChild(a);
  a = inputElem("number", propObject, property, val[startIndex + 3], startIndex + 3);
  a.className = "col-md-3";
  d.appendChild(a);
  return d;
}

function createInputElement(actorShader, propertyName, value) {
// always4 ja columns
  "use strict";
  var type = typeof value;

  var e;
  if(type === "string") {
    e = inputElem("string", actorShader, propertyName, value);
  } else if(type === "number") {
    e = inputElem("number", actorShader, propertyName, value);
  } else if(type === "boolean") {
    e = inputElem("checkbox", actorShader, propertyName, value);
  } else {
    var length = value.length;
    if(length === 2) {
      e = inputElem2("number", actorShader, propertyName, value, 0);
    } else if(length === 3) {
      e = inputElem3("number", actorShader, propertyName, value, 0);
    } else if(length === 4) {
      e = inputElem4("number", actorShader, propertyName, value, 0);
    } else if(length === 9) {
      e = createElement("div");
      e.appendChild( inputElem3("number", actorShader, propertyName, value, 0) );
      e.appendChild( inputElem3("number", actorShader, propertyName, value, 3) );
      e.appendChild( inputElem3("number", actorShader, propertyName, value, 6) );
    } else if(length === 16) {
      e = createElement("div");
      e.appendChild( inputElem4("number", actorShader, propertyName, value, 0) );
      e.appendChild( inputElem4("number", actorShader, propertyName, value, 4) );
      e.appendChild( inputElem4("number", actorShader, propertyName, value, 8) );
      e.appendChild( inputElem4("number", actorShader, propertyName, value, 12) );
    } else {
      throw "should not happen";
    }
  }

  return e;
}

function rebuildAnimationList() {
  "use strict";

  var tbody = $("#animationTableBody")[0];

  removeAllChildren(tbody);

  var properties = animationList[animationSelectionIndex].properties;

  for(var i = 0; i < properties.length; i++) {
    var property = properties[i];
    var tr = createElement("tr");
    var td = createElement("td", {innerHTML: "" + property.actor + property.actor.name + "(" + property.actor.getId() + ")"});
    tr.appendChild(td);
    td = createElement("td", {innerHTML: "" + property.property});
    tr.appendChild(td);
    td = createElement("td", {innerHTML: "" + property.value});
    tr.appendChild(td);
    td = createElement("td", {innerHTML: "" + property.interpolation});
    tr.appendChild(td);
    td = createElement("td", {innerHTML: "" + property.delay});
    tr.appendChild(td);
    td = createElement("td", {innerHTML: "" + property.duration});
    tr.appendChild(td);
    td = createElement("td",
                       {innerHTML: "<a class=\"btn btn-default btn-xs\" href=\"javascript:changeAnimationProperty(" + i + ")\"" + " role=\"button\">...</a>"});
    tr.appendChild(td);

    tbody.appendChild(tr);
  }

}

function changeAnimationProperty(index) {
  "use strict";

  var e = $("#modalPropertyBody")[0];

  rebuildAnimationList();

  $("#animationTabHeader")[0].click();

  var ps = animationList[animationSelectionIndex].properties[index];
  var propertyName = ps.property;
  var actor = ps.actor;
  var actorId = actor.getId();
  var value = ps.value;
  var delay = ps.delay;
  var duration = ps.duration;

  // setup modal dialog to change added value
  e["data-animationPropertyIndex"] = index;
  e["data-propertyName"] = propertyName;
  e["data-object"] = actor;

  removeAllChildren(e);

  e.appendChild( createElementInnerHTML("div", "Add to animation :" + animationSelectionIndex) );

  e.appendChild( createElementInnerHTML("hr", "") );

  e.appendChild(
    createElement("div",
                  {innerHTML: "Actor :(" + actorId + ") '" + actor.name + "'"
                  }) );

  e.appendChild( createElementInnerHTML("hr", "") );

  e.appendChild( createElementInnerHTML("div", "Property:" + propertyName) );

  e.appendChild( createElementInnerHTML("hr", "") );

  // e.appendChild( createElementInputForValue(value) );

  e.appendChild( createInputElement(actor, propertyName, value) );

  e.appendChild( createElementInnerHTML("hr", "") );
  e.appendChild( createElementInnerHTML("div", "Delay") );
  e.appendChild( createElementInnerHTML("hr", "") );
  e.appendChild( createElement("input", {step: "any", type: "number", id: "delay", value: delay} ) );

  e.appendChild( createElementInnerHTML("hr", "") );
  e.appendChild( createElementInnerHTML("div", "Duration") );
  e.appendChild( createElementInnerHTML("hr", "") );
  e.appendChild( createElement("input", {step: "any", type: "number", id: "duration", value: duration} ) );

  $("#modalAddProperty").modal("show");

}

function addPropertyToAnimation(actorId, propertyName) {
  "use strict";
  // var e = document.getElementById("modalPropertyBody");

  var root = dali.stage.getRootLayer();
  var actor = root.findChildById(actorId);
  var value;

  if(propertyName in actor) {
    value = actor[propertyName];
  } else {
    var shader = actor.getShader();
    value = shader[propertyName];
    shader.delete(); // wrapper
  }

  // add an item in and switch to the property view
  animationList[animationSelectionIndex].properties.push( {actor: actor,
                                                           property: propertyName,
                                                           value: value,
                                                           interpolation: "Linear",
                                                           delay: 0,
                                                           duration: 3} );

  changeAnimationProperty( animationList[animationSelectionIndex].properties.length - 1 );

  animationList[animationSelectionIndex].dirtyData = true;

  root.delete();
}

function getAllElements(tagName, fromRoot, list) {
  "use strict";

  for(var i = 0; i < fromRoot.children.length; i++) {
    var c = fromRoot.children[i];
    if(c.tagName === tagName) {
      list.push(c);
    }
    if(c.children.length) {
      getAllElements(tagName, c, list);
    }
  }
}


$("#addPropertySave")[0].addEventListener("click", function(//e
) {
  "use strict";

  var modal = $("#modalPropertyBody")[0];
  var index = modal["data-animationPropertyIndex"];
  var animationInfo = animationList[animationSelectionIndex];
  var prop = animationInfo.properties[index];
  var delay = document.getElementById("delay");
  var duration = document.getElementById("duration");

  var inputs = [];

  getAllElements("INPUT", modal, inputs);

  var value = [];
  for(var i = 0; i < inputs.length; i++) {
    var c = inputs[i];
    if(c.id === "duration" || c.id === "delay") {
      continue;
    }
    value.push(Number(c.value));
  }

  if(value.length === 1) {
    value = value[0];
  }

  prop.value = value;
  prop.delay = Number(delay.value);
  prop.duration = Number(duration.value);
  prop.interpolation = "Linear";

  rebuildAnimationList();

  animationInfo.dirtyData = true;

  console.log("changed property");

});


function selectAnimation(index) {
  "use strict";
  var e = $("#animationSelection")[0];

  if(index < animationList.length) {
    animationSelectionIndex = index;
    e.innerHtml = "Animation:" + animationSelectionIndex;
  } else {
    animationSelectionIndex = undefined;
    e.innerHtml = "Animation: None selected";
  }

  $("#btnAnimationSelection")[0].innerHTML = "Selection:" +
    animationSelectionIndex + " <span class=\"caret\"></span>";

  rebuildAnimationList();

}

function rebuildAnimationMenu() {
  "use strict";
  var i;

  var e = $("#animationDropDown")[0];
  var count = e.children.length;
  for (i = 0; i < count; i++) {
    e.removeChild(e.children[0]);
  }

  // <li><a href="javascript:selectAnimation(1)">Anim 1</a></li>

  for(i = 0; i < animationList.length; i++) {
    var li = document.createElement("li");

    var a = document.createElement("a");
    a.href = "javascript:selectAnimation(" + i + ");";
    a.text = "Select Animation(" + i + ") '"; // + child.name + "'";
    li.appendChild(a);

    e.appendChild(li);
  }

  selectAnimation(animationSelectionIndex);
}

function addNewAnimation() {
  "use strict";
  animationList.push( { animation: null,
                        properties: [],
                        dirtyData: false,
                        looping: false,
                        endAction: "Discard"
                      } );

  animationSelectionIndex = animationList.length - 1;

  rebuildAnimationMenu();
}

function playAnimation() {
  "use strict";
  var animationInfo = animationList[animationSelectionIndex];

  if(animationInfo.animation && animationInfo.dirtyData) {
    animationInfo.animation.stop();
    animationInfo.animation.clear();
    animationInfo.animation.delete(); // wrapper
    animationInfo.animation = null;
    animationInfo.dirtyData = false;
  }

  if(animationInfo.animation) {
    animationInfo.animation.play();
  } else {
    var props = animationInfo.properties;
    animationInfo.animation = new dali.Animation();
    var animation = animationInfo.animation;
    for(var i = 0; i < props.length; i++) {
      var p = props[i];

      if(p.property in p.actor) {
        animation.animateTo(p.actor, p.property, p.value, p.interpolation, p.delay, p.duration );
      } else {
        var shader = p.actor.getShader();
        animation.animateTo(shader, p.property, p.value, p.interpolation, p.delay, p.duration );
        shader.delete(); // wrapper
      }
    }

    animation.setEndAction(animationInfo.endAction);

    animation.setLooping(animationInfo.looping);

    animation.play();
  }
}

function clearAnimation() {
  "use strict";
  var animationInfo = animationList[animationSelectionIndex];

  if(animationInfo.animation) {
    animationInfo.animation.stop();
    animationInfo.animation.clear();
    animationInfo.animation.delete(); // wrapper
    animationInfo.animation = null;
  }

  animationInfo.properties = [];

  rebuildAnimationList();
}

function stopAnimation() {
  "use strict";
  var animationInfo = animationList[animationSelectionIndex];

  if(animationInfo.animation) {
    animationInfo.animation.stop();
  }
}

function selectActor(id) { // from html tree
  "use strict";
  if(id === null) {
    eventHandler.selectActor(null);
  } else {
    var root = dali.stage.getRootLayer();
    var actor = root.findChildById(id);
    if (actor) {
      eventHandler.selectActor(actor);
    }
    root.delete(); // wrapper
  }
}

function rebuildTree() {
  "use strict";
  // remove childred
  var e = document.getElementById("tree");
  var count = e.children.length;
  for (var i = 0; i < count; i++) {
    e.removeChild(e.children[0]);
  }

  // <ul class="nav nav-list">
  //      <li><label class="tree-toggler nav-header">Header 1</label>

  var root = dali.stage.getRootLayer() ; // dali.stage.rootRotationActor;

  var recurse = function(parentElement, actor) {
    var children = actor.getChildren();
    if (children) {
      var ul = null;

      for (var i = 0; i < children.length; ++i) {
        var child = children[i];

        if (child.name[0] !== "*") {

          var li = document.createElement("li");

          var a = document.createElement("a");
          a.href = "javascript:selectActor(" + child.getId() + ");";
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

  recurse(e, root);

  root.delete(); // wrapper
}


function EventHandler() {
  "use strict";
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
  this.handlersMouseMove = []; // valid; touchedActor(can be null), mouse_x, mouse_y
  this.handlersMouseDrag = []; // valid; touchedActor(can be null), drag_dx, drag_dy
  this.handlersSelect = []; // valid; touchedActor(can be null), drag_dx, drag_dy
  this.handlersDeselect = []; // valid; touchedActor(can be null), drag_dx, drag_dy
}

EventHandler.prototype.register = function(name, handler) {
  "use strict";
  var handlers = this["handlers" + name];
  if (!(handler in handlers)) {
    handlers.push(handler);
  }
};

EventHandler.prototype.unRegister = function(name, handler) {
  "use strict";
  var handlers = this["handlers" + name];
  var index = handlers.index(handler);
  if (index >= 0) {
    handlers.splice(index, 1);
  }
};

EventHandler.prototype.updateMouseState = function(canvas, e) {
  "use strict";
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
    hid = hitActor.getId();
  }

  if (hid !== tid) {
    if (tid > 0) {
      this.handleEvent(this.handlersDeselect);
      this.touchedActor = null;
    }
  }

  this.selectActor(hitActor);

};

EventHandler.prototype.selectActor = function(hitActor) {
  "use strict";
  if (hitActor) {
    this.handleEvent(this.handlersDeselect);

    var layer = dali.stage.getRootLayer();
    if (hitActor.getId() === dali.stage.rootRotationActor.getId()) {
      // dont select our rotation actor
      hitActor = null;
    } else {
      // dont select the root layer
      if (hitActor.getId() === layer.getId()) {
        hitActor = null;
      }
    }
    layer.delete(); // wrapper

    this.mouseDownPosition = [this.mouse_x, this.mouse_y, 0];
    this.touchedActor = hitActor;
  } else {
    if(this.touchedActor) {
      this.handleEvent(this.handlersDeselect);
    }
    this.touchedActor = null;
  }

  this.handleEvent(this.handlersSelect);
};


EventHandler.prototype.mouseMove = function(canvas, e) {
  "use strict";
  this.updateMouseState(canvas, e);

  if(this.mouseIsDown) {
    // if (this.touchedActor) {
    this.drag_dx = (this.mouse_x - this.mouseDownPosition[0]);
    this.drag_dy = (this.mouse_y - this.mouseDownPosition[1]);
    // }

    this.handleEvent(this.handlersMouseDrag);
  }

  this.handleEvent(this.handlersMouseMove);
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
    a.name = "**_dali_adornment";
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
  "use strict";
  var i;
  var a;
  this.attachedTo = null;
  this.resizers = []; // array of 8 small square resizers
  this.lines = []; // array of 4 bounding box lines
  this.lineColor = [0.5, 0.5, 0.5, 1];
  this.resizerColor = [0, 0, 0, 1];
  this.depthAdornments = new DepthAdornment();

  // ActorWrapper CreateSolidColorActor( const Vector4& color, bool border, const Vector4& borderColor, const unsigned int borderSize )
  for (i = 0; i < 4; i++) { // lines
    a = dali.createSolidColorActor(this.lineColor, false, [0, 0, 0, 1], 0);
    a.name = "**_dali_adornment";
    this.lines.push(a);
  }

  for(i = 0; i < 8; i++) { // squares
    a = dali.createSolidColorActor(this.resizerColor, true, [1, 1, 1, 1], 1);
    a.name = "**_dali_adornment";
    this.resizers.push(a);
    this.resizers[i].size = [10, 10, 1];
    //    this.resizers[i].__mouseDrag = this.__mouseDragPositionZ;
    // this.resizers[i].connectSignal("touched", this.onTouched);
  }

  ActorList.show(this.lines, false);
  ActorList.show(this.resizers, false);

}


Cursors.prototype.attach = function(actor) {
  "use strict";
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
  "use strict";
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
  "use strict";
  if (this.attachedTo) {
    this.__positionActors(this.attachedTo);
    this.depthAdornments.reposition();
  }
};

Cursors.prototype.__positionActors = function(actor) {
  "use strict";
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
  "use strict";
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
  this.rootLayerIndicator.name = "**_dali_adornment";

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
      dali.stage.rootRotationActor.rotation = self.rootRotation;
    }

  };

  this.addActor = function(actor) {
    if (eventHandler.touchedActor) {
      eventHandler.touchedActor.add(actor);
      actor.position = [20,20,0]; // offset so new actor isnt right behind touched
    } else {
      dali.stage.add(actor);
    }

    rebuildTree();

  };

  this.__handleSelect = function(eventHandler) {
    if (eventHandler.mouse_buttons === 0) { // left click only selects actor
      var actor = eventHandler.touchedActor;
      self.selectedActor = actor;
      self.cursors.attach(self.selectedActor);
      dali.stage.add(self.rootLayerIndicator);
      onActorSelected(actor);
    }
  };

  this.__handleDeselect = function(eventHandler) {
    self.cursors.unattach(self.selectedActor);
    dali.stage.remove(self.rootLayerIndicator);
  };

  // initialize
  eventHandler.register("MouseDrag", this.__handleMouseDrag);
  eventHandler.register("Select", this.__handleSelect);
  eventHandler.register("Deselect", this.__handleDeselect);

  //
  // Monkey patch the stage object (to add to root object not the root layer for rotations)
  //
  dali.stage.rootRotationActor = new dali.Actor();
  dali.stage.rootRotationActor.parentOrigin = [0.5, 0.5, 0.5];
  dali.stage.rootRotationActor.anchorPoint = [0.5, 0.5, 0.5];
  dali.stage.rootRotationActor.name = "app rotation actor";
  dali.stage.add(dali.stage.rootRotationActor);

  dali.stage.add = function(actor) {
    dali.stage.rootRotationActor.add(actor);
  };

  dali.stage.remove = function(actor) {
    dali.stage.rootRotationActor.remove(actor);
  };

}

function addTextImageFromCanvas(text) {
  "use strict";
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

  app.addActor(a);

  return a;
}



//
// Global
//

var eventHandler = new EventHandler();
var app = new App(eventHandler);
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
              "Linear", 0, 3);
  a.animateTo(actor, "position", [0, 0, 0], "Linear", 3, 3);
  a.animateTo(actor, "rotation", [0, 0, 1, 720 * Math.random() - 360], "Linear", 0, 3);
  a.animateTo(actor, "rotation", [0, 0, 1, 0], "Linear", 3, 3);

  a.play();
  //    a.delete(); // delete the animtion object but a.Play() keeps it running in Dali
  return anim;
}

function animate3DView() {
  var root = dali.stage.rootRotationActor;
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

  anim.animateBy(rotator, "rotation", toRotation, "Linear", 0, 2);
  anim.animateBy(rotator, "rotation", actors[i].rotation, "Linear", 2, 2);

  anim.animateBy(rotator, "position", [0, 0, -800], "Linear", 0, 2);
  anim.animateBy(rotator, "position", [0, 0, 0], "Linear", 2, 2);

  anim.play();
  anim.delete();

}

//------------------------------------------------------------------------------
// test helper functions
//------------------------------------------------------------------------------

function clear() {
  var root = dali.stage.rootRotationActor;
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
  var root = dali.stage.rootRotationActor;

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
  var root = dali.stage.rootRotationActor;

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
  var root = dali.stage.rootRotationActor;
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

var selectGeometryType = function(index) {
  "use strict";
  if(index === 0) {
    geometryType = "image";
  } else if(index === 1) {
    geometryType = "text";
  } else if(index === 2) {
    geometryType = "untextured mesh";
  } else if(index === 3) {
    geometryType = "textured mesh";
  }

  $("#dropdownGeometryType")[0].innerText = "Geometry:" + geometryType;
};

var getShadersFromUi = function() {
  "use strict";
  var e = ace.edit("editorVertex");
  var ret = {};
  ret.vertex = native2ascii(e.getSession().getValue());
  e = ace.edit("editorFragment");
  ret.fragment = native2ascii(e.getSession().getValue());

  ret.geometryType = "image"; // @todo! use shader state

  ret.geometryHints = "";
  if( $("#grid")[0].checked ) {
    ret.geometryHints += " grid";
  }
  if( $("#gridx")[0].checked ) {
    ret.geometryHints += " gridx";
  }
  if( $("#gridy")[0].checked ) {
    ret.geometryHints += " gridy";
  }
  if( $("#depthbuffer")[0].checked ) {
    ret.geometryHints += " depthbuffer";
  }
  if( $("#blending")[0].checked ) {
    ret.geometryHints += " blending";
  }
  if( $("#doesntModifyGeometry")[0].checked ) {
    ret.geometryHints += " doesntModifyGeometry";
  }

  return ret;
};

$("#modalSaveShaderYes")[0].addEventListener("click", function(//e
) {
  "use strict";
  var name = $("#modalSaveShaderName")[0].value;

  var info = modalSaveOldShader;
  info.name = name;
  shaderSources.push(info);

  rebuildShaderDropdown();

  console.log("changed property");

});

var setShaderUI = function(options) {
  "use strict";
  var e;
  var vertex = "";
  var fragment = "";
  var geometryType = "";
  var geometryHints = "";

  vertex = options.vertex;
  fragment = options.fragment;
  geometryType = options.geometryType;
  geometryHints = options.geometryHints;

  e = ace.edit("editorVertex");
  e.getSession().setValue(vertex);

  e = ace.edit("editorFragment");
  e.getSession().setValue(fragment);

  if(geometryType.search("image") >= 0) {
    $("#geometryImage")[0].click();
  }
  if(geometryType.search("text") >= 0) {
    $("#geometryText")[0].click();
  }
  if(geometryType.search("untexturedMesh") >= 0) {
    $("#geometryUntexturedMesh")[0].click();
  }
  if(geometryType.search("texturedMesh") >= 0) {
    $("#geometryTexturedMesh")[0].click();
  }
// <li><label class="btn btn-primary"> <input type="checkbox" id="grid" autocomplete="off">Grid </label></li>
// <li><label class="btn btn-primary"> <input type="checkbox" id="gridx"  autocomplete="off">GridX</label></li>
// <li><label class="btn btn-primary"> <input type="checkbox" id="gridy" autocomplete="off">GridY</label></li>
// <li><label class="btn btn-primary"> <input type="checkbox" id="depthbuffer" autocomplete="off">DepthBuffer</label></li>
// <li><label class="btn btn-primary"> <input type="checkbox" id="blending" autocomplete="off">Blending</label></li>
// <li><label class="btn btn-primary"> <input type="checkbox" id="doesntModifyGeometry" autocomplete="off">Doesnt modify geometry</label></li>

};

// load shader into actor.
var loadShader = function(index) {
  "use strict";
  var options;
  var id = dali.actor.getId();

  if( id in actorIdToShaderSet ) {
    // save the old one if desired
    modalSaveOldShader = getShadersFromUi();
    $("#modalSaveShaderName")[0].value = new Date().toUTCString();
    $("#modalSaveShader").modal("show");
  }

  options = shaderSources[index];

  // copy object
  actorIdToShaderSet[id] = {name: options.name,
                            geometryType: options.geometryType,
                            geometryHints: options.geometryHints,
                            vertex: options.vertex,
                            fragment: options.fragment};

  setShaderUI(actorIdToShaderSet[id]);

};

var getShaderInfo = function() {
  "use strict";
  var uishaders = getShadersFromUi();

  return dali.getProgramInfo( canvas.getContext("webgl"),
                              uishaders.geometryType,
                              uishaders.vertex,
                              uishaders.fragment );
};

var checkShader = function() {
  "use strict";
  var info = getShaderInfo();
  // var info = {
  //   attributes: [],
  //   uniforms: [],
  //   attributeCount: 0,
  //   uniformCount: 0,
  //   hasError: false,    // compiles without error
  //   vertexError: "",
  //   fragmentError: "",
  //   linkError: ""
  // };

  var vertexPrepend = dali.prepends[geometryType].vertex;
  var fragmentPrepend = dali.prepends[geometryType].fragment;

  var errors = "";
  var count;
  var editor;
  var description;
  var col;
  var line;
  var i;

  textShaderErrors.value = "";

  editor = ace.edit("editorVertex");
  if (info.hasError && info.vertexError) {
    editor.getSession().setOption("useWorker", false);
    textShaderErrors.value = "VERTEX:\n" + info.vertexError;
    errors = info.vertexError.split("\n");
    count = vertexPrepend.split("\n").length;
    for(i = 0; i < errors.length; i++) {
      if(errors[i]) {
        description = errors[i].split(":");
        col = Number(description[1]);
        try {
          line = Number(description[2]);
        } catch(e) {
          line = "unknown";
        }
        editor.getSession().setAnnotations([{row: line - count, column: col, text: errors[i], type: "error"}]);
      }
    }
  } else {
    editor.getSession().setOption("useWorker", true);
    editor.getSession().setAnnotations([]);
  }

  editor = ace.edit("editorFragment");
  if (info.hasError && info.fragmentError) {
    editor.getSession().setOption("useWorker", false);
    textShaderErrors.value += "FRAGMENT:\n" + info.fragmentError;
    errors = info.fragmentError.split("\n");
    count = fragmentPrepend.split("\n").length;
    for(i = 0; i < errors.length; i++) {
      if(errors[i]) {
        description = errors[i].split(":");
        col = Number(description[1]);
        try {
          line = Number(description[2]);
        } catch(e) {
          line = "unknown";
        }
        editor.getSession().setAnnotations([{row: line - count, column: col, text: errors[i], type: "error"}]);
      }
    }
  } else {
    editor.getSession().setOption("useWorker", true);
    editor.getSession().setAnnotations([]);
  }

  // @todo 
  // if(!info.hasError) {
  //   if(shaderSourceSelection >= 0) {
  //     shaderSources[shaderSourceSelection].vertex = info.vertex;
  //     shaderSources[shaderSourceSelection].fragment = info.fragment;
  //   }
  // }

  return !info.hasError;
};

var setShader = function(actor) {
  "use strict";
  var ret = false;
  if (checkShader()) { // otherwise dali stops working
    var id = actor.getId();
    if( id in actorIdToShaderSet) {
      if("shaderEffect" in actorIdToShaderSet[id]) {
        actorIdToShaderSet[id].shaderEffect.delete(); // the wrapper
      }
    }

    var shaderOptions = getShadersFromUi();
    var shaderEffect = new dali.ShaderEffect(shaderOptions);
    actor.setShaderEffect(shaderEffect);
    actorIdToShaderSet[ id ] = shaderOptions;
    actorIdToShaderSet[ id ].shaderEffect = shaderEffect;

    eventHandler.selectActor(actor); // redoes property tree

    ret = true;
  } else {
    console.log("Cannot set shader");
  }
  return ret;
};

var checkAndUpdateShader = function() {
  "use strict";
  if(checkShader()) {
    var actor = eventHandler.touchedActor;

    if(actor) {
      var animationInfo = animationList[animationSelectionIndex];
      var animation = animationInfo.animation;
      var progress;
      if(animation) {
        progress = animation.getCurrentProgress();
        animation.stop();
        animationInfo.dirtyData = true; // trigger reload on playAnimation
      }

      setShader(actor);

      if(animation) {
        playAnimation();
        if(animationInfo.animation) {
          animationInfo.animation.setCurrentProgress(progress); // where we left off
        }
      }
    }
  }
};

var getShader = function(actor) {
  "use strict";
  if(actor == null) {
    actor = eventHandler.touchedActor;
  }

  if( actor.getId() in actorIdToShaderSet) {
    if("shaderEffect" in actorIdToShaderSet[actor.getId()]) {
      return actorIdToShaderSet[actor.getId()].shaderEffect;
    }
  }
  return null;
};

var getAnimation = function() {
  "use strict";
  return animationList[animationSelectionIndex].animation;
};

var getActor = function() {
  "use strict";
  return eventHandler.touchedActor;
};


//------------------------------------------------------------------------------
// test functions
//------------------------------------------------------------------------------

function test_hierarchy() {
  "use strict";
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
  assert(directChildren[0].getId() === hello.getId());

  actor.position = [100, 100, 0];

  var root = dali.stage.rootRotationActor;

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
}

function test_registerPropery() {
  "use strict";
  console.log("test_registerPropery...");
  var s = dali.stage;
  var root = s.rootRotationActor;

  var another = new dali.TextActor();
  another.parentOrigin = [0.5, 0.5, 0.5];
  another.anchorPoint = [0.5, 0.5, 0.5];
  another.text = "peppa";
  another.name = another.text;
  another.size = [100, 100, 1];
  another.position = [-50, 100, 0];
  root.add(another);

  // var imageText = addTextImageFromCanvas("Colour");
  // imageText.connectSignal("touched", onTouched);

  var c = root.getChildAt(root.getChildCount() - 1);
  //var n = c.getChildCount();
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
}

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
}

function test_getset() {
  "use strict";
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
  console.log("  -> ok test_getset");
};

function test_animation_spline() {
  "use strict";
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
  a.animatePath(actor, path, [1, 0, 0], "Linear", 0, 3);
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
  "use strict";
  console.log("test_animation...");
  threeSquares();
  var col = {};
  collectByName(col);
  var actor = col.red;

  var a = new dali.Animation();
  a.setDuration(3);
  a.animateTo(actor, "position", [20, 0, 0], "Linear", 0, 3);
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
    a.animateBy(actor, "position", [20, 0, 0], "Linear", 0, 3);
    a.play();

    window.setTimeout(checkNewPos, 4000);
  }

  window.setTimeout(checkPos, 4000);
}

function test_shadereffect1() {
  "use strict";
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
  a.animateTo(shader, "weight", final, "Linear", 0, 3);
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
  "use strict";
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


function demo() {
  "use strict";

  //addNewAnimation();

  var properties = animationList[animationSelectionIndex];

  for(var i = 0; i < shaderSources.length; i++) {
    var options = shaderSources[i];
    if("animateTo" in options) {
      var img = addTestImage(3);
      selectActor(img.getId());
      loadShader(i);
      img.name = img.name + "-" + options.name;
      for(var j = 0; j < options.animateTo.length; j++) {
        var animateToOptions = options.animateTo[j];
        //addPropertyToAnimation(img.getId(), animateToOptions[0]); // name

        animationList[animationSelectionIndex].properties.push( {actor: img,
                                                                 property: animateToOptions[0],
                                                                 value: animateToOptions[1],
                                                                 interpolation: animateToOptions[2],
                                                                 delay: animateToOptions[3],
                                                                 duration: animateToOptions[4]} );
        animationList[animationSelectionIndex].dirtyData = true;

        // var prop = properties.properties[ properties.properties.length - 1 ];
        // prop.value = animateToOptions[1];
        // prop.interpolation = animateToOptions[2];
        // prop.delay = animateToOptions[3];
        // prop.duration = animateToOptions[4];

      }
      selectActor(null);
    }
  }

  $("#loopOn")[0].click();
  playAnimation();
}

//------------------------------------------------------------------------------
// regression test
//------------------------------------------------------------------------------

function regression_test() {
  "use strict";
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
  "use strict";
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
  "use strict";
  var shader = new dali.ShaderEffect({
    vertex: shaderSource2.vertex,
    fragment: shaderSource2.fragment
  });

  actor.setShaderEffect(shader);

  var final = [5, 5, 5, 1];

  var a = new dali.Animation();
  a.setDuration(3);
  a.setLooping(true);
  a.animateTo(shader, "weight", final, "Linear", 0, 3);

  a.play();

  a.delete();

}


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

function defaultImageSetup(imageActor, filename) {
  "use strict";
  var i = imageActor;
  i.visible = true;
  i.anchorPoint = [0.5, 0.5, 0.5];
  i.parentOrigin = [0.5, 0.5, 0.5];
  i.position = [0, 0, -10];
  i.size = [100, 100, 1];
  i.name = filename.replace(/-/g, "_").replace(/\./g, "_");
}

function createImageRGBAFromImageData(filename, imageData) {
  "use strict";
  var i = new dali.ImageActor();
  i.setImageData(imageData); // decoded RGBA data
  defaultImageSetup(i, filename);
  return i;
}

function addImageActorRGBA(filename, imageData) {
  "use strict";
  var i = createImageRGBAFromImageData(filename, imageData);
  app.addActor(i);
}

function asyncFileReaderGetImageData(file, callback) {
  "use strict";
  if (!file) {
    return;
  }

  var img = new Image();

  var objectUrl = window.URL.createObjectURL(file);

  img.onload = function( //e
    ) {
      var canvas = document.createElement("canvas");
      canvas.width = img.width; // naturalWidth;
      canvas.height = img.height; // naturalHeight;
      var context = canvas.getContext("2d");
      context.drawImage(img, 0, 0 );
      var imageData = context.getImageData(0, 0, img.naturalWidth, img.naturalHeight); // <-ImageData
      callback(file.name, imageData);
      window.URL.revokeObjectURL(objectUrl);
    };

  img.src = objectUrl;

}

fileInputCanvasDecorder.addEventListener("change", function() {
  "use strict";

  var file = fileInputCanvasDecorder.files[0];

  asyncFileReaderGetImageData(file, addImageActorRGBA);
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

btnShaderSourceC.addEventListener("click", function(// e
) {
  "use strict";
  var i;
  var info = getShadersFromUi();

  var data = "<pre><code>" +
        "#define MAKE_STRING(A)#A\n\n" +
        "std::string vertexShader = MAKE_STRING(\n";

  var lines = info.vertex.split("\n");
  for(i = 0; i < lines.length; i++) {
    data += lines[i] + "\\n" + "\n";
  }
  data += ");\n\n";

  data += "std::string fragShader = MAKE_STRING(\n";

  lines = info.fragment.split("\n");
  for(i = 0; i < lines.length; i++) {
    data += lines[i] + "\\n" + "\n";
  }
  data += ");\n\n";

  data += "ShaderEffect shaderEffect;\n" +
    "shaderEffect = ShaderEffect::New( vertexShader, fragmentShader,\n" +
    "                                GeometryType( " + info.geometryType + ")\n" +
    "                                ShaderEffect::GeometryHints( " + info.geometryHints + " ) )\n";

  data += "</code><pre>";

  var myWindow = window.open("data:text/html," + encodeURIComponent(data));
  //                             "_blank");  // , "width=00,height=100");
  myWindow.focus();

});

btnShaderSourceJS.addEventListener("click", function(// e
) {
  "use strict";
  var i;
  var info = getShadersFromUi();

  var data = "<pre><code>" +
        "var shaderOptions = {vertex:\n";

  var lines = info.vertex.split("\n");
  for(i = 0; i < lines.length; i++) {
    data += "                     \"" + lines[i] + "\\n\" +\n";
  }
  data += "                     \"\",\n";

  data += "                     fragment:\n";
  lines = info.fragment.split("\n");
  for(i = 0; i < lines.length; i++) {
    data += "                     \"" + lines[i] + "\\n\" +\n";
  }
  data += "                     \"\",\n";

  data += "                     geometryType: \"" + info.geometryType + "\",\n";
  data += "                     geometryHints: \"" + info.geometryHints + "\"\n";
  data += "                     };\n";

  data += "</code><pre>";

  var myWindow = window.open("data:text/html," + encodeURIComponent(data));
  //                             "_blank");  // , "width=00,height=100");
  myWindow.focus();

});

btnShaderSourceJSON.addEventListener("click", function(// e
) {
  "use strict";
  var info = getShadersFromUi();

  var type = "";
  if(info.geometryType === "image") {
    type = "GEOMETRY_TYPE_IMAGE";
  }
  if(info.geometryType === "text") {
    type = "GEOMETRY_TYPE_TEXT";
  }
  if(info.geometryType === "untexturedMesh") {
    type = "GEOMETRY_TYPE_UNTEXTURED_MESH";
  }
  if(info.geometryType === "texturedMesh") {
    type = "GEOMETRY_TYPE_TEXTURED_MESH";
  }

  var hints = ""; // tbd

  var vertex = info.vertex.replace(/\n/g, "\\n");
  var fragment = info.fragment.replace(/\n/g, "\\n");

  var data = "<pre><code>" +
        "{\"shader-effects\": {\n" +
        "  \"" + "effect" + "\": {\n" +
        "    \"program\": {\n" +
        "    \"vertexPrefix\": \"\",\n" +
        "    \"vertex\": \"" + vertex + "\",\n" +
        "    \"fragmentPrefix\": \"\",\n" +
        "    \"fragment\": \"" + fragment + "\",\n" +
        "    \"geometry-type\": \"" + type + "\"\n" +
        "    },\n" +
        "    \"geometry-hints\": \"" + hints + "\",\n" +
        "    \"grid-density\": \"" + "0" + "\",\n" +
        "    \"image\": {\n" +
        "      \"filename\": \"\"\n" +
        "    }\n" +
        "  }\n" +
        " }\n" +
        "}\n";

  var myWindow = window.open("data:text/html," + encodeURIComponent(data));
//                             "_blank");  // , "width=00,height=100");
  myWindow.focus();

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

  a.size = [Number($("#solidActorWidth")[0].value), Number($("#solidActorHeight")[0].value), 1];

  app.addActor(a);
});

window.setTimeout(init, 1000);

function updateStatistics(eventHandler){
  "use strict";

  var elem = $("#statistics")[0];

  var actor = eventHandler.touchedActor;

  var usedRootLayer = false;

  if(!actor) {
    usedRootLayer = true;
    actor = dali.stage.getRootLayer();
  }

  var rt = dali.stage.getRenderTaskList().getTask(0);

  var xy;
  try {
    xy = dali.screenToLocal(eventHandler.mouse_x, eventHandler.mouse_y,
                            actor, rt);
  } catch(err) {
    xy = [0, 0];
  }

  var screenxy = dali.worldToScreen(actor.position, rt);

  var name = "Actor";

  if(usedRootLayer) {
    name = "Root Actor";
  }

  var prec = 3;
  elem.innerHTML = name + " (" + actor.getId() + ") '" + actor.name + "'" + "<br>" +
    "Screen:" + eventHandler.mouse_x.toFixed(prec) + "," + eventHandler.mouse_y.toFixed(prec) + "<br>" +
    name + " Local:" + xy[0].toFixed(prec) + "," + xy[1].toFixed(prec) + "<br>" +
    name + " To Screen:" + screenxy[0].toFixed(prec) + "," + screenxy[1].toFixed(prec) + "<br>";

  if(usedRootLayer) { // dont delete eventHandler object
    actor.delete(); // wrapper
  }

  rt.delete(); // wrapper

}


$(".btn-toggle").click(function() {
  "use strict";
  $(this).find(".btn").toggleClass("active");

  if($(this).find(".btn-primary").size() > 0) {
    $(this).find(".btn").toggleClass("btn-primary");
  }

  $(this).find(".btn").toggleClass("btn-default");

  if(this.id === "loop") {
    var looping = $(this).find("#loopOn").hasClass("active");
    animationList[animationSelectionIndex].looping = looping;
    animationList[animationSelectionIndex].dirtyData = true;
  }

  if(this.id === "endAction") {
    var bake = $(this).find("#endBake").hasClass("active");
    if(bake) {
      animationList[animationSelectionIndex].endAction = "Bake";
    } else {
      animationList[animationSelectionIndex].endAction = "Discard";
    }
    animationList[animationSelectionIndex].dirtyData = true;
  }

});

function rebuildShaderDropdown()
{
  "use strict";
  //
  // shader source drop down
  //
  var e = document.getElementById("shaderDropDown");
  var count = e.children.length;
  for (var i = 0; i < count; i++) {
    e.removeChild(e.children[0]);
  }
  var li;
  var a;
  for(i = 0; i < shaderSources.length; i++) {
    li = document.createElement("li");
    a = document.createElement("a");
    a.href = "javascript:loadShader(" + i + ")";
    a.text = shaderSources[i].name;
    li.appendChild(a);
    e.appendChild(li);
  }

  // li = document.createElement("li");
  // li.className = "divider";
  // e.appendChild(li);

  // li = document.createElement("li");
  // a = document.createElement("a");
  // a.href = "javascript:loadShader(-1)";
  // a.text = "Shader for current selected actor"; //shaderSources[0].name;
  // li.appendChild(a);
  // e.appendChild(li);
}

function init() {
  "use strict";

  // var root = dali.stage.getRootLayer();
  // root.name = "*" + root.name; // * at start means non selectable by convention
  // root.delete(); // wrapper

  rebuildShaderDropdown();

  var editor = ace.edit("editorVertex");
  editor.getSession().addEventListener("change", function() {
    "use strict";
    if(userEditingShader) {
      checkAndUpdateShader();
    }
  });

  editor = ace.edit("editorFragment");
  editor.getSession().addEventListener("change", function() {
    "use strict";
    if(userEditingShader) {
      checkAndUpdateShader();
    }
  });

  var shaderTab = $("#tab2primary")[0];
  var lastClassName = shaderTab.className;
  window.setInterval( function() {
    var className = shaderTab.className;
    if (className !== lastClassName) {
      //
      // an attempt to get the editboxes to display the correct content.
      // when you setValue() the content and they aren't visible then
      // they dont update properly until you click in the box
      //
      var e = ace.edit("editorVertex");
      e.setShowInvisibles(true);
      e.setShowInvisibles(false);

      e = ace.edit("editorFragment");
      e.setShowInvisibles(true);
      e.setShowInvisibles(false);
      lastClassName = className;
    }
  }, 1);

  addNewAnimation();    // add at least one animation

  var dropdown = $("#geometryTypeDropDown")[0];
  var li = document.createElement("li");
  var a = document.createElement("a");
  a.href = "javascript:selectGeometryType(0)"
  a.text = "Geometry:Image";
  a.id = "geometryImage";
  // a.className = "geometry-tooltip";
  //a.setAttribute("data-toggle", "tooltip");
  a.setAttribute("rel", "popover");
  a.setAttribute("data-original-title", "VERTEX:\n" + dali.prepends.image.vertex + "\nFRAGMENT:\n" + dali.prepends.image.fragment);
  a.setAttribute("data-placement", "right");
  li.appendChild(a);
  dropdown.appendChild(li);

  li = document.createElement("li");
  a = document.createElement("a");
  a.href = "javascript:selectGeometryType(1)";
  a.text = "Geometry:Text";
  a.id = "geometryText";
  // a.setAttribute("data-toggle", "tooltip");
  // a.setAttribute("data-original-title", dali.prepends.text);
  a.setAttribute("rel", "popover");
  a.setAttribute("data-original-title", "VERTEX:\n" + dali.prepends.text.vertex + "\nFRAGMENT:\n" + dali.prepends.text.fragment);
  a.setAttribute("data-placement", "right");
  li.appendChild(a);
  dropdown.appendChild(li);

  li = document.createElement("li");
  a = document.createElement("a");
  a.href = "javascript:selectGeometryType(2)";
  a.text = "Geometry:Untextured Mesh";
  a.id = "geometryUntexturedMesh";
  // a.setAttribute("data-toggle", "tooltip");
  // a.setAttribute("data-original-title", dali.prepends["untextured mesh"]);
  a.setAttribute("rel", "popover");
  a.setAttribute("data-original-title", "VERTEX:\n" + dali.prepends["untextured mesh"].vertex + "\nFRAGMENT:\n" + dali.prepends["untextured mesh"].fragment);
  a.setAttribute("data-placement", "right");
  li.appendChild(a);
  dropdown.appendChild(li);

  li = document.createElement("li");
  a = document.createElement("a");
  a.href = "javascript:selectGeometryType(3)";
  a.text = "Geometry:Textured Mesh";
  a.id = "geometryTexturedMesh";
  // a.setAttribute("data-toggle", "tooltip");
  // a.setAttribute("data-original-title", dali.prepends["textured mesh"]);
  a.setAttribute("rel", "popover");
  a.setAttribute("data-original-title", "VERTEX:\n" + dali.prepends["textured mesh"].vertex + "\nFRAGMENT:\n" + dali.prepends["textured mesh"].fragment);
  a.setAttribute("data-placement", "right");
  li.appendChild(a);
  dropdown.appendChild(li);


  $('a[rel=popover]').tooltip();

  Object.defineProperty(dali, "shader", {
    enumerable: true,
    configurable: false,
    get: function() {
      return getShader();
    }
  });

  Object.defineProperty(dali, "actor", {
    enumerable: true,
    configurable: false,
    get: function() {
      return getActor();
    }
  });

  Object.defineProperty(dali, "animation", {
    enumerable: true,
    configurable: false,
    get: function() {
      return getAnimation();
    }
  });

  eventHandler.handlersMouseMove.push(updateStatistics);

}

