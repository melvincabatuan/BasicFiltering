#include "com_cabatuan_basicfiltering_MainActivity.h"
#include <android/log.h>
#include <android/bitmap.h>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

#define  LOG_TAG    "LBPFaceDetection"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  DEBUG 0


Mat *pSrcColor = NULL;
Mat *pResult = NULL;


void message(Mat &img, string text){
  int fontFace = FONT_HERSHEY_TRIPLEX; //  FONT_HERSHEY_SCRIPT_SIMPLEX
  double fontScale = 0.65;
  int thickness = 1.5;
  int baseline=0;
  Size textSize = getTextSize(text, fontFace, fontScale, thickness, &baseline);
  baseline += thickness;

  // center the text
  Point textOrg((img.cols - textSize.width)/2,
              (img.rows + textSize.height)/2);

   // draw the box
/*
  rectangle(img, textOrg + Point(0, baseline),
          textOrg + Point(textSize.width, -textSize.height),
          Scalar(255,255,255));
*/

/*
    // draw the baseline  
   line(img, textOrg + Point(0, thickness),
     textOrg + Point(textSize.width, thickness),
     Scalar(255, 0, 0));

*/

    // put the text 
    putText(img, text, textOrg, fontFace, fontScale,
        Scalar::all(255), thickness, 8);
}



/*
 * Class:     com_cabatuan_basicfiltering_MainActivity
 * Method:    filter
 * Signature: (Landroid/graphics/Bitmap;[BI)V
 */
JNIEXPORT void JNICALL Java_com_cabatuan_basicfiltering_MainActivity_filter
  (JNIEnv *pEnv, jobject clazz, jobject pTarget, jbyteArray pSource, jint pFilter){

   AndroidBitmapInfo bitmapInfo;
   uint32_t* bitmapContent;

   if(AndroidBitmap_getInfo(pEnv, pTarget, &bitmapInfo) < 0) abort();
   if(bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) abort();
   if(AndroidBitmap_lockPixels(pEnv, pTarget, (void**)&bitmapContent) < 0) abort();

   /// Access source array data... OK
   jbyte* source = (jbyte*)pEnv->GetPrimitiveArrayCritical(pSource, 0);
   if (source == NULL) abort();

   /// cv::Mat for YUV420sp source
    Mat src(bitmapInfo.height + bitmapInfo.height/2, bitmapInfo.width, CV_8UC1, (unsigned char *)source);
    Mat srcGray(bitmapInfo.height, bitmapInfo.width, CV_8UC1, (unsigned char *)source);

    /// Destination image
    Mat mbgra(bitmapInfo.height, bitmapInfo.width, CV_8UC4, (unsigned char *)bitmapContent);

    if(pSrcColor == NULL)
       pSrcColor = new Mat(bitmapInfo.height, bitmapInfo.width, CV_8UC3);
 
    Mat srcColor = *pSrcColor;

   //cvtColor(src, srcColor, CV_YUV2BGR_NV21); // Red became Purple
    cvtColor(src, srcColor, CV_YUV2RGB_NV21); // Correct colors         

    
/***************************************************************************************************/
    /// Native Image Processing HERE...  

 
    if(DEBUG){
      LOGI("Starting native image processing...");
    }
       

    if(pResult == NULL)
       pResult = new Mat(srcColor.size(), srcColor.type());

    Mat result = *pResult;

    /// Averaging Filter/ boxFilter
    if (pFilter == 1){

           boxFilter(srcColor, result, -1, Size(9,9));
           message(result, "boxFilter");
           cvtColor(result, mbgra, CV_BGR2BGRA);
 
     }
    /// Gaussian Blur    
    else if (pFilter == 2){

            GaussianBlur(srcColor, result, Size(9,9),0,0);
            message(result, "Gaussian Blur");
            cvtColor(result, mbgra, CV_BGR2BGRA);
    }

  
    /// Median Blur
    else if (pFilter == 3){

            medianBlur(srcColor, result, 9);
            message(result, "Median Blur");
            cvtColor(result, mbgra, CV_BGR2BGRA);

    }

    /// Color image
    else {
         cvtColor(srcColor, mbgra, CV_BGR2BGRA);
    }





    if(DEBUG){
      LOGI("Successfully finished native image processing...");
    }
/*************************************************************************************************/


    /// Release Java byte buffer and unlock backing bitmap
    pEnv-> ReleasePrimitiveArrayCritical(pSource,source,0);
   if (AndroidBitmap_unlockPixels(pEnv, pTarget) < 0) abort();


}
