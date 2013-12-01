/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+This is a little Disclaimer for if you havn't read the one on our site.	   +
+The tools and tutorials KD-Team develops and publishes are only ment for          +
+educational purpose only.WE DO NOT encourage the use of this tools and       	   +
+tutorials for mailicious purpose.We learned a lot during the development of them  +
+so we hope you also learn and don't just use it without any brains.		   +
+We take completly NO responsability for any damage caused by them nor	           +
+are we or our isp responsible for what you do with them.  			   +
+Greetz: KD-Team						                   +
+http://www.kd-team.com					                           +
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Made By DiabloHorn
Thx to vnet576 for helping out with a few things.
NOTE: this has only been tested succesfull on lans
it seems that most ip's block outgoing packets with spoofed ip.

This has no error checking so just use like:
spoofip.exe destination source

Excelent reference on coding with win32 raw socket:

http://blacksun.box.sk
- Windows Internet Programming, Written by: Binary Rape
*/

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "icmpcontrol.h"

#pragma comment (lib, "ws2_32")
// This is the global define for the kinda icmp packet we are gonna send
//it's a echo request.

#define TYPEREQUEST 8

void main(int argc,char *argv[])
{
	WSADATA wsa; //normal windows startup thingie for sockets
	SOCKET sock; // the socket itself
	SOCKADDR_IN dinfo; //inet protocol structure
	char sbuffer[DATALEN+28]; //buffer we are gonna send
	icmpreq *icmp_packet;
	BOOL bopt = 1; //setting the sockopt to true

//normal wsastartup on windows
	if(WSAStartup(MAKEWORD(2,0),&wsa) != 0)
	{
		printf("WSAStartup() failed\n");
	}
//normal socket making
	if((sock = socket(AF_INET,SOCK_RAW,IPPROTO_RAW)) == SOCKET_ERROR)
	{
		printf("Socket failed %d\n",GetLastError());
		exit(0);
	}
//preparing the buffer to hold our packet.
	icmp_packet = (icmpreq*)sbuffer;
//defining the sendto ip
	dinfo.sin_family = AF_INET;
	dinfo.sin_port = htons(0);
	dinfo.sin_addr.s_addr = inet_addr(argv[1]);
//just some general debug info
	printf("spoofed header: %i\n",sizeof(icmp_packet->sphdr));
	printf("icmp header %i\n",sizeof(icmp_packet->icmp));
	printf("echo data: %i\n",sizeof(icmp_packet->echoData));
	printf("total: %i\n",(sizeof(icmp_packet->sphdr) + sizeof(icmp_packet->icmp) + sizeof(icmp_packet->echoData)));
//Here we are filling in the ip header with the data.
	icmp_packet->sphdr.ip_hl = 5;
	icmp_packet->sphdr.ip_v = 4;
	icmp_packet->sphdr.ip_tos = 0;
	icmp_packet->sphdr.ip_len = sizeof(icmp_packet->sphdr) + sizeof(icmp_packet->icmp) + sizeof(icmp_packet->echoData) ;
	icmp_packet->sphdr.ip_id = 1;
	icmp_packet->sphdr.ip_off = 0;
	icmp_packet->sphdr.ip_ttl = 255;
	icmp_packet->sphdr.ip_p = IPPROTO_ICMP;
	icmp_packet->sphdr.ip_sum = 0;
//The following line is where we spoofe the sender ip like you can see it's just 1 line;)
	icmp_packet->sphdr.ip_src = inet_addr(argv[2]);
	icmp_packet->sphdr.ip_dst = dinfo.sin_addr.s_addr;
//Here we are filling the icmp header.
	icmp_packet->icmp.icmp_type = TYPEREQUEST;
	icmp_packet->icmp.icmp_code = 0;
	icmp_packet->icmp.icmp_seq = (unsigned short)GetTickCount(); //GetTickCount() is just to make shure it's unique
	icmp_packet->icmp.icmp_id = (unsigned short)GetTickCount();
//Filling the echodata buffer with a's can also be filled with other things as well ;)
	memset(icmp_packet->echoData,'a',DATALEN);
	memcpy(icmp_packet->echoData,argv[3],strlen(argv[3]));
//Calculating the checksum for the icmp header.
	icmp_packet->icmp.icmp_cksum = checksum((unsigned short*)sbuffer,sizeof(icmpreq));
//Setting the correct options for the socket. Thus making it take our own ip header
//instead of the normal one.
	if(setsockopt(sock,IPPROTO_IP,IP_HDRINCL,(char *)&bopt,sizeof(bopt)) == SOCKET_ERROR)
	{
		printf("IP_HDRINCL failed\n");
	}
//and finallay sending the entire packet to the destination host.
	if(sendto(sock,sbuffer,sizeof(icmpreq),0,(SOCKADDR*)&dinfo,sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		printf("sendto error %d\n",GetLastError());
		exit(0);
	}
//Closing and cleaning up the socket and wsa
	closesocket(sock);
	WSACleanup();
}