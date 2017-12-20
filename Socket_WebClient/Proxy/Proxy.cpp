#include "stdafx.h"
#include "Proxy.h"
#include <string>
/* Khai bao thu vien */
#include "afxsock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
int main(){
	CSocket csServer;
	CSocket csClient;
	
	if(csServer.Create(8888,SOCK_STREAM,L"127.0.0.1")==false){
		cout<<"Khoi tao socket Server that bai!"<<endl;
		return 0;
	}

	if(csServer.Listen()==false){
		cout<<"Khong the lang nghe tren port nay"<<endl;
	}
	cout<<"Listening connection.."<<endl;

	return 1;
}