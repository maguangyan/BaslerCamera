
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>
#include <pylon/PylonIncludes.h>
#include<iostream>
#include <pylon/PylonGUI.h>    
//�����Ƿ񱣴�ͼƬ
#define saveImages 1
//�����Ƿ��¼��Ƶ
#define recordVideo 0

//�����ռ�.
using namespace Pylon;
using namespace cv;
using namespace std;
//����ץȡ��ͼ����
static const uint32_t c_countOfImagesToGrab = 10;

int main()
{
	//Pylon�Զ���ʼ������ֹ
	Pylon::PylonAutoInitTerm autoInitTerm;

	//�����������������ʶ��������
	CInstantCamera camera(CTlFactory::GetInstance().CreateFirstDevice());
	// ��ӡ���������
	std::cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;
	//��ȡ����ڵ�ӳ���Ի���������
	GenApi::INodeMap& nodemap = camera.GetNodeMap();
	//�����
	camera.Open();
	//��ȡ��������Ⱥ͸߶�
	GenApi::CIntegerPtr width = nodemap.GetNode("Width");
	GenApi::CIntegerPtr height = nodemap.GetNode("Height");

	//���������󻺳���,Ĭ��Ϊ10
	camera.MaxNumBuffer = 5;
	// �½�pylon ImageFormatConverter����.
	CImageFormatConverter formatConverter;
	//ȷ��������ظ�ʽ
	formatConverter.OutputPixelFormat = PixelType_BGR8packed;
	// ����һ��Pylonlmage��������������OpenCV images
	CPylonImage pylonImage;

	//����һ�����α�����������ץȡ��ͼ���Լ������ļ�������
	int grabbedlmages = 0;

	// �½�һ��OpenCV video creator����.
	VideoWriter cvVideoCreator;
	//�½�һ��OpenCV image����.

	Mat openCvImage;
	// ��Ƶ�ļ���

	std::string videoFileName = "openCvVideo.avi";
	// ������Ƶ֡��С
	cv::Size frameSize = Size((int)width->GetValue(), (int)height->GetValue());

	//������Ƶ�������ͺ�֡�ʣ�������ѡ��
	// ֡�ʱ���С�ڵ����������֡��
	cvVideoCreator.open(videoFileName, CAP_ANY, 10, frameSize, true);
	//cvVideoCreator.open(videoFileName, CV_F0URCC('M','P',,4','2��), 20, frameSize, true);
	//cvVideoCreator.open(videoFileName, CV_FOURCC('M', '3', 'P', 'G'), 20, frameSize, true);


	// ��ʼץȡc_countOfImagesToGrab images.
	//���Ĭ����������ץȡģʽ
	camera.StartGrabbing(c_countOfImagesToGrab, GrabStrategy_LatestImageOnly);

	//ץȡ�������ָ��
	CGrabResultPtr ptrGrabResult;

	// ��c_countOfImagesToGrab images��ȡ�ָ��ɹ�ʱ��Camera.StopGrabbing() 
	//��RetrieveResult()�����Զ�����ֹͣץȡ

	while (camera.IsGrabbing())

	{
		// �ȴ����պͻָ�ͼ�񣬳�ʱʱ������Ϊ5000 ms.
		camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);

		//���ͼ��ץȡ�ɹ�
		if (ptrGrabResult->GrabSucceeded())
		{
			// ��ȡͼ������
			cout << "SizeX: " << ptrGrabResult->GetWidth() << endl;
			cout << "SizeY: " << ptrGrabResult->GetHeight() << endl;

			//��ץȡ�Ļ�������ת����pylon image.
			formatConverter.Convert(pylonImage, ptrGrabResult);

			// �� pylon imageת��OpenCV image.
			openCvImage = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t*)pylonImage.GetBuffer());




			//�����Ҫ����ͼƬ
			if (saveImages)
			{
				std::ostringstream s;
				// �����������ļ����洢ͼƬ
				s << "./image_src/image_" << grabbedlmages << ".jpg";
				std::string imageName(s.str());
				//����OpenCV image.
				bool isWrite = imwrite(imageName, openCvImage);
				grabbedlmages++;
				if (!isWrite)
				{
					cout << "write image failed!" << endl;
				}
			}

			//�����Ҫ��¼��Ƶ
			if (recordVideo)
			{

				cvVideoCreator.write(openCvImage);
			}

			//�½�OpenCV display window.
			namedWindow("OpenCV Display Window", WINDOW_NORMAL); // other options: CV_AUTOSIZE, CV_FREERATIO
			//��ʾ��ʱӰ��.
			imshow("OpenCV Display Window", openCvImage);

			// Define a timeout for customer's input in
			// '0' means indefinite, i.e. the next image will be displayed after closing the window.
			// '1' means live stream
			waitKey(1);//���������1��0����Ҫ�����رմ��ڰ�ť���ܲɼ���һ��ͼ��

		}

	}


	//catch (GenICam::GenericException &e)
	//{
	//	// Error handling.
	//	cerr << "An exception occurred." << endl
	//		<< e.GetDescription() << endl;
	//}
	return 0;
}
