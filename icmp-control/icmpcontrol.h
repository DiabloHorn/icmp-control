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
*/
//Universal Checksum
// Mike Muuss' in_cksum() function
// and his comments from the original
// ping program
//
// * Author -
// *Mike Muuss
// *U. S. Army Ballistic Research Laboratory
// *December, 1983
USHORT checksum(USHORT *buffer, int size)
{
    unsigned long cksum=0;
    while (size > 1)
    {
        cksum += *buffer++;
        size  -= sizeof(USHORT);
    }
    if (size)
    {
        cksum += *(UCHAR*)buffer;
    }
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >>16);
    return (USHORT)(~cksum);
}

//Just the needed define for the sockopt
#define IP_HDRINCL 2

//the IP Header struct
typedef struct ipheader {
 unsigned char ip_hl:4, ip_v:4; //this means that each member is 4 bits
 unsigned char ip_tos;
 unsigned short int ip_len;
 unsigned short int ip_id;
 unsigned short int ip_off;
 unsigned char ip_ttl;
 unsigned char ip_p;
 unsigned short int ip_sum;
 unsigned int ip_src;
 unsigned int ip_dst;
}spoofediphdr;  //total ip header length: 20 bytes (=160 bits)


//The ICMP header struct
typedef struct ICMP_HDR
{
	unsigned char icmp_type;
	unsigned char icmp_code;
	unsigned short icmp_cksum;
	unsigned short icmp_id;
	unsigned short icmp_seq;
	//8bytes
}icmp_header;

//The size of the echodata that we are gonna fill
#define DATALEN 100

//The inal struct that will be sended.
typedef struct icmprequest
{
	spoofediphdr sphdr;
	icmp_header icmp;
	char echoData[DATALEN];
} icmpreq;