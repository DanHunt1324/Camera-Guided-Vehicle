/****************************
  This include file, index_OCV_ColorTrack.h, the Client, is an intoduction of OpenCV.js to the ESP32 Camera environment. The Client was
  developed and written by Andrew R. Sass. Permission to reproduce the index_OCV_ColorTrack.h file is granted free of charge if this
  entire copyright notice is included in all copies of the index_OCV_ColorTrack.h file.

  Complete instructions at https://RandomNerdTutorials.com/esp32-cam-opencv-js-color-detection-tracking/
*******************************/
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
   <title>CAMERA GUIDED VEHICLE</title>
   <meta charset="utf-8">
   <meta name="viewport" content="width=device-width,initial-scale=1">
   <!----ANN:3--->
   <script async src=" https://docs.opencv.org/master/opencv.js" type="text/javascript"></script>
</head>
<style>
html {
    font-family: Arial, Helvetica, sans-serif;
    }
body {
    background-color:  #FFFAFA;
    margin: 0px;
}
h1 {
    font-size: 1.6rem;
    color:white;
    text-align: center;
}
.topnav {
    overflow: hidden;
    background-color: #1ac6ff;
}
.main-controls{
  padding-top: 5px;
}
h2 {
    color: #0A1128;
    font-size: 1rem;
}
.section {
    margin: 2px;
    padding: 10px;
}
.column{
    float: left;
    width: 50%
}
table {
    margin: 0;
    width: 90%;
    border-collapse: collapse;
}
th{
    text-align: center;
}
.row{
    margin-right:50px;
    margin-left:50px;
}

#track{ // Button to start tracking
    border: none;
    color: #FEFCFB;
    background-color: #0A1128;
    padding: 15px;
    text-align: center;
    display: inline-block;
    font-size: 24px;
    border-radius: 4px;
}
#restart{ // Button to restart the ESP32 Module
    border: none;
    color: #FEFCFB;
    background-color: #7B0828;
    padding: 15px;
    text-align: center;
    display: inline-block;
    font-size: 24px;
    border-radius: 4px;
}
button{ // Generic button
    border: none;
    color: #FEFCFB;
    background-color: #0A1128;
    padding: 10px;
    text-align: center;
    display: inline-block;
    border-radius: 4px;
}

</style>
<body>
    <div class="topnav">
        <h1>ESP32-CAM Color Detection and Tracking</h1>
    </div>
    <div class="main-controls">
        <table>
            <tr>
                <td><center><input type="button" id="track" value="Object Tracking"></center></td>
                <td><center><input type="button" id="restart" value="Reset Board"></center></td>
            </tr>
        </table>
    </div>
<div class="container">
  <div class = "row">
    <div class = "column">
        <div class="section">
            <div class ="video-container">
                <h2>Video Streaming</h2>
                <center><img id="ShowImage" src="" style="display:none"></center>
                <center><canvas id="canvas" style="display:none"></canvas></center>
            </div>
        </div>

        <div class="section"><!-----ADDING IN A NEW SECTION FOR HSV MASK--->
            <h2>HSVcancas</h2>
           <center> <canvas id="HSVcanvas"></canvas></center>
        </div>
        <div class="section"><!-----ADDING IN A NEW SECTION FOR HSV MASK--->
            <h2>HSVcancas2</h2>
            <center><canvas id="HSVcanvas2"></canvas></center>
        </div>

    </div>   <!------endfirstcolumn---------------->

    <div class = "column">
        <div class="section">
            <h2>Image Mask</h2>
            <canvas id="imageMask"></canvas>
        </div>
        <div class="section">
            <h2>Image Canvas</h2>
            <canvas id="imageCanvas"></canvas>
        </div>


        <div class="section">
            <table>
                <tr>
                    <td><strong>XCM:</strong> <span id="XCMdemo"></span></td>
                    <td><strong>YCM:</strong> <span id="YCMdemo"></span></td>
                </tr>
            </table>
        </div>

        <div class="section">
            <canvas id="textCanvas" width="480" height="180" style= "border: 1px solid #black;"></canvas>
            <iframe id="ifr" style="display:none"></iframe>
            <div id="message"></div>
        </div>
        </div>  <!------end2ndcolumn------------------------>
  </div>   <!-----endrow---------------------->
</div>   <!------endcontainer-------------->
 <!--------------- </body>----------------->
 <!----------------</html>----------------->
<div class="modal"></div>
<script>
var colorDetect = document.getElementById('track');                   // Modified this to track
var ShowImage = document.getElementById('ShowImage');
var canvas = document.getElementById("canvas");
var context = canvas.getContext("2d");

var HSVcanvas = document.getElementById('HSVcanvas'); //ADDED
var HSVcontext = canvas.getContext("2d"); //ADDED
var HSVcanvas2 = document.getElementById('HSVcanvas2'); //ADDED
var HSVcontext2 = canvas.getContext("2d"); //ADDED

var imageMask = document.getElementById("imageMask");
var imageMaskContext = imageMask.getContext("2d");
var imageCanvas = document.getElementById("imageCanvas");
var imageContext = imageCanvas.getContext("2d");
var txtcanvas = document.getElementById("textCanvas");
var ctx = txtcanvas.getContext("2d");
var message = document.getElementById('message');
var ifr = document.getElementById('ifr');
var myTimer;
var restartCount=0;
const modelPath = 'https://ruisantosdotme.github.io/face-api.js/weights/';
let currentStream;
let displaySize = { width:400, height: 296 }
let faceDetection;


let b_tracker = true; // tracker has been set to true so can remove button as will always be on.
let x_cm = 0;
let y_cm = 0;

let b_contour = false; // Contour is not necessary and probably slows down performance.


//THESE VALUES ARE KEY as they determine the RBG range to be tracked

var RMAX=217;
var RMIN=30;
var GMAX=255;
var GMIN=150;
var BMAX=255;
var BMIN=180;
var THRESH_MIN=120;


colorDetect.onclick = function (event) { //When the button is pressed the image tracking begins

  clearInterval(myTimer);
  myTimer = setInterval(function(){error_handle();},5000);
  ShowImage.src=location.origin+'/?colorDetect='+Math.random();
}


//ANN:READY
var Module = {
  onRuntimeInitialized(){onOpenCvReady();}
}

function onOpenCvReady(){
  console.log("OpenCV IS READY!!!");
  drawReadyText(); // Displays the fact that opencv is ready to webpage canvas
  document.body.classList.remove("loading");
}


function error_handle() { // Handles any unexpected errors
  restartCount++;
  clearInterval(myTimer);
  if (restartCount<=2) {
    message.innerHTML = "Get still error. <br>Restart ESP32-CAM "+restartCount+" times.";
    myTimer = setInterval(function(){colorDetect.click();},10000);
    ifr.src = document.location.origin+'?restart';
  }
  else
    message.innerHTML = "Get still error. <br>Please close the page and check ESP32-CAM.";
}


colorDetect.style.display = "block";

ShowImage.onload = function (event) { // When image is found run the following...
  console.log("SHOW iMAGE");
  clearInterval(myTimer);
  restartCount=0;
  canvas.setAttribute("width", ShowImage.width);
  canvas.setAttribute("height", ShowImage.height);
  canvas.style.display = "block";
  imageCanvas.setAttribute("width", ShowImage.width);
  imageCanvas.setAttribute("height", ShowImage.height);
  imageCanvas.style.display = "block";


  // ADDED BY ME
  HSVcanvas.setAttribute("width", ShowImage.width);
  HSVcanvas.setAttribute("height", ShowImage.height);
  HSVcanvas.style.display = "block";

  HSVcanvas2.setAttribute("width", ShowImage.width);
  HSVcanvas2.setAttribute("height", ShowImage.height);
  HSVcanvas2.style.display = "block";
  //

  

  imageMask.setAttribute("width", ShowImage.width);
  imageMask.setAttribute("height", ShowImage.height);
  imageMask.style.display = "block";

  context.drawImage(ShowImage,0,0,ShowImage.width,ShowImage.height);

  DetectImage();
}


restart.onclick = function (event) {
  fetch(location.origin+'/?restart=stop');
}


async function DetectImage() { // Function that contains the cv model
  console.log("DETECT IMAGE");

  /***************opencv********************************/
  let src = cv.imread(ShowImage);
  arows = src.rows;
  acols = src.cols;
  aarea = arows*acols;
  adepth = src.depth();
  atype = src.type();
  achannels = src.channels();
  console.log("rows = " + arows);
  console.log("cols = " + acols);
  console.log("pic area = " + aarea);
  console.log("depth = " + adepth);
  console.log("type = " + atype);
  console.log("channels = " + achannels);

  /******************COLOR DETECT******************************/

  let tracker = 0;

  var XCMoutput = document.getElementById("XCMdemo");
  var YCMoutput = document.getElementById("YCMdemo");

  XCMoutput.innerHTML = 0;
  YCMoutput.innerHTML = 0;


  let M00Array = [0,];
  let orig = new cv.Mat();
  let mask = new cv.Mat();
  let mask1 = new cv.Mat();
  let mask2 = new cv.Mat();
  let contours = new cv.MatVector();
  let hierarchy = new cv.Mat();
  let rgbaPlanes = new cv.MatVector();

  let hsv = new cv.Mat();
  let hsvPlanes = new cv.MatVector();
  let hsvmask = new cv.Mat();
  let hsvmask1 = new cv.Mat();

  clear_canvas();






  orig = cv.imread(ShowImage);
  cv.split(orig,rgbaPlanes);  //SPLIT
  let BP = rgbaPlanes.get(2);  // SELECTED COLOR PLANE
  let GP = rgbaPlanes.get(1);
  let RP = rgbaPlanes.get(0);
  cv.merge(rgbaPlanes,orig);

  console.log("ISCONTINUOUS = " + orig.isContinuous());


  // Modifying the code to take hsv as the source image instead of source.
  var HMIN = 75;
  var HMAX = 120;
  var SMIN = 100;
  var SMAX = 255;
  var VMIN = 100; 
  var VMAX = 255;
  

  cv.cvtColor(src, hsv, cv.COLOR_BGR2HSV,0); // produces the image hsv chich is src converted
  cv.imshow('HSVcanvas', hsv); // displays the image in hsv on the html webpage
  


  let hsvhigh = new cv.Mat(hsv.rows,hsv.cols,hsv.type(),[HMAX,SMAX,VMAX,255]); //Setting the max values of hsv
  let hsvlow = new cv.Mat(hsv.rows,hsv.cols,hsv.type(),[HMIN,SMIN,VMIN,0]); //Setting the min values of hsv

  cv.inRange(hsv,hsvlow,hsvhigh,hsvmask1); // setting the range of hsv values that will be accepted
  cv.threshold(hsvmask1,hsvmask,120,255,cv.THRESH_BINARY);

  
  cv.imshow('HSVcanvas2', hsvmask);

  let high = new cv.Mat(src.rows,src.cols,src.type(),[RMAX,GMAX,BMAX,255]);
  let low = new cv.Mat(src.rows,src.cols,src.type(),[RMIN,GMIN,BMIN,0]);

  cv.inRange(src,low,high,mask1);
  //inRange(source image, lower limit, higher limit, destination image)
  cv.threshold(mask1,mask,THRESH_MIN,255,cv.THRESH_BINARY);
  //threshold(source image,destination image,threshold,255,threshold method);



/********************start contours******************************************/
  if(b_tracker == true){ // if tracking then find contours of object

    cv.findContours(mask,contours,hierarchy,cv.RETR_CCOMP,cv.CHAIN_APPROX_SIMPLE);
    //findContours(source image, array of contours found, hierarchy of contours
        // if contours are inside other contours, method of contour data retrieval,
        //algorithm method)
   }
    console.log("CONTOUR_SIZE = " + contours.size());  // Compare this to M00 for finding distance.

    //draw contours
    if(b_contour==true){
     for(let i = 0; i < contours.size(); i++){
        cv.drawContours(src,contours,i,[0,0,0,255],2,cv.LINE_8,hierarchy,100)
     }
    }

    let cnt;
    let Moments;
    let M00;
    let M10;



    for(let k = 0; k < contours.size(); k++){
        cnt = contours.get(k);
        Moments = cv.moments(cnt,false);
        M00Array[k] = Moments.m00;
       // cnt.delete();
    }


    let max_area_arg = MaxAreaArg(M00Array);
    console.log("MAXAREAARG = "+max_area_arg);



    let ArgMaxArea = MaxAreaArg(M00Array);
    if(ArgMaxArea > 0){
    cnt = contours.get(MaxAreaArg(M00Array));
    //cnt = contours.get(54);
    Moments = cv.moments(cnt,false);
    M00 = Moments.m00;
    M10 = Moments.m10;
    M01 = Moments.m01;
    x_cm = M10/M00;    // 75 for circle_9.jpg
    y_cm = M01/M00;    // 41 for circle_9.jpg

    XCMoutput.innerHTML = Math.round(x_cm);
    YCMoutput.innerHTML = Math.round(y_cm);

    console.log("M00 = "+M00);
    console.log("XCM = "+Math.round(x_cm));
    console.log("YCM = "+Math.round(y_cm));


    /* LOOKS LIKE THIS IS THE COMMAND THAT PASSES THE VALUE TO THE C++ CODE */

    //fetch(document.location.origin+'/?xcm='+Math.round(x_cm)+';stop');
    
   fetch(document.location.origin+'/?cm='+Math.round(x_cm)+';'+Math.round(y_cm)+';stop');

    //MY CODE

    fetch(document.location.origin+'/?M00='+M00+';'); //Gives the value of M00 through the console to tracker_vehicle.h



    console.log("M00ARRAY = " + M00Array);

    //**************min area bounding rect********************
    let rotatedRect=cv.minAreaRect(cnt);
    let vertices = cv.RotatedRect.points(rotatedRect);

    for(let j=0;j<4;j++){
        cv.line(src,vertices[j],
            vertices[(j+1)%4],[0,0,255,255],2,cv.LINE_AA,0);
    }
    //***************end min area bounding rect*************************************


    //***************bounding rect***************************
    let rect = cv.boundingRect(cnt);
    let point1 = new cv.Point(rect.x,rect.y);
    let point2 = new cv.Point(rect.x+rect.width,rect.y+rect.height);

    cv.rectangle(src,point1,point2,[0,0,255,255],2,cv.LINE_AA,0);
    //*************end bounding rect***************************


    //*************draw center point*********************
    let point3 = new cv.Point(x_cm,y_cm);
    cv.circle(src,point3,2,[0,0,255,255],2,cv.LINE_AA,0);
    //***********end draw center point*********************

    }//end if(ArgMaxArea >= 0)
    else{
      if(ArgMaxArea==-1){
        console.log("ZERO ARRAY LENGTH");
      }
      else{              //ArgMaxArea=-2
        console.log("Nothing detected");
      }
    }



  if(b_tracker==true){
     cv.imshow('imageMask', mask);
  }
  //cv.imshow('imageMask', R);
  cv.imshow('imageCanvas', src);

  src.delete();
  high.delete();
  low.delete();
  orig.delete();
  mask1.delete();
  mask2.delete();
  mask.delete();
  contours.delete();
  hierarchy.delete();
//  cnt.delete();
  RP.delete();
  hsv.delete();
  hsvmask.delete();
  hsvmask1.delete();
  hsvPlanes.delete();
  hsvhigh.delete();
  hsvlow.delete();




 /********************END COLOR DETECT****************************/

/***************end opencv******************************/


 setTimeout(function(){colorDetect.click();},10);
}
//end detectimage


function MaxAreaArg(arr){
    if (arr.length == 0) {
        return -1;
    }

    var max = arr[0];
    var maxIndex = 0;
    var dupIndexCount = 0; //duplicate max elements?

    if(arr[0] >= .90*aarea){
        max = 0;
    }

    for (var i = 1; i < arr.length; i++) {
        if (arr[i] > max && arr[i] < .99*aarea) {
            maxIndex = i;
            max = arr[i];
            dupIndexCount = 0;
        }
        else if(arr[i]==max && arr[i]!=0){
            dupIndexCount++;
        }
    }

    if(dupIndexCount==0){
        return maxIndex;
    }

    else{
        return -2;
    }
}//end MaxAreaArg



function clear_canvas(){
    ctx.clearRect(0,0,txtcanvas.width,txtcanvas.height);
    ctx.rect(0,0,txtcanvas.width,txtcanvas.height);
    ctx.fillStyle="white";
    ctx.fill();
}

function drawReadyText(){
    ctx.fillStyle = 'black';
    ctx.font = '20px serif';
    ctx.fillText('OpenCV.JS READY',txtcanvas.width/4,txtcanvas.height/10);
}

function drawColRowText(x,y){
    ctx.fillStyle = 'black';
    ctx.font = '20px serif';
    ctx.fillText('ImageCols='+x,0,txtcanvas.height/10);
    ctx.fillText('ImageRows='+y,txtcanvas.width/2,txtcanvas.height/10);
}


function drawXCM_YCM_Text(){
    ctx.fillStyle = 'black';
    ctx.font = '20px serif';
    ctx.fillText('XCM='+Math.round(x_cm),0,3*txtcanvas.height/10);
    ctx.fillText('YCM='+Math.round(y_cm),txtcanvas.width/4,3*txtcanvas.height/10);
}

function drawErrorTracking_Text(){
    ctx.fillStyle = 'black';
    ctx.font = '20px serif';
    ctx.fillText('ERROR TRACKING-NO CONTOUR',0,3*txtcanvas.height/10);
}

  </script>
</body>
</html>
)rawliteral";
