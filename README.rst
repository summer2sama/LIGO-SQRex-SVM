LIGO-SQRex-SVM
==============

Introduction
============
- LIGO-SQRex-SVM is an application based on SQRex-SVM algorithm. The goal is to find the most discriminative features of LIGO data.
- The SQRex-SVM algorithm is from the following paper: http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=4161896&tag=1. This algorithm belongs to the decompositional family of rule extraction algorithms.It utilizes the SVs only. One of its most important steps is to rank the features to obtain the most discriminative features.
- This application use the algorithm to find a set of discriminative features and tried to get a smaller set that will get a good results of TPR and FPR on TN and TP SVs.
- This application also use svm.cpp and svm.h from libsvm-2.16. It is available for download at http://www.csie.ntu.edu.tw/~cjlin/libsvm/.
- The required datafile is a LIGO-SVM model file which is trained by libsvm based.

Compile and Run
===============
- Get S6_100ms_full_train.dat.model as testing datafile.
- Compile: $ g++ SQRex.c svm.cpp -o SQRex
- Run: $ ./SQRex S6_100ms_full_train.dat.model

Output
======
- We can find a set of features whose zscore is bigger than 1.0 in the output information:
  - Rank the features by TPR/FPR ratio:
  - index:785 zscore:1.216311 threshold:-0.985045 TPR:0.318386 FPR:0.012881 ratio:24.717598 
  - index:790 zscore:1.324263 threshold:-0.978384 TPR:0.320628 FPR:0.015394 ratio:20.827720 
  - index:766 zscore:1.047990 threshold:-0.996703 TPR:0.210762 FPR:0.013195 ratio:15.972774 
  - index:1006 zscore:1.182220 threshold:-0.911581 TPR:0.282511 FPR:0.024819 ratio:11.382699 
  - index:996 zscore:1.171494 threshold:-0.905662 TPR:0.275785 FPR:0.025762 ratio:10.705157 
  - index:783 zscore:1.072738 threshold:-0.971267 TPR:0.403587 FPR:0.038329 ratio:10.529663 
  - index:788 zscore:1.048664 threshold:-0.952093 TPR:0.385650 FPR:0.044612 ratio:8.644540 
  - index:976 zscore:1.082782 threshold:-0.918710 TPR:0.273543 FPR:0.032359 ratio:8.453263 
  - index:1000 zscore:1.176295 threshold:-0.868729 TPR:0.309417 FPR:0.038014 ratio:8.139458 
  - index:986 zscore:1.058533 threshold:-0.920181 TPR:0.266816 FPR:0.032988 ratio:8.088341 
  - index:1010 zscore:1.196609 threshold:-0.884590 TPR:0.295964 FPR:0.036758 ratio:8.051742 
  - index:998 zscore:1.128454 threshold:-0.875150 TPR:0.343049 FPR:0.045240 ratio:7.582820 
  - index:1008 zscore:1.156136 threshold:-0.911788 TPR:0.331839 FPR:0.044612 ratio:7.438325 
  - index:988 zscore:1.043493 threshold:-0.925048 TPR:0.336323 FPR:0.046183 ratio:7.282420 
  - index:1016 zscore:1.000726 threshold:-0.913344 TPR:0.264574 FPR:0.037072 ratio:7.136771 
  - index:951 zscore:1.012199 threshold:-0.904279 TPR:0.253363 FPR:0.035815 ratio:7.074168 
  - index:768 zscore:1.213154 threshold:-0.863896 TPR:0.394619 FPR:0.058121 ratio:6.789577 
  - index:773 zscore:1.065199 threshold:-0.849074 TPR:0.336323 FPR:0.051524 ratio:6.527535 
  - index:980 zscore:1.108753 threshold:-0.888831 TPR:0.320628 FPR:0.053723 ratio:5.968177 
  - index:990 zscore:1.075358 threshold:-0.885534 TPR:0.325112 FPR:0.059064 ratio:5.504425 
  - index:1020 zscore:1.007359 threshold:-0.851814 TPR:0.318386 FPR:0.078542 ratio:4.053686 
  - index:955 zscore:1.015882 threshold:-0.856192 TPR:0.293722 FPR:0.078856 ratio:3.724769 

- From information above, We could see that the most discriminative features are from the 0,1,3 feature of a detecter.
- Information about the smaller rule set this application tried to get is as follows:
  - Rule Set:
  - 998 -0.875150 1 
  - 976 -0.918710 1 
  - 788 -0.952093 1 
  - 783 -0.971267 1 
  - 1006 -0.911581 1 
  - 766 -0.996703 1 
  - 790 -0.978384 1 
  - 785 -0.985045 1 
  - TP:360 FP:410 TPR:0.807175 FPR:0.128809 ratio:6.266433 

