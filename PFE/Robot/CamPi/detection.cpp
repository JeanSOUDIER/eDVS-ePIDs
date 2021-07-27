#include "detection.hpp"
#include "camera.hpp"
#include "logger.hpp"
#include "BaseThread.hpp"
#include "pid.hpp"
#include "pidN2.hpp"
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <fstream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string.h>
#include <raspicam/raspicam_cv.h>
using namespace cv;
using namespace std;


detection::detection(int AireMax, int AireMin, int Xmax, int Xmin, int Ymin, 
    int Ymax,std::chrono::time_point<std::chrono::high_resolution_clock> begin_timestamp )
    : BaseThread("detection"), m_AireMax(AireMax), m_AireMin(AireMin), m_Xmax(Xmax), m_Xmin(Xmin), m_Ymin(Ymin), m_Ymax(Ymax) {
        m_cam = new camera();
        PIDBille= new pid(0.001,0.07735, 0, 0.4455, 9.4248, -9.4248, 
            10.43, begin_timestamp);//paramètres pour le PID de la bille
        // PIDBille= new pid(0.02,0.07735, 0.003288, 0.35, 9.4248, -9.4248, 
        //     10.43, begin_timestamp);//Polynome deg 7
        // PIDBille= new pid(0.02,0.07735*1.8, 0.003288*1.3, 0.4455*50, 9.4248, -9.4248, 
        //     10.43, begin_timestamp);
         // PIDBille= new pid(0.02,0.22,0.5, 0.05, 9.4248, -9.4248, 
         //    10.43, begin_timestamp);
        // PIDBille= new pid(0.02,0.1192,0.081, 0.045, 9.4248, -9.4248, 
        //     10.43, begin_timestamp);
         // PIDBille= new pid(0.02,0.140,0.05, 0.04, 9.4248, -9.4248, 
         //    10.43, begin_timestamp);
        m_log=new logger("TestBilleRelevePos",begin_timestamp,-1);
        m_log_cpu=new logger("TestTempsPhoto",begin_timestamp,-1);
        m_log_compute=new logger("TestTempsCompute",begin_timestamp,-1);
    }

    

Mat detection::LireImage(){//Méthode qui prend une photo et fait le suivi de la bille sur cette image et la retourne

    Mat image=m_cam->TakePic();//On prend une photo
    Mat image_sortie;
    Mat image_copy = image.clone();//On clone l'image prise en photo
    cvtColor(image,image, COLOR_BGR2GRAY);//On passe l'image en nuance de gris
    threshold(image, image, 0 , 255,THRESH_OTSU);//On binarise l'image avec le seuil d'Otsu
    
//On trouve les contours
    vector<vector<Point>> contours;//le contour est un vecteur de point
    vector<Vec4i> hierarchy;
    findContours(image, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
     


    int taille=contours.size();//Nombre de contours détectés
    int n[taille];
    for (int j = 0; j <taille; j++) {
       
        if(contourArea(contours.at(j))<m_AireMax && contourArea(contours.at(j))>m_AireMin){//On élimine les contours d'aire trop grande ou trop petite
            n[j]=j;
        } else {
            n[j]=-1;
        }
    }

    int X[taille];
    int Y[taille];
    //On calcul les coordonnées du centre du rectangle pour les contours restants
    for (int k=0;k<taille;k++){
        if (n[k]!=-1){
            Rect box = boundingRect(contours[n[k]]);//Rectangle bordant le contour     
            X[k]=box.x+box.width/2;//Coordonnées du centre du rectangle
            Y[k]=box.y+box.height/2;
       } else {
            X[k]=-1;
            Y[k]=-1;
       }
    }

    //On élimine les contours qui ne correspondent pas à la bille avec des coordonnées en Y trop petites ou trop grandes
    for (int k=0;k<taille;k++){
        if (X[k]<m_Xmin || X[k]>m_Xmax || Y[k]>m_Ymax || Y[k]<m_Ymin){
            X[k]=-1;
            Y[k]=-1;
        }
    }
    //Dessin des contours sur l'image clone
    for (int k=0;k<taille;k++){
        if (X[k]!=-1 && Y[k]!=-1){
            drawContours(image_copy, contours, n[k], Scalar(0, 255, 0), 0);//On dessine les contours
            Rect box = boundingRect(contours[n[k]]);//On dessine le rectangle bordant les contours     
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
    // Point pt1(m_Xmin,m_Ymin);
    // Point pt2(m_Xmax,m_Ymax);
    // cv::rectangle(image_copy, pt1, pt2, cv::Scalar(0,0,255));
    image_sortie=image_copy;
    return image_sortie;//On retourne l'image 
}

detection::~detection() {//Destructeur
    delete m_cam;
    delete PIDBille;
    delete m_log;
}

  
void detection::LirePosBille(void){//Méthode de détection de la bille et de renvoi de sa position
    m_log_cpu->Tic();//calcul et sauvegarde du temps pour prendre une photo
    Mat image=m_cam->TakePic();//On prend une photo
    m_log_cpu->Tac();
    m_log_compute->Tic();
    
    int indice=0;
    Mat image_sortie;

    Mat image_copy = image.clone();//On clone l'image
    cvtColor(image,image, COLOR_BGR2GRAY);//On passe l'image en nuance de gris
    threshold(image, image, 0 , 255,THRESH_OTSU);//On binarise l'image avec le seuil d'Otsu
    
    
//On trouve les contours 
    vector<vector<Point>> contours;//le contour est un vecteur de point
    vector<Vec4i> hierarchy;
    findContours(image, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    
    int taille=contours.size();//Nombre de contours détectés
    int n[taille];
    for (int j = 0; j <taille; j++) {
        if(contourArea(contours.at(j))<m_AireMax && contourArea(contours.at(j))>m_AireMin){//On élimine les contours d'aire trop grande ou trop petite
            n[j]=j;
        } else {
            n[j]=-1;
        }
    }
    int X[taille];
    int Y[taille];

//On calcul les coordonnées du centre du rectangle pour les contours restants
    for (int k=0;k<taille;k++){
        if (n[k]!=-1){
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
            X[k]=-1;
            Y[k]=-1;
        }
    }
//On récupère l'indice correspondant au contour de la bille
    for (int k=0;k<taille;k++){
        if (X[k]!=-1 && Y[k]!=-1){
            // drawContours(image_copy, contours, n[k], Scalar(0, 255, 0), 0);
            // imwrite("_image_contours_filtrage.jpg",image_copy);
            // imwrite("_image_contours.jpg",image_copy);
            //On dessine le rectangle bordant le contour
            indice=k;
            // // cout<<"ok"<<endl;
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
    // Point pt1(m_Xmin,m_Ymin);
    // Point pt2(m_Xmax,m_Ymax);
    // cv::rectangle(image_copy, pt1, pt2, cv::Scalar(0,0,255));
    PIDBille->PosBille.store((Y[indice]-m_mil)*m_k);//On stocke la position de la bille en convertissant la position en mm
    m_log_compute->Tac();

    // imshow("c", image_copy);
    // while(waitKey(0) != 27) {}
}
void* detection::ThreadRun(){
    int Pos=(50-m_mil)*m_k;
    int var=(50-m_mil)*m_k;
    int i=0;

    PIDBille->StartThread();//on lance le thread du PIDBille
    while (GetStartValue()) {
        LirePosBille();
        Pos=PIDBille->PosBille.load();//On lit la position de la bille 
         if ((Pos>(26-m_mil)*m_k && Pos<(93-m_mil)*m_k && abs((Pos-var)<50) /*&& abs(Pos-var)>1*/) || i<2  ){//On teste si la position de la bille est cohérente
            m_log->WriteIN({ i, Pos });//On sauveagarde la position dans le fichier csv
            m_log->TacI();
            PIDBille->PosBille.store(Pos);//On sauvegarde la position
            var=Pos;
            i++;
        } else {//Si la position détectée n'est pas cohérente on renvoit la position précédente
            cout<<"precedent"<<endl;
            m_log->WriteIN({ i, var });//On sauvegarde la position précédente dans le fichier csv
            m_log->TacI();
            PIDBille->PosBille.store(var);//On sauvegarde la position précédente
            i++;  
        }
    }
    PIDBille->StopThread();//on stop le thread du PID de la Bille
    return ReturnFunction();
}

 void detection::ReadPot() {//méthode qui appelle la méthode ReadPot() de PIDBille
   PIDBille->ReadPot();
}
 void detection::SetCons(double a) {//méthode qui appelle la méthode SetCons() de PIDBille
   PIDBille->SetCons(a);
}