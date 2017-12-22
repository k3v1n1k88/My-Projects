// Proxy2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Proxy2.h"
#include "afxsock.h"
#include <stdexcept>
#include <string>
#include <fstream>
#include <windows.h>
#include "TimeOutSocket.h"
#include <thread>
#include <vector>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Trang web forbiden
#define forbiden "<!DOCTYPE html>\r\n<html>\r\n<head><title>403 Forbiden</title></head>\r\n<body>\r\n<h1>Forbiden 403</h1>\r\n<p>You don't have permission to host %s</p>\r\n</body>\r\n</html>"

// Trang web not found
#define pagenotfound "<!DOCTYPE html>\r\n<html>\r\n<head><title>401 Page not found</title></head>\r\n<body>\r\n<font size=\"25\"><b>401 Page not found</b></font>\r\n<p>Your request URL was not found</p>\r\n<p>Please check again</p>\r\n<hr>\r\n<p align=\"center\" >Proxy create by k3v1n1k88</p>\r\n</body>\r\n</html>"
//#define https "<!DOCTYPE html>\r\n<html>\r\n<head><title>Sorry, we do not support for HTTPS</title></head>\r\n<body>\r\n<font size=\"25\"><b>401 Page not reach</b></font>\r\n<p>Your request is not support</p>\r\n<p>Please try other</p>\r\n<hr>\r\n<p align=\"center\" >Proxy create by k3v1n1k88</p>\r\n</body>\r\n</html>"
//#define forbiden "<html><head><title>403 Forbiden</title></head>\n<body><h1>Forbiden 403</h1>\n<p>You don't have permission to access/on this website</p>\n</body>\n</html>"
// The one and only application object
#define MAX_THREADS 5

CWinApp theApp;

using namespace std;

typedef struct MyData{
    SOCKET *hcs;
} MYDATA, *PMYDATA;

//Dung de Get IP tu 1 host cho truoc
char* GetIP(char* host);

//Dung de lay ten cua host tu lenh GET
char* GetNameHost(const char *get);

//Chuyen tu char* sang LPCWSTR
wchar_t *convertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}

//Check black list
BOOL CheckBlackList(fstream &fin, char* host, CSocket &cs);
DWORD WINAPI BeginProcess(LPVOID lp);


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	PMYDATA pDataArray[MAX_THREADS];
    DWORD   dwThreadIdArray[MAX_THREADS];
    HANDLE  hThreadArray[MAX_THREADS];
	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
			if(AfxSocketInit()==false){
				cout<<"Cannot create Socket Library!";
				return false;
			}
			

			CSocket csServer;	// Socket Server
			SOCKET hSocket[10];	


			if(csServer.Create(8888,SOCK_STREAM,L"127.0.0.1")==false){
				cout<<"Khoi tao socket that bai..."<<endl;
				cout<<csServer.GetLastError();
				return 1;
			}

			if(csServer.Listen(10)==false){
				cout<<"Khong the lang nghe tren port nay"<<endl;
			}

			// Bat dau chay server voi 5 luong
			while(1){
				CSocket *csClient = new CSocket;
				for(int i=0;i<10;i++){
					if(csServer.Accept(*csClient)){
						cout<<"Client da ket noi..."<<endl;
						hSocket[i] = csClient ->Detach();
						hThreadArray[i] = CreateThread(NULL, 0,BeginProcess,(LPVOID)&hSocket[i],0,&dwThreadIdArray[0]);
					}
				}
				WaitForMultipleObjects(10, hThreadArray, TRUE, INFINITE);
				delete csClient;
			}
				//for(int i=0;i<MAX_THREADS;i++){
				//	//csServer.Attach(csClient[i]);
				//	cout<<"DANG CHO MOT KET NOI..."<<endl;
				//	CSocket *csClient = new CSocket;
				//	if(csServer.Accept(*csClient)==TRUE){
				//		//csClient[i].Detach();

				//		hSocket[i] = csClient->Detach();
				//		/*pDataArray[i] = (PMYDATA) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,sizeof(MYDATA));
				//		pDataArray[i]->hcs = &hSocket[i];*/
				//		hThreadArray[i] = CreateThread(NULL, 0,(LPTHREAD_START_ROUTINE) BeginProcess,(LPVOID)hSocket[i],0,&dwThreadIdArray[i]);
				//		if(hThreadArray[i]==NULL){
				//			cout<<"Khong the tao thread "<<i<<" "<<endl;
				//		}
				//		
				//		//csClient[i].ShutDown();
				//		//csClient[i].Close();
				//		//csClient[i].Detach();
				//	}
				//	
				//}
				//WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);
				//for(int i=0; i<MAX_THREADS; i++)
				//{
				//	
				//	CloseHandle(hThreadArray[i]);
				//	if(pDataArray[i] != NULL)
				//	{
				//		HeapFree(GetProcessHeap(), 0, pDataArray[i]);
				//		pDataArray[i] = NULL;    // Ensure address is not reused.
				//	}
				//}
			//}
			
			return 0;
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}

char* GetIP(char* host){
	struct hostent *hent;
	int iplen = 15; //XXX.XXX.XXX.XXX
	char *ip = (char *)malloc(iplen + 1);
	memset(ip, 0, iplen + 1);
	if ((hent = gethostbyname(host)) == NULL)
	{
		perror("Can't get IP");
		return 0;
	}
	if (inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip, iplen) == NULL)
	{
		perror("Can't resolve host");
		return 0;
	}
	return ip;
}

//Get host from GET instruction
char* GetNameHost(const char *get){
	char *host;
	char *temp;
	char *tempget;
	tempget = new char [strlen(get)+1];
	memcpy(tempget,get,strlen(get)+1);
	char* pfind = strstr(tempget,"Host");
	host = strtok_s(pfind," ",&temp);
	host = strtok_s(NULL,"\r\n: ",&temp);
	return host;
}

BOOL CheckBlackList(fstream &fin, char* host, CSocket *cs){
	while(!fin.eof()){
		char blacklist[100];
		fin>>blacklist;
		if(strstr(host,blacklist)!=NULL){
			char *buildweb;
			buildweb = new char [strlen(forbiden)+strlen(host)-1];
			sprintf_s(buildweb, strlen(buildweb)+1, forbiden,host);
			cs->Send(buildweb,strlen(buildweb),0);
			return TRUE;
		}
	}
	return FALSE;
}

DWORD WINAPI BeginProcess(LPVOID lp){
	SOCKET hcs =*(SOCKET*) lp;
	CSocket *csClient = new CSocket;
	csClient->Attach(hcs);
	cout<<"Dang nhan du lieu tu browser..."<<endl;
	char* buff;
	CSocket csWebsite;
	fstream fin;
	fstream fout;
	string buffer;
	char *host =new char;
	char *ip =new char;
	char* get=new char;
	char* pstrBufferReceiveFromWeb = new char;
	fout.open("output.txt",ios_base::out);
	cout<<"Client da ket noi..."<<endl;				
	int count = 1;
	while(count){
		buff = new char[1000];
		count = csClient->Receive(buff,1000,0);
		buffer.append(buff,count);
		if(count<1000)
			break;
	}
	//cout<<buffer<<endl;
	if(buffer.size()!=0){
		//sua lai thanh 1.0
		if(buffer.find("HTTP/1.1")!=string::npos){
			buffer.replace(buffer.find("HTTP/1.1"),8,"HTTP/1.0");
		}
		//Sua connection closed
		if(buffer.find("css")==string::npos){
			if(buffer.find("keep-alive")!=string::npos){
				buffer.replace(buffer.find("keep-alive"),10,"Closed");
			}
			if(buffer.find("keep-alive")!=string::npos){
				buffer.replace(buffer.find("keep-alive"),10,"Closed");
			}
		}
		//Build GET instruction from GET browser
		get = new char[buffer.size()+1];
		memcpy(get,buffer.c_str(),buffer.size()+1);
		cout<<get;
		host = GetNameHost(get);
		ip = GetIP(host);
		cout<<"Host : "<<host<<endl;
		if(ip == NULL){
			// when IP is not avaible we don't process it
			cout<<"IP: don't know! Please check again"<<endl;
			csClient->Send(pagenotfound,strlen(pagenotfound),0);
		}
		// Begin work with IP available
		else{
			string strReceiveFromWeb = *(new string);
			pstrBufferReceiveFromWeb = new char [1000];
			cout<<"IP: "<<ip<<endl;
			fin.open("blacklist.txt",ios_base::in);
			// Xu li website bi cam
			if(fin.is_open()&&CheckBlackList(fin,host,csClient)){
				/*while(!fin.eof()){
					char blacklist[100];
					fin>>blacklist;
					if(strstr(host,blacklist)!=NULL){
						char *buildweb;
						buildweb = new char [strlen(forbiden)+strlen(host)-1];
						sprintf_s(buildweb, strlen(buildweb)+1, forbiden,host);
						csClient.Send(buildweb,strlen(buildweb),0);
						break;
					}
				}*/
		
			}
			//Xu li website k bi cam
			else{
				csWebsite.Create();
	
				if(csWebsite.Connect(convertCharArrayToLPCWSTR(ip),80)<0){
					ASSERT(FALSE);
					cout<<"Counld't connect!"<<endl;
					csClient->Send(pagenotfound,strlen(pagenotfound),0);
				}
				else{
					int num = 1;

					//Send request
					csWebsite.Send(get,strlen(get),0);
					while(num){
						// Cac cTimeOut muc dich de set timeoout cho socket khi cho` qua lau
						CTimeOutSocket cTimeOut;
						if(!cTimeOut.SetTimeOut(3000)){
							ASSERT(FALSE);
						}
						num = csWebsite.Receive(pstrBufferReceiveFromWeb,1000,0);
						int nError = GetLastError();
						if(nError == WSAEWOULDBLOCK)
							num = 0;
				
						if(num == -1){
							break;
						}

						if(!cTimeOut.KillTimeOut()){
							ASSERT(FALSE);
						}
						//cout<<strReceiveFromWeb;
						if(num!=0){
							strReceiveFromWeb.append(pstrBufferReceiveFromWeb,num);
							csClient->Send(strReceiveFromWeb.c_str(),num,0);
							fout<<strReceiveFromWeb;
							strReceiveFromWeb.clear();
						}
					}
				}
				csWebsite.ShutDown();
				csWebsite.Close();
				delete pstrBufferReceiveFromWeb;
			}
	
		}//End work with website available
	
		delete get;
	}
	//delete host;
	//delete buildweb;
	//delete buildweb;
	csClient->ShutDown();
	csClient->Close();
	delete buff;
	buffer.clear();
	cout<<"...KET THUC KET NOI*/"<<endl<<endl;
	fout.close();
	fin.close();	
	return 0;
	csClient->Detach();
}