#include "video.h"
#include "RobotProtocol.h"

cv::Mat BinaryRed(cv::Mat frame)
{
	// hsv로 바꾸고
	cv::Mat Temp , temp, temp2 ;
	cvtColor(frame, Temp, CV_BGR2HSV) ; 
	cv::Mat small(2, 2, CV_8U, cv::Scalar(1));

	// 빨간색으로 이진화 
	inRange(Temp, cv::Scalar(0,100,80), cv::Scalar(10,255,255), temp ) ;
	inRange(Temp, cv::Scalar(170,100,80), cv::Scalar(180,255,255),temp2) ;
	Temp = temp | temp2 ;

	cv::erode(Temp, Temp , small ) ;
	cv::dilate(Temp, Temp , cv::Mat() ) ;

	return Temp ;
}


void BinaryGreen(cv::Mat &frame)
{
	// YCBCR 바꾸고
	cvtColor(frame, frame, CV_BGR2YCrCb) ; 
	cv::Mat temp, temp2 ,temp3  ;
	cv::Mat small(2, 2, CV_8U, cv::Scalar(1));
	cv::Mat element(4, 4, CV_8U, cv::Scalar(1));

	// 초록색으로 이진화 
//	inRange(frame, cv::Scalar(5,3,3), cv::Scalar(70,120,120),temp )  ;
	inRange(frame, cv::Scalar(1,0,0), cv::Scalar(160,119,119),temp )  ;
	inRange(frame, cv::Scalar(50,0,0), cv::Scalar(250,115,115),temp2 )  ;
	
	frame = temp | temp2 ;

	cv::erode(frame,frame, small) ; // default 3x3
	cv::dilate(frame, frame , element) ; // element
	cv::dilate(frame, frame , element) ;

}

// Red라인을 찾고 30도 미만인 경우 직선의 그래프의 양 좌표를 딴다.
cv::Mat HoughRedLine(cv::Mat &frame, bool *angle, int *x1, int *y1, int *x2, int *y2, float *Rho ,float *Theta, bool *Left)
{
	// canny 엣지 알고리즘 적용
	cv::Mat contours ;
	cv::Canny(frame, contours, 125, 350) ;

	//선 감지 위한 허프 변환
	std::vector<cv::Vec2f> lines ;
	//단계별 크기, 투표 최대 갯수
	cv::HoughLines(contours, lines, 1, PI / 180, 20 ) ;

	//선 그리기
	cv::Mat result(contours.rows, contours.cols , CV_8U , cv::Scalar(255)) ;

	// 선 벡터를 반복해 선 그리기
	std::vector<cv::Vec2f>::const_iterator it= lines.begin();

	while (it!=lines.end()) {
		float rho = (*it)[0];   // 첫 번째 요소는 rho 거리
		float theta = (*it)[1]; // 두 번째 요소는 델타 각도
		*Left = false ;

		cv::Point pt1(rho/cos(theta), 0); // 첫 행에서 해당 선의 교차점   
		cv::Point pt2((rho-result.rows*sin(theta))/cos(theta), result.rows - 1);

		// 30도
		if( theta  > PI/3. &&  theta < PI/3.*2. ) {

			*angle = true ;
			// 직선의 식 rho = xcos(theta) + ysin(theta) ;
			*x1 = ScreenX1 ; *x2 = ScreenX2 ;
			*y1 = abs(-rho/sin(theta)) ;	*y2 = abs(int(ScreenX2*cos(theta) - rho )) / sin(theta) ; 	
			*Rho = rho ; *Theta = theta ;	

		    	// 마지막 행에서 해당 선의 교차점
		   	cv::line(frame, pt1, pt2, cv::Scalar(128,128,128), 5); // 선으로 그리기 

			break ;
		}
		if(it == lines.end() - 1 ) {
			// 왼쪽으로 돌아야 한다.
			if(theta < PI/3. && theta > 0 || theta > PI && theta < PI/3.*4. ) *Left = true ;
			 // 마지막 행에서 해당 선의 교차점
			cv::line(frame, pt1, pt2, cv::Scalar(128,128,128), 5); // 선으로 그리기
		}
		++it;
	 }

	return frame ;
}

bool FindRed(cv::Mat frame) 
{
	for(int i = 0 ; i < frame.rows ; i++){
		for(int j = 0 ; j < frame.cols ; j  = j + 2){
			if(frame.ptr(i,j)[0]) return true ;
		}
	}
	return false ;
}

void DeleteAboveLine(cv::Mat temp, cv::Mat &frame, int x1, int y1, int x2, int y2, float rho , float theta)
{
	for(int i = 0 ; i < temp.rows ; i++){
		for(int j = 0 ; j < temp.cols ; j++){
			// 빨간 직선위에 모두 날린다.
			if( rho - j * cos(theta) >  i * sin(theta) )  {
					frame.ptr(i,j)[0] = 0 ;
					frame.ptr(i,j)[1] = 0 ;
					frame.ptr(i,j)[2] = 0 ;
			}
		}
	}

}

short ExistAndDistance(cv::Mat &frame) 
{
	uchar* data = (uchar*) frame.data ;
	for(int i =  frame.rows - 1  ; i >= 0 ; i--){
		for(int j = frame.cols - 1 ; j >= 0 ; j--){
			if(data[i * frame.cols + j]) {
				return (i/20)  ;
			}
		}
	}
	return -1 ;
}

void ImageProcess()
{
	cv::VideoCapture capture(0);
	bool OverAngle ;
	if( !capture.isOpened() ) {
	        std::cerr << "Could not open camera" << std::endl;
	        exit(1) ;
	}
	Uart_open() ; // 통신 준비
	Init_console() ; 

    while (true) {
	// start
	int x1, x2, y1, y2 ;
	bool Left ;
	float rho, theta ;
        cv::Mat frame , temp ;
        capture >> frame;  // get a new frame from webcam
	cv::imshow("Original", frame);

	// 최근접 화소 보간법
	cv::resize(frame, frame, cv::Size(frame.cols/4 , frame.rows/4) , 0 , 0 , CV_INTER_NN) ;
	OverAngle = false ;

	// front Red ?
	temp = BinaryRed(frame) ;

	if(FindRed(temp)) {// Yes
		temp = HoughRedLine(temp,  &OverAngle, &x1,&y1,&x2,&y2, &rho, &theta, &Left) ;
		// within 30 ?
		if( OverAngle == true )  { // yes 
			DeleteAboveLine(temp, frame,  x1,y1,x2,y2,rho, theta) ;
			BinaryGreen(frame) ;
			short Distance = ExistAndDistance(frame) ;
			if(Distance){
				switch(Distance){
					case  -1: std::cout << "-1" << std::endl ;
					case 0: std::cout << "0" <<  std::endl ;
					case 1: std::cout << "1" <<  std::endl ;
					case 2: std::cout << "2" << std::endl ;
					case 3: std::cout << "3" << std::endl ;
					case 4: std::cout << "4" << std::endl ;
					case 5: std::cout << "5" << std::endl ;			
				}
			}			
		//	std::cout << Distance << std::endl; 
		}
		else { 
			if(Left) std::cout << "Turn Left " << std::endl;
			else std::cout <<"Turn Right " << std::endl ; 
			// Turn Left / Right 
		} ;	
	}
	else { // No
//		Motion(109) ; // turn back
//		std::cout << Check_Read() << std::cout  ;
	} 	

	cv::imshow("hough", temp);
        cv::imshow("GreenResult", frame);

        if (cv::waitKey(30) >= 0) break;
    }
	 Uart_close() ;
}
