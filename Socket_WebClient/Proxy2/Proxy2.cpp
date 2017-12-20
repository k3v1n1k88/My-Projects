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
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define forbiden "<!DOCTYPE..><html>\r\n<head><title>403 Forbiden</title></head>\r\n<body>\r\n<h1>Forbiden 403</h1>\r\n<p>You don't have permission to host %s</p>\r\n</body>\r\n</html>"
#define pagenotfound "<!DOCTYPE..><html>\r\n<head><title>401 Page not found</title></head>\r\n<body>\r\n<font size=\"25\"><b>401 Page not found</b></font>\r\n<p>Your request URL was not found</p>\r\n<p>Please check again</p>\r\n<hr>\r\n<p align=\"center\" >Proxy create by k3v1n1k88</p>\r\n</body>\r\n</html>"
//#define forbiden "<html><head><title>403 Forbiden</title></head>\n<body><h1>Forbiden 403</h1>\n<p>You don't have permission to access/on this website</p>\n</body>\n</html>"
// The one and only application object

CWinApp theApp;

using namespace std;

char* GetIP(char* host);
char* GetNameHost(const char *get);
wchar_t *convertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

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
			

			CSocket csServer;
			CSocket csClient;
			CSocket csWebsite;
	
			if(csServer.Create(8888,SOCK_STREAM,L"127.0.01")==false){
				cout<<"Khoi tao socket that bai..."<<endl;
				cout<<csServer.GetLastError();
				return 1;
			}

			if(csServer.Listen()==false){
				cout<<"Khong the lang nghe tren port nay"<<endl;
			}
			
			//Begin while(1)
			while(1){
				string buffer;
				char* pstrBufferReceiveFromWeb;
				char* buff;
				char* get;
				char *host;
				char *ip;
				fstream fin;
				fstream fout;
				fout.open("output.txt",ios_base::out);
				cout<<"/* DANG LANG NGHE 1 KET NOI..."<<endl<<endl;

				
				if(csServer.Accept(csClient)==true){
					cout<<"Client da ket noi..."<<endl;				
					int count = 1;
					while(count){
						buff = new char[1000];
						count = csClient.Receive(buff,1000,0);
						buffer.append(buff,count);
						if(count<1000)
							break;
					}
					cout<<buffer<<endl;
					if(buffer.size()!=0){
						//Build GET instruction from GET browser
						get = new char[buffer.size()+1];
						memcpy(get,buffer.c_str(),buffer.size()+1);

						host = GetNameHost(get);
						ip = GetIP(host);
						cout<<"Host : "<<host<<endl;
						if(ip == NULL){
							// when IP is not avaible we don't process it
							cout<<"IP: don't know! Please check again"<<endl;
							csClient.Send(pagenotfound,strlen(pagenotfound),0);
						}
						
						// Begin work with IP available
						else{
							string strReceiveFromWeb;
							pstrBufferReceiveFromWeb = new char [1000];
							cout<<"IP: "<<ip<<endl;
							fin.open("blacklist.txt",ios_base::in);
							// Xu li website bi cam
							if(fin.is_open()){
								while(!fin.eof()){
									char blacklist[100];
									fin>>blacklist;
									if(strstr(host,blacklist)!=NULL){
										char *buildweb;
										buildweb = new char [strlen(forbiden)+strlen(host)-1];
										sprintf_s(buildweb, strlen(buildweb)+1, forbiden,host);
										csClient.Send(buildweb,strlen(buildweb),0);
										break;
									}
								}
							}
							//Xu li website k bi cam
							else{
								csWebsite.Create();
							
								if(csWebsite.Connect(convertCharArrayToLPCWSTR(ip),80)<0){
									ASSERT(FALSE);
									cout<<"Counld't connect!"<<endl;
									csClient.Send(pagenotfound,strlen(pagenotfound),0);
								}
								else{
									int num = 1;

									//Send request
									csWebsite.Send(get,strlen(get),0);
									while(num){
										//timeval timeout = { 5, 0 };
										//fd_set in_set;

										//FD_ZERO(&in_set);
										//FD_SET(csWebsite, &in_set);

										//// select the set
										//int cnt = select(csWebsite + 1, &in_set, NULL, NULL, &timeout);
										CTimeOutSocket cTimeOut;
										if(!cTimeOut.SetTimeOut(3000)){
											ASSERT(FALSE);
										}
										if((num = csWebsite.Receive(pstrBufferReceiveFromWeb,1024,0))<0){
											Sleep(1000);
										}
										if(num == -1){
											csWebsite.Send("",0,0);
											break;
										}
										if(!cTimeOut.KillTimeOut()){
											ASSERT(FALSE);
										}
										//cout<<strReceiveFromWeb;
										if(num!=0){
											strReceiveFromWeb.append(pstrBufferReceiveFromWeb,num);
											csClient.Send(strReceiveFromWeb.c_str(),num,0);
											fout<<strReceiveFromWeb;
											strReceiveFromWeb.clear();		
										}
										if(num == 0 ){
											csWebsite.Send("",0,0);
											break;
										}
									}
								}
								csWebsite.ShutDown();
								csWebsite.Close();
								//delete pstrBufferReceiveFromWeb;
							}
						
						}//End work with website available
						
						delete get;
					}
					//delete host;
					//delete buildweb;
					//delete buildweb;
					csClient.ShutDown();
					csClient.Close();
					delete buff;
					buffer.clear();
					cout<<"...KET THUC KET NOI*/"<<endl<<endl;
					fout.close();
					fin.close();
				}
			}//end while(1)
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