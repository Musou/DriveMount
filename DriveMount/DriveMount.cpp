// DriveMount.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>

#define fsize_min 4200000000
#define fsize_max 4300000000

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	// variable declarations
	WCHAR buf[MAX_PATH+1]={0}; // volume GUID
	WCHAR buf2[MAX_PATH+1]={0}; // volume name
	WCHAR snb[MAX_PATH+1]={0}; // filesystem type
	WCHAR dbuf[MAX_PATH+1]={0}; // directory name
	char pathnames[MAX_PATH+1]={0};
	PDWORD length=0;
	DWORD sn=0; // serial number
	DWORD mcl=0; // max char length
	DWORD sf=0; // system flags
	unsigned __int64 FBA=0;
	unsigned __int64 TNB=0;
	unsigned __int64 TNFB=0;
	char ch[260];
	char ch2[260];
	char ch3[260];
	char DefChar = ' ';
	HANDLE hVol;
	int drives=0; // counter to check how many drives we have
	vector<wstring> dvec; // vector of drives which we will map
	vector<wstring> dirvec; //names of directories to which we map
	
	// generate the directories where we will mount the drives
	for(int i=0;i<26;++i) {
		char dirname[6]="C:\\ \\";
		dirname[3]=i+65;
		MultiByteToWideChar(CP_ACP,0,dirname,7,dbuf,MAX_PATH);
		CreateDirectory(dbuf,0);
		wstring temp(dbuf);
		dirvec.push_back(temp);
	}
	for(int i=0;i<6;++i) {
		char dirname[7]="C:\\A \\";
		dirname[4]=i+65;
		MultiByteToWideChar(CP_ACP,0,dirname,7,dbuf,MAX_PATH);
		CreateDirectory(dbuf,0);
		wstring temp(dbuf);
		dirvec.push_back(temp);
	}

	// grab first volume, get name of volume, get volume info
	hVol=FindFirstVolume(buf,(DWORD) MAX_PATH);
	GetVolumeInformationByHandleW(hVol,buf,MAX_PATH,&sn,&mcl,&sf,snb,MAX_PATH);
	GetVolumeInformation(buf,buf2,MAX_PATH,&sn,&mcl,&sf,snb,MAX_PATH);
	
	// convert to human readable from widechar
	WideCharToMultiByte(CP_ACP,0,buf,-1, ch,260,&DefChar, NULL);
	WideCharToMultiByte(CP_ACP,0,buf2,-1, ch3,260,&DefChar, NULL);
    WideCharToMultiByte(CP_ACP,0,snb,-1, ch2,260,&DefChar, NULL);

	//get size of disk for check
	//GetVolumePathNamesForVolumeName(buf,(LPTSTR)pathnames,MAX_PATH,length);
	GetDiskFreeSpaceEx(buf,(PULARGE_INTEGER)&FBA,(PULARGE_INTEGER)&TNB,(PULARGE_INTEGER)&TNFB);
	string snbstr(ch2);
	if(TNB>fsize_min && TNB<fsize_max && snbstr.compare("FAT32")==0) {
		++drives;
		wstring temp(buf);
		dvec.push_back(temp);
	}

	while(FindNextVolume(hVol,buf,(DWORD)MAX_PATH)) {
		GetVolumeInformationByHandleW(hVol,buf,MAX_PATH,&sn,&mcl,&sf,snb,MAX_PATH);
		GetVolumeInformation(buf,buf2,MAX_PATH,&sn,&mcl,&sf,snb,MAX_PATH);
		WideCharToMultiByte(CP_ACP,0,buf,-1, ch,260,&DefChar, NULL);
		WideCharToMultiByte(CP_ACP,0,snb,-1, ch2,260,&DefChar, NULL);

		GetVolumePathNamesForVolumeName(buf,(LPTSTR)pathnames,MAX_PATH,length);
		GetDiskFreeSpaceEx(buf,(PULARGE_INTEGER)&FBA,(PULARGE_INTEGER)&TNB,(PULARGE_INTEGER)&TNFB);
		string snbstr(ch2);
		if(TNB>fsize_min && TNB<fsize_max && snbstr.compare("FAT32")==0) {
			++drives;
			wstring temp(buf);
			dvec.push_back(temp);
		}
	}

	if(drives==32) {
		while(!dvec.empty()) {
			const WCHAR *drivename=dvec.back().c_str();
			const WCHAR *dirname=dirvec.back().c_str();
			SetVolumeMountPoint(dirname,drivename);
			dirvec.pop_back();
			dvec.pop_back();
		}
	}
	return 0;
}

