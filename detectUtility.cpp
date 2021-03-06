#include "detectUtility.h"
using namespace cv;
using namespace std;

//Init
std::vector<int> red;
std::vector<int> orange;
std::vector<int> yellow;
std::vector<int> green;
std::vector<int> blue;
std::vector<int> purple;
float webcamAngleApertureX;// 1 pixel width ~= webcamAngleApertureX radians
float webcamAngleApertureY;// 1 pixel height ~= webcamAngleApertureY radians

/* Init predifined threshold values for each colors and webcamAngleApertures*/
void initHsvColors(){
    red.push_back(165);//low H
    red.push_back(145);//low S
    red.push_back(55);//low V
    red.push_back(179);//high H
    red.push_back(255);//high S
    red.push_back(255);//high V
    orange.push_back(0);
    orange.push_back(140);
    orange.push_back(96);
    orange.push_back(34);
    orange.push_back(255);
    orange.push_back(255);
    yellow.push_back(19);
    yellow.push_back(207);
    yellow.push_back(212);
    yellow.push_back(30);
    yellow.push_back(255);
    yellow.push_back(255);
    green.push_back(28);
    green.push_back(76);
    green.push_back(134);
    green.push_back(48);
    green.push_back(142);
    green.push_back(183);
    blue.push_back(75);
    blue.push_back(145);
    blue.push_back(55);
    blue.push_back(130);
    blue.push_back(255);
    blue.push_back(255);
    purple.push_back(130);
    purple.push_back(145);
    purple.push_back(55);
    purple.push_back(160);
    purple.push_back(255);
    purple.push_back(255);
    webcamAngleApertureX =2*atan(20.0/(2.0*24));
    webcamAngleApertureY =2*atan(15.0/(2.0*23));
}


/* This function return true if a Point of contour 1 is at less than
   dist_is_close of a Point of contour 2 */
bool find_if_close(vector<Point>  cnt1,vector<Point>  cnt2, float dist_is_close){
    float dist = 0 ;
    bool close = false;
    for(int k=0; k< (int)cnt1.size(); k++){
        for(int l=0; l< (int)cnt2.size(); l++){
            dist = sqrt(pow(cnt1[k].y-cnt2[l].y,2)+pow(cnt1[k].x-cnt2[l].x,2));
            if(dist < dist_is_close){
                close = true;
            }
        }
    }
    return close;
}

/* This function merge two contours and calculate the smallest rectangle
   containing this merged contour. In order to be drawable the function
    convexHull is used to sort the Points in a clockwise order.*/
vector<Point> merge2Contours(vector<Point>  cnt1,vector<Point>  cnt2){
    vector<Point> mContour = cnt1;
    for(int l=0; l< (int)cnt2.size(); l++){
        mContour.push_back(cnt2[l]);
    }
    vector<Point> hull;
    convexHull(Mat(mContour),hull);
    Mat hull_points(hull);
    return hull;
}

/* This function merge the contours which are close and returns a new vector
    of contours.*/
vector<vector<Point> > mergeAllContours(vector<vector<Point> > contours,
                                float const& dist_is_close){

    vector<vector<Point> > contoursBis;
    vector<vector<Point> > mergedContours;
    int size =(int)contours.size();
    int q =0;
    bool play= true;
    while(play){
        contoursBis.erase(contoursBis.begin(),contoursBis.end());
        while(q < size){

            if(q!=0 && find_if_close(contours[0],contours[q], dist_is_close)){
                contours[0]=merge2Contours(contours[0],contours[q]);
            }
            else{
                if(q!=0){
                    contoursBis.push_back(contours[q]);
                }
            }
            q+=1;
      }
        if(contoursBis==contours){
            play = false;
        }
        else{
            mergedContours.push_back(contours[0]);
            contours=contoursBis;
            if( contours.size()==1){
                mergedContours.push_back(contours[0]);
                return mergedContours;
            }
            size =(int)contours.size();
            q=0;
        }
    }
    return mergedContours;
}
/*This function print a vector of contour */
void printContour(vector<vector<Point> > ct){
    for(int i = 0; i < (int)ct.size(); i++ ){
        cout <<"" << endl;
        for(int j = 0; j < (int)ct[i].size(); j++ ){
            cout << ct[i][j] << endl;
        }
    }
}

/*This function associate each color input to the corresponding HSV threshold
  values and drawing colors values*/
vector<vector<int> > findHsvTreshold(vector<string> colors, vector<Scalar>& colorDrawing){
    vector<vector<int> > hsvValues;

    string color = "red";
    for(int i = 0; i < colors.size();i ++){
        color = colors[i];
        if(color.compare("red")==0){
            hsvValues.push_back(red);
            colorDrawing.push_back(Scalar(0,0,255));
        }
        else if(color.compare("orange")==0){
            hsvValues.push_back(orange);
            colorDrawing.push_back(Scalar(0,140,255));
        }
        else if(color.compare("yellow")==0){
            hsvValues.push_back(yellow);
            colorDrawing.push_back(Scalar(82,255,255));
        }
        else if(color.compare("green")==0){
            hsvValues.push_back(green);
            colorDrawing.push_back(Scalar(0,255,0));
        }
        else if(color.compare("blue")==0){
            hsvValues.push_back(blue);
            colorDrawing.push_back(Scalar(200,40,30));
        }
        else if(color.compare("purple")==0){
            hsvValues.push_back(purple);
            colorDrawing.push_back(Scalar(200,30,150));
        }
    }
    return hsvValues;
}

/*This function threshold an image with the HSV threshold values contained in
  color*/
Mat threshold(Mat const& imgOriginal,vector<int> const& color){
    int iLowH = color[0];
    int iHighH = color[3];

    int iLowS = color[1];
    int iHighS = color[4];

    int iLowV = color[2];
    int iHighV = color[5];

    Mat imgHSV;
    cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

    Mat imgThresholded;
    inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
return imgThresholded;
}


void morphologicalOperations (Mat& imgThresholded){
    //morphological opening (removes small objects from the foreground)
    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
    dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

    //morphological closing (removes small holes from the foreground)
    dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

}

/*Find the contours , bounding rectangles and centers of the detected obstacles */
void computeContoursCentersRectangles(Mat const& imgThresholded,vector<Point2f>& mc,vector<Rect>& rotated_bounding_rects, int minAreaToDetect  ){

    //findContours
    Mat cpImgThresholded;
    imgThresholded.copyTo(cpImgThresholded);
    Mat imgContours;
    vector<vector<Point> > contours;
    vector<vector<Point> > contoursMerged;
    Rect rotated_bounding_rect;
    vector<Point> contourMerged;
    vector<Vec4i> hierarchy;

    findContours(cpImgThresholded, contours, hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    contoursMerged=mergeAllContours(contours, 150);
    for(int i = 0; i<(int)contoursMerged.size(); i++){
        rotated_bounding_rects.push_back(boundingRect(contoursMerged[i]));
    }
    /// Get the moments
    vector<Moments> mu(contoursMerged.size() );
    vector<Point2f> mc1( contoursMerged.size() );
    mc=mc1;
    double dArea = 0;
    for( int i = 0; i < contoursMerged.size(); i++ ){
        mu[i] = moments( contoursMerged[i], false );
        dArea=mu[i].m00;
        if (dArea > minAreaToDetect){// if the area <= 20000, I consider that the there are no object in the image and it's because of the noise, the area is not zero
            mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );///  Get the mass centers:
        }
        else{
            rotated_bounding_rects.erase(rotated_bounding_rects.begin()+i);
        }
    }
}

void drawCentersRectangles(Mat& imgOriginal,vector<Point2f> const& mc,vector<Rect> const& rotated_bounding_rects,Scalar const& aColorDrawing ){
    Point2f rect_points[4];
    for(int i = 0; i<(int)rotated_bounding_rects.size(); i++ ){
        circle( imgOriginal, mc[i], 10, aColorDrawing, 1, 8, 0 );
        rectangle(imgOriginal, rotated_bounding_rects[i], aColorDrawing,4, 8,0);
    }
}
/* Merge the bounding rectangles of each frames */
vector<Rect> compareRects(cv::Mat& imgOriginal,std::vector<cv::Rect> const& rotated_bounding_rects){

    vector<Rect> mergedRects, rectsBis, rotated_bounding_rects_copy ;
    rotated_bounding_rects_copy=rotated_bounding_rects;
    Point center0, centerQ,mergedTopLeftCorner;
    cv::Size size0, sizeQ, mergedSize;
    int size =(int)rotated_bounding_rects_copy.size();
    int q =0;
    float dist=0;
    bool play= true;
    //cout <<"BEGIN "<< size0 << endl;
    while(play){
        rectsBis.erase(rectsBis.begin(),rectsBis.end());
        while(q < size){
            size0=rotated_bounding_rects_copy[0].size();
            sizeQ=rotated_bounding_rects_copy[q].size();
            dist=max(max(size0.width,sizeQ.width),max(size0.height,sizeQ.height));
            //cout <<" size0 "<< size0 << endl;
            //cout << "rotated_bounding_rects_copy[0].tl() " << rotated_bounding_rects_copy[0].tl() <<endl;
            //cout <<" sizeQ "<< sizeQ << endl;
            //cout << "rotated_bounding_rects_copy[0].tl() " << rotated_bounding_rects_copy[q].tl() <<endl;
            center0.x=(rotated_bounding_rects_copy[0].tl().x + size0.width/2.0);
            center0.y=(rotated_bounding_rects_copy[0].tl().y + size0.height/2.0);
            centerQ.x=(rotated_bounding_rects_copy[q].tl().x + sizeQ.width/2.0);
            centerQ.y=(rotated_bounding_rects_copy[q].tl().y + sizeQ.height/2.0);

            if(q!=0 && (sqrt(pow(center0.x-centerQ.x,2)+pow(center0.y-centerQ.y,2))<=dist)){
                    mergedSize.height=(size0.height+sizeQ.height)/2.0;
                    mergedSize.width=(size0.width+sizeQ.width)/2.0;
                    mergedTopLeftCorner.x= (rotated_bounding_rects_copy[0].tl().x + rotated_bounding_rects_copy[q].tl().x )/2.0;
                    mergedTopLeftCorner.y= (rotated_bounding_rects_copy[0].tl().y + rotated_bounding_rects_copy[q].tl().y )/2.0;
                    rotated_bounding_rects_copy[0]=Rect(mergedTopLeftCorner.x,mergedTopLeftCorner.y,mergedSize.width,mergedSize.height);
                    //cout <<" aaaaaaa" << endl;
                    //cout <<  rotated_bounding_rects_copy[0].size() << endl;
                    //cout <<"" << endl;
                }
            else{
                    if(q!=0){
                        rectsBis.push_back(rotated_bounding_rects_copy[q]);
                    }
                }
            q+=1;
      }
        if(rectsBis==rotated_bounding_rects_copy){
            play = false;
        }
        else{
            mergedRects.push_back(rotated_bounding_rects_copy[0]);
            rotated_bounding_rects_copy=rectsBis;
            if( rotated_bounding_rects_copy.size()==1){
                mergedRects.push_back(rotated_bounding_rects_copy[0]);
                return mergedRects;
            }
            size =(int)rotated_bounding_rects_copy.size();
            q=0;
        }
    }
    return mergedRects;
 }
 /*Find the centers of a list of rectangles */
vector<Point> findRectanglesCenters(std::vector<cv::Rect> const& rects){
     vector<Point> centerList;
     Point center;
     Size size;
     for(int i=0; i<(int)rects.size(); i++ ){
         size=rects[i].size();
         center.x=(int) (rects[i].tl().x + size.width/2.0);
         center.y=(int) (rects[i].tl().y + size.height/2.0);
         centerList.push_back(center);
     }
     return centerList;
 }
 /*Suppress rectangles with an area < minAreaRectangle */
 void supressSmallRectangles(vector<Rect>& rects,int minAreaRectangle){
     vector<Rect> newRectList;
     for (int i=0; i<(int) rects.size(); i++){
         if(rects[i].area()>minAreaRectangle){
             newRectList.push_back(rects[i]);
         }
     }
     rects=newRectList;
 }
  /*For the node architecture, create compute the angular position of te obstacles*/
void computeObstaclesAnglePosition(cv::Mat const& imgOriginal, std::vector<ObstacleData>& Obstacles,std::vector<std::vector<cv::Rect> > rotated_bounding_rects_several_captures ){
    ObstacleData currentObstacle;
    Size imageSize=imgOriginal.size(), rectangleSize;
    Point topLeftCorner, leftPoint, rightPoint, imageCenter(imageSize.width/2.0,imageSize.height/2.0);
    int imgHeight=imageSize.height;
    int imgWidth=imageSize.width;
    float webcamAngleApertureXPerPixel = webcamAngleApertureX/imageSize.width;
    float webcamAngleApertureYPerPixel = webcamAngleApertureY/imageSize.height;
    for(int i = 0; i<(int)rotated_bounding_rects_several_captures.size(); i++){
        for(int j = 0; j<(int)rotated_bounding_rects_several_captures[i].size(); j++){
            rectangleSize=rotated_bounding_rects_several_captures[i][j].size();
            topLeftCorner=rotated_bounding_rects_several_captures[i][j].tl();
            leftPoint.x = topLeftCorner.x;
            leftPoint.y = topLeftCorner.y +rectangleSize.height/2.0;
            rightPoint.x = topLeftCorner.x+rectangleSize.width;
            rightPoint.y = topLeftCorner.y +rectangleSize.height/2.0;
            currentObstacle.LeftBoundheadingRelativeToBoat = (-imageCenter.x+leftPoint.x)*webcamAngleApertureXPerPixel;
            currentObstacle.RightBoundheadingRelativeToBoat = (-imageCenter.x+rightPoint.x)*webcamAngleApertureXPerPixel;
            currentObstacle.minDistanceToObstacle=0;
            currentObstacle.maxDistanceToObstacle=-1;
            Obstacles.push_back(currentObstacle);
            }
    }
}
void printObstacleVector(std::vector<ObstacleData> v){
    cout << "Obstacle vector begin" << endl;
    for(int i = 0; i<(int)v.size(); i++){
        cout << "" << endl;
        cout << "LeftBoundheadingRelativeToBoat" << v[i].LeftBoundheadingRelativeToBoat <<endl;
        cout << "RightBoundheadingRelativeToBoat" << v[i].RightBoundheadingRelativeToBoat <<endl;
        cout << "minDistanceToObstacle" << v[i].minDistanceToObstacle <<endl;
        cout << "maxDistanceToObstacle" << v[i].maxDistanceToObstacle <<endl;
    }
    cout << "Obstacle vector end" << endl;
    cout << "" << endl;
}
