#include "detection.hpp"
#include "camera.hpp"
#include "logger.hpp"
#include <iostream>
//#include <opencv2/aruco.hpp>
#include <unistd.h>
#include <ctime>
#include <fstream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string.h>
#include <raspicam/raspicam_cv.h>
using namespace cv;
using namespace std;


detection::detection(int AireMax, int AireMin, int Xmax, int Xmin, int Ymin, int Ymax)
    : m_AireMax(AireMax), m_AireMin(AireMin), m_Xmax(Xmax), m_Xmin(Xmin), m_Ymin(Ymin), m_Ymax(Ymax) {
        m_cam = new camera();}

    

Mat detection::LireImage(){

  Mat image=m_cam->TakePic();
    
    // cout << "photo " << chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - m_begin_timestamp).count() << endl;
    // m_begin_timestamp = chrono::high_resolution_clock::now();

    // int indice=0;
    Mat image_sortie;

    resize(image,image,Size(128,128));
    Mat image_copy = image.clone();
    cvtColor(image,image, COLOR_BGR2GRAY);//On passe en nuance de gris
    threshold(image, image, 0 , 255,THRESH_OTSU);//



//Rotation de l'image
    Point centre(64,64);
    Mat rot_matrix= getRotationMatrix2D(centre,-1.5, 1);
    warpAffine(image,image, rot_matrix, image.size());


  
//On trouve les contours
    
    vector<vector<Point>> contours;//le contour est un vecteur de point
    vector<Vec4i> hierarchy;
    findContours(image, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
     
    

    int taille=contours.size();//Nombre de contours détectés
    int n[taille];
    for (int j = 0; j <taille; j++) {
        //cout<<"test"<<endl;
        if(contourArea(contours.at(j))<m_AireMax && contourArea(contours.at(j))>m_AireMin){
            //cout<<"contouraire"<<endl;
            n[j]=j;
        } else {
            n[j]=-1;
        }
    }

    int X[taille];
    int Y[taille];
//On calcul les coordonnées du rectangle de la zone détectée
    for (int k=0;k<taille;k++){
        if (n[k]!=-1){
            //cout<<"rect"<<endl;
            Rect box = boundingRect(contours[n[k]]);//Rectangle bordant le contour     
            X[k]=box.x+box.width/2;//Coordonnées du centre du rectangle
            Y[k]=box.y+box.height/2;
       } else {
            X[k]=-1;
            Y[k]=-1;
       }
    }

//On élimine les contours avec des coordonnées en Y trop petit ou trop grand
    for (int k=0;k<taille;k++){
        if (X[k]<m_Xmin || X[k]>m_Xmax || Y[k]>m_Ymax || Y[k]<m_Ymin){
            //cout<<"coord"<<endl;
            X[k]=-1;
            Y[k]=-1;
        }
    }
//Dessin des contours
    for (int k=0;k<taille;k++){
        if (X[k]!=-1 && Y[k]!=-1){
            drawContours(image_copy, contours, n[k], Scalar(0, 255, 0), 0);
            //On dessine le rectangle bordant le contour
            // indice=k;
            // cout<<"ok"<<endl;
            Rect box = boundingRect(contours[n[k]]);     
            rectangle(image_copy, box, Scalar(255, 0, 0));
            //On dessine la croix au centre du rectangle
            Point p1(X[k]-2,Y[k]);
            Point p2(X[k]+2,Y[k]);
            Point p3(X[k],Y[k]-2);
            Point p4(X[k],Y[k]+2);
            line(image_copy, p1, p2, Scalar(0, 0,255),1, LINE_8);
            line(image_copy, p3, p4, Scalar(0, 0,255),1, LINE_8);

        }
    }
    Point pt1(m_Xmin,m_Ymin);
    Point pt2(m_Xmax,m_Ymax);
    cv::rectangle(image_copy, pt1, pt2, cv::Scalar(0,0,255));

    // cout << "compute "<< chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - m_begin_timestamp).count() << endl;
    // cout<<" "<<endl;
    image_sortie=image_copy;
    return image_sortie;
}

detection::~detection() {
    delete m_cam;
}
  
 int detection::LirePosBille(){
    // chrono::time_point<chrono::high_resolution_clock> m_begin_timestamp = chrono::high_resolution_clock::now();



    Mat image=m_cam->TakePic();
    
    // cout << "photo " << chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - m_begin_timestamp).count() << endl;
    // m_begin_timestamp = chrono::high_resolution_clock::now();

    int indice=0;
    Mat image_sortie;

    resize(image,image,Size(128,128));
    Mat image_copy = image.clone();
    cvtColor(image,image, COLOR_BGR2GRAY);//On passe en nuance de gris
    threshold(image, image, 0 , 255,THRESH_OTSU);//



//Rotation de l'image
    Point centre(64,64);
    Mat rot_matrix= getRotationMatrix2D(centre,-1.5, 1);
    warpAffine(image,image, rot_matrix, image.size());


  
//On trouve les contours
    
    vector<vector<Point>> contours;//le contour est un vecteur de point
    vector<Vec4i> hierarchy;
    findContours(image, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
     
    

    int taille=contours.size();//Nombre de contours détectés
    int n[taille];
    for (int j = 0; j <taille; j++) {
        //cout<<"test"<<endl;
        if(contourArea(contours.at(j))<m_AireMax && contourArea(contours.at(j))>m_AireMin){
            //cout<<"contouraire"<<endl;
            n[j]=j;
        } else {
            n[j]=-1;
        }
    }

    int X[taille];
    int Y[taille];
//On calcul les coordonnées du rectangle de la zone détectée
    for (int k=0;k<taille;k++){
        if (n[k]!=-1){
            //cout<<"rect"<<endl;
            Rect box = boundingRect(contours[n[k]]);//Rectangle bordant le contour     
            X[k]=box.x+box.width/2;//Coordonnées du centre du rectangle
            Y[k]=box.y+box.height/2;
       } else {
            X[k]=-1;
            Y[k]=-1;
       }
    }

//On élimine les contours avec des coordonnées en Y trop petit ou trop grand
    for (int k=0;k<taille;k++){
        if (X[k]<m_Xmin || X[k]>m_Xmax || Y[k]>m_Ymax || Y[k]<m_Ymin){
            //cout<<"coord"<<endl;
            X[k]=-1;
            Y[k]=-1;
        }
    }
//Dessin des contours
    for (int k=0;k<taille;k++){
        if (X[k]!=-1 && Y[k]!=-1){
            //drawContours(image_copy, contours, n[k], Scalar(0, 255, 0), 0);
            //On dessine le rectangle bordant le contour
            indice=k;
            // cout<<"ok"<<endl;
            // Rect box = boundingRect(contours[n[k]]);     
            // rectangle(image_copy, box, Scalar(255, 0, 0));
            // //On dessine la croix au centre du rectangle
            // Point p1(X[k]-2,Y[k]);
            // Point p2(X[k]+2,Y[k]);
            // Point p3(X[k],Y[k]-2);
            // Point p4(X[k],Y[k]+2);
            // line(image_copy, p1, p2, Scalar(0, 0,255),1, LINE_8);
            // line(image_copy, p3, p4, Scalar(0, 0,255),1, LINE_8);

        }
    }

    // cout << "compute "<< chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - m_begin_timestamp).count() << endl;
    // cout<<" "<<endl;
    return Y[indice];
}
