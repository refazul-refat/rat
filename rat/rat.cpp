#include <iostream>
#include <stdio.h>
#include <time.h>
using namespace std;

unsigned char SBOX[256] = 
{
	0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
	0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
	0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
	0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
	0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
	0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
	0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
	0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
	0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
	0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
	0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
	0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
	0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
	0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
	0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
	0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};
unsigned char RCON[40]=
{
	0x01,0x00,0x00,0x00,
	0x02,0x00,0x00,0x00,
	0x04,0x00,0x00,0x00,
	0x08,0x00,0x00,0x00,
	0x10,0x00,0x00,0x00,
	0x20,0x00,0x00,0x00,
	0x40,0x00,0x00,0x00,
	0x80,0x00,0x00,0x00,
	0x1b,0x00,0x00,0x00,
	0x36,0x00,0x00,0x00,
};

unsigned char TABLE[256][4];
void initLookupTable()
{
	char dump;
	FILE* fr=fopen("table.txt","r");
	FILE* fw=fopen("test.txt","w");
	for(int i=0;i<256;i++)
	{	
		unsigned int x;
		fscanf(fr,"%x",&x);
		
		TABLE[i][0]=(x & 0xFF000000)>>24;
		TABLE[i][1]=(x & 0x00FF0000)>>16;
		TABLE[i][2]=(x & 0x0000FF00)>>8;
		TABLE[i][3]=(x & 0x000000FF)>>0;
		
		fprintf(fw,"%x%x%x%x\n",TABLE[i][0],TABLE[i][1],TABLE[i][2],TABLE[i][3]);
	}
	fclose(fw);
}

unsigned int RAT[2];
unsigned int RRAT[2];
void calculateMagicInteger_s()
{
	srand(time(NULL));
	unsigned char* R=new unsigned char[16];
	unsigned char* RR=new unsigned char[16];
	for(int i=0;i<16;i++)
		R[i]=i;

	/*
	for(int i=0;i<100;i++)
	{
		int j=rand()%16;
		int k=rand()%16;
		char t=R[j];
		R[j]=R[k];
		R[k]=t;
	}
	*/
	R[0]=8;		RR[8]=0;
	R[1]=3;		RR[3]=1;
	R[2]=7;		RR[7]=2;
	R[3]=10;	RR[10]=3;
	R[4]=2;		RR[2]=4;
	R[5]=11;	RR[11]=5;
	R[6]=0;		RR[0]=6;
	R[7]=9;		RR[9]=7;
	R[8]=5;		RR[5]=8;
	R[9]=14;	RR[14]=9;
	R[10]=1;	RR[1]=10;
	R[11]=13;	RR[13]=11;
	R[12]=15;	RR[15]=12;
	R[13]=12;	RR[12]=13;
	R[14]=4;	RR[4]=14;
	R[15]=6;	RR[6]=15;

	RAT[0]=0;				RRAT[0]=0;
	RAT[0]+=(int)R[0]<<28;	RRAT[0]+=(int)RR[0]<<28;
	RAT[0]+=(int)R[1]<<24;	RRAT[0]+=(int)RR[1]<<24;
	RAT[0]+=(int)R[2]<<20;	RRAT[0]+=(int)RR[2]<<20;
	RAT[0]+=(int)R[3]<<16;	RRAT[0]+=(int)RR[3]<<16;
	RAT[0]+=(int)R[4]<<12;	RRAT[0]+=(int)RR[4]<<12;
	RAT[0]+=(int)R[5]<<8;	RRAT[0]+=(int)RR[5]<<8;
	RAT[0]+=(int)R[6]<<4;	RRAT[0]+=(int)RR[6]<<4;
	RAT[0]+=(int)R[7]<<0;	RRAT[0]+=(int)RR[7]<<0;

	RAT[1]=0;
	RAT[1]+=(int)R[8]<<28;	RRAT[1]+=(int)RR[8]<<28;
	RAT[1]+=(int)R[9]<<24;	RRAT[1]+=(int)RR[9]<<24;
	RAT[1]+=(int)R[10]<<20;	RRAT[1]+=(int)RR[10]<<20;
	RAT[1]+=(int)R[11]<<16;	RRAT[1]+=(int)RR[11]<<16;
	RAT[1]+=(int)R[12]<<12;	RRAT[1]+=(int)RR[12]<<12;
	RAT[1]+=(int)R[13]<<8;	RRAT[1]+=(int)RR[13]<<8;
	RAT[1]+=(int)R[14]<<4;	RRAT[1]+=(int)RR[14]<<4;
	RAT[1]+=(int)R[15]<<0;	RRAT[1]+=(int)RR[15]<<0;

	delete[] R;
	delete[] RR;
}

unsigned char KEY[11][16];
void generateRoundKeys(void* v)
{
	for(int i=0;i<16;i++)
		KEY[0][i]=*((unsigned char*)v+i);
	for(int i=1;i<=10;i++)
	{
		KEY[i][0]=KEY[i-1][0] ^ SBOX[KEY[i-1][13]] ^ RCON[4*(i-1)+0];
		KEY[i][1]=KEY[i-1][1] ^ SBOX[KEY[i-1][14]] ^ RCON[4*(i-1)+1];
		KEY[i][2]=KEY[i-1][2] ^ SBOX[KEY[i-1][15]] ^ RCON[4*(i-1)+2];
		KEY[i][3]=KEY[i-1][3] ^ SBOX[KEY[i-1][12]] ^ RCON[4*(i-1)+3];

		KEY[i][4]=KEY[i-1][4] ^ KEY[i][0];
		KEY[i][5]=KEY[i-1][5] ^ KEY[i][1];
		KEY[i][6]=KEY[i-1][6] ^ KEY[i][2];
		KEY[i][7]=KEY[i-1][7] ^ KEY[i][3];

		KEY[i][8]=KEY[i-1][8] ^ KEY[i][4];
		KEY[i][9]=KEY[i-1][9] ^ KEY[i][5];
		KEY[i][10]=KEY[i-1][10] ^ KEY[i][6];
		KEY[i][11]=KEY[i-1][11] ^ KEY[i][7];

		KEY[i][12]=KEY[i-1][12] ^ KEY[i][8];
		KEY[i][13]=KEY[i-1][13] ^ KEY[i][9];
		KEY[i][14]=KEY[i-1][14] ^ KEY[i][10];
		KEY[i][15]=KEY[i-1][15] ^ KEY[i][11];
		
		for(int j=0;j<16;j++)
		{
			cout<<hex<<(unsigned int)KEY[i][j]<<" ";
		}
		cout<<endl;
	}
	cout<<endl;
}

void encrypt(void* v)
{
	unsigned char* r0=new unsigned char[16];
	for(int i=0;i<16;i++)
		r0[i]=*((unsigned char*)v+i);

	unsigned char* r1=new unsigned char[16];
	for(int i=0;i<16;i++)
	{
		r1[i]=r0[i] ^ KEY[0][i];
		//cout<<hex<<(unsigned int)r1[i]<<" ";
	}
	//cout<<endl;

	delete[] r0;
	
	for(int i=1;i<=9;i++)
	{
		unsigned char* temp=new unsigned char[16];

		temp[0]=TABLE[r1[0]][0] ^ TABLE[r1[5]][3] ^ TABLE[r1[10]][2] ^ TABLE[r1[15]][1] ^ KEY[i][0];
		temp[1]=TABLE[r1[0]][1] ^ TABLE[r1[5]][0] ^ TABLE[r1[10]][3] ^ TABLE[r1[15]][2] ^ KEY[i][1];
		temp[2]=TABLE[r1[0]][2] ^ TABLE[r1[5]][1] ^ TABLE[r1[10]][0] ^ TABLE[r1[15]][3] ^ KEY[i][2];
		temp[3]=TABLE[r1[0]][3] ^ TABLE[r1[5]][2] ^ TABLE[r1[10]][1] ^ TABLE[r1[15]][0] ^ KEY[i][3];

		temp[4]=TABLE[r1[4]][0] ^ TABLE[r1[9]][3] ^ TABLE[r1[14]][2] ^ TABLE[r1[3]][1] ^ KEY[i][4];
		temp[5]=TABLE[r1[4]][1] ^ TABLE[r1[9]][0] ^ TABLE[r1[14]][3] ^ TABLE[r1[3]][2] ^ KEY[i][5];
		temp[6]=TABLE[r1[4]][2] ^ TABLE[r1[9]][1] ^ TABLE[r1[14]][0] ^ TABLE[r1[3]][3] ^ KEY[i][6];
		temp[7]=TABLE[r1[4]][3] ^ TABLE[r1[9]][2] ^ TABLE[r1[14]][1] ^ TABLE[r1[3]][0] ^ KEY[i][7];

		temp[8]=TABLE[r1[8]][0] ^ TABLE[r1[13]][3] ^ TABLE[r1[2]][2] ^ TABLE[r1[7]][1] ^ KEY[i][8];
		temp[9]=TABLE[r1[8]][1] ^ TABLE[r1[13]][0] ^ TABLE[r1[2]][3] ^ TABLE[r1[7]][2] ^ KEY[i][9];
		temp[10]=TABLE[r1[8]][2] ^ TABLE[r1[13]][1] ^ TABLE[r1[2]][0] ^ TABLE[r1[7]][3] ^ KEY[i][10];
		temp[11]=TABLE[r1[8]][3] ^ TABLE[r1[13]][2] ^ TABLE[r1[2]][1] ^ TABLE[r1[7]][0] ^ KEY[i][11];

		temp[12]=TABLE[r1[12]][0] ^ TABLE[r1[1]][3] ^ TABLE[r1[6]][2] ^ TABLE[r1[11]][1] ^ KEY[i][12];
		temp[13]=TABLE[r1[12]][1] ^ TABLE[r1[1]][0] ^ TABLE[r1[6]][3] ^ TABLE[r1[11]][2] ^ KEY[i][13];
		temp[14]=TABLE[r1[12]][2] ^ TABLE[r1[1]][1] ^ TABLE[r1[6]][0] ^ TABLE[r1[11]][3] ^ KEY[i][14];
		temp[15]=TABLE[r1[12]][3] ^ TABLE[r1[1]][2] ^ TABLE[r1[6]][1] ^ TABLE[r1[11]][0] ^ KEY[i][15];

		for(int j=0;j<16;j++)
		{
			r1[j]=temp[j];
			//cout<<hex<<(unsigned int)r1[j]<<" ";
		}
		//cout<<endl;

		delete[] temp;
	}
	unsigned char* temp=new unsigned char[16];

	temp[0]=SBOX[r1[0]] ^ KEY[10][0];
	temp[1]=SBOX[r1[5]] ^ KEY[10][1];
	temp[2]=SBOX[r1[10]] ^ KEY[10][2];
	temp[3]=SBOX[r1[15]] ^ KEY[10][3];

	temp[4]=SBOX[r1[4]] ^ KEY[10][4];
	temp[5]=SBOX[r1[9]] ^ KEY[10][5];
	temp[6]=SBOX[r1[14]] ^ KEY[10][6];
	temp[7]=SBOX[r1[3]] ^ KEY[10][7];

	temp[8]=SBOX[r1[8]] ^ KEY[10][8];
	temp[9]=SBOX[r1[13]] ^ KEY[10][9];
	temp[10]=SBOX[r1[2]] ^ KEY[10][10];
	temp[11]=SBOX[r1[7]] ^ KEY[10][11];

	temp[12]=SBOX[r1[12]] ^ KEY[10][12];
	temp[13]=SBOX[r1[1]] ^ KEY[10][13];
	temp[14]=SBOX[r1[6]] ^ KEY[10][14];
	temp[15]=SBOX[r1[11]] ^ KEY[10][15];

	for(int j=0;j<16;j++)
	{
		r1[j]=temp[j];
		*((unsigned char*)v+j)=r1[j];
		//cout<<hex<<(unsigned int)r1[j]<<" ";
	}
	//cout<<endl<<endl<<endl;

	delete[] temp;
}
unsigned char f(unsigned char i)
{
	unsigned char j=1;
	if((i & 128) == 0)
		j=0;
	//unsigned char k=((255-i)>>4)<<2;
	//unsigned char x=(RAT[j]>>k) & 15;
	//unsigned char y=i & 15;
	//return (x<<4) + y;
	return (((RAT[j]>>(((255-i)>>4)<<2)) & 15)<<4)+(i & 15);
}
unsigned char fi(unsigned char i)
{
	unsigned char j=1;
	if((i & 128) == 0)
		j=0;
	//unsigned char k=((255-i)>>4)<<2;
	//unsigned char x=(RAT[j]>>k) & 15;
	//unsigned char y=i & 15;
	//return (x<<4) + y;
	return (((RRAT[j]>>(((255-i)>>4)<<2)) & 15)<<4)+(i & 15);
}
void protectedEncrypt(void* v)
{
	unsigned char* r0=new unsigned char[16];
	for(int i=0;i<16;i++)
		r0[i]=*((unsigned char*)v+i);

	unsigned char* r1=new unsigned char[16];
	for(int i=0;i<16;i++)
	{
		r1[i]=r0[i] ^ KEY[0][i];
		//cout<<hex<<(unsigned int)r1[i]<<" ";
	}
	//cout<<endl;

	delete[] r0;
	
	for(int i=1;i<=9;i++)
	{
		unsigned char* temp=new unsigned char[16];

		temp[0]=TABLE[(unsigned int)fi(r1[0])][0] ^ TABLE[(unsigned int)fi(r1[5])][3] ^ TABLE[(unsigned int)fi(r1[10])][2] ^ TABLE[(unsigned int)fi(r1[15])][1] ^ KEY[i][0];
		temp[1]=TABLE[(unsigned int)fi(r1[0])][1] ^ TABLE[(unsigned int)fi(r1[5])][0] ^ TABLE[(unsigned int)fi(r1[10])][3] ^ TABLE[(unsigned int)fi(r1[15])][2] ^ KEY[i][1];
		temp[2]=TABLE[(unsigned int)fi(r1[0])][2] ^ TABLE[(unsigned int)fi(r1[5])][1] ^ TABLE[(unsigned int)fi(r1[10])][0] ^ TABLE[(unsigned int)fi(r1[15])][3] ^ KEY[i][2];
		temp[3]=TABLE[(unsigned int)fi(r1[0])][3] ^ TABLE[(unsigned int)fi(r1[5])][2] ^ TABLE[(unsigned int)fi(r1[10])][1] ^ TABLE[(unsigned int)fi(r1[15])][0] ^ KEY[i][3];

		temp[4]=TABLE[(unsigned int)fi(r1[4])][0] ^ TABLE[(unsigned int)fi(r1[9])][3] ^ TABLE[(unsigned int)fi(r1[14])][2] ^ TABLE[(unsigned int)fi(r1[3])][1] ^ KEY[i][4];
		temp[5]=TABLE[(unsigned int)fi(r1[4])][1] ^ TABLE[(unsigned int)fi(r1[9])][0] ^ TABLE[(unsigned int)fi(r1[14])][3] ^ TABLE[(unsigned int)fi(r1[3])][2] ^ KEY[i][5];
		temp[6]=TABLE[(unsigned int)fi(r1[4])][2] ^ TABLE[(unsigned int)fi(r1[9])][1] ^ TABLE[(unsigned int)fi(r1[14])][0] ^ TABLE[(unsigned int)fi(r1[3])][3] ^ KEY[i][6];
		temp[7]=TABLE[(unsigned int)fi(r1[4])][3] ^ TABLE[(unsigned int)fi(r1[9])][2] ^ TABLE[(unsigned int)fi(r1[14])][1] ^ TABLE[(unsigned int)fi(r1[3])][0] ^ KEY[i][7];

		temp[8]=TABLE[(unsigned int)fi(r1[8])][0] ^ TABLE[(unsigned int)fi(r1[13])][3] ^ TABLE[(unsigned int)fi(r1[2])][2] ^ TABLE[(unsigned int)fi(r1[7])][1] ^ KEY[i][8];
		temp[9]=TABLE[(unsigned int)fi(r1[8])][1] ^ TABLE[(unsigned int)fi(r1[13])][0] ^ TABLE[(unsigned int)fi(r1[2])][3] ^ TABLE[(unsigned int)fi(r1[7])][2] ^ KEY[i][9];
		temp[10]=TABLE[(unsigned int)fi(r1[8])][2] ^ TABLE[(unsigned int)fi(r1[13])][1] ^ TABLE[(unsigned int)fi(r1[2])][0] ^ TABLE[(unsigned int)fi(r1[7])][3] ^ KEY[i][10];
		temp[11]=TABLE[(unsigned int)fi(r1[8])][3] ^ TABLE[(unsigned int)fi(r1[13])][2] ^ TABLE[(unsigned int)fi(r1[2])][1] ^ TABLE[(unsigned int)fi(r1[7])][0] ^ KEY[i][11];

		temp[12]=TABLE[(unsigned int)fi(r1[12])][0] ^ TABLE[(unsigned int)fi(r1[1])][3] ^ TABLE[(unsigned int)fi(r1[6])][2] ^ TABLE[(unsigned int)fi(r1[11])][1] ^ KEY[i][12];
		temp[13]=TABLE[(unsigned int)fi(r1[12])][1] ^ TABLE[(unsigned int)fi(r1[1])][0] ^ TABLE[(unsigned int)fi(r1[6])][3] ^ TABLE[(unsigned int)fi(r1[11])][2] ^ KEY[i][13];
		temp[14]=TABLE[(unsigned int)fi(r1[12])][2] ^ TABLE[(unsigned int)fi(r1[1])][1] ^ TABLE[(unsigned int)fi(r1[6])][0] ^ TABLE[(unsigned int)fi(r1[11])][3] ^ KEY[i][14];
		temp[15]=TABLE[(unsigned int)fi(r1[12])][3] ^ TABLE[(unsigned int)fi(r1[1])][2] ^ TABLE[(unsigned int)fi(r1[6])][1] ^ TABLE[(unsigned int)fi(r1[11])][0] ^ KEY[i][15];

		for(int j=0;j<16;j++)
		{
			r1[j]=temp[j];
			//cout<<hex<<(unsigned int)r1[j]<<" ";
		}
		//cout<<endl;

		delete[] temp;
	}
	unsigned char* temp=new unsigned char[16];

	temp[0]=SBOX[r1[0]] ^ KEY[10][0];
	temp[1]=SBOX[r1[5]] ^ KEY[10][1];
	temp[2]=SBOX[r1[10]] ^ KEY[10][2];
	temp[3]=SBOX[r1[15]] ^ KEY[10][3];

	temp[4]=SBOX[r1[4]] ^ KEY[10][4];
	temp[5]=SBOX[r1[9]] ^ KEY[10][5];
	temp[6]=SBOX[r1[14]] ^ KEY[10][6];
	temp[7]=SBOX[r1[3]] ^ KEY[10][7];

	temp[8]=SBOX[r1[8]] ^ KEY[10][8];
	temp[9]=SBOX[r1[13]] ^ KEY[10][9];
	temp[10]=SBOX[r1[2]] ^ KEY[10][10];
	temp[11]=SBOX[r1[7]] ^ KEY[10][11];

	temp[12]=SBOX[r1[12]] ^ KEY[10][12];
	temp[13]=SBOX[r1[1]] ^ KEY[10][13];
	temp[14]=SBOX[r1[6]] ^ KEY[10][14];
	temp[15]=SBOX[r1[11]] ^ KEY[10][15];

	for(int j=0;j<16;j++)
	{
		r1[j]=temp[j];
		*((unsigned char*)v+j)=r1[j];
		//cout<<hex<<(unsigned int)r1[j]<<" ";
	}

	delete[] temp;
}

void mixItUp()
{
	unsigned char TEMP[256][4];
	for(int i=0;i<256;i++)
	{
		for(int j=0;j<4;j++)
			TEMP[i][j]=TABLE[(unsigned int)f(i)][j];
	}
	
	FILE* fw=fopen("mixed.txt","w");
	for(int i=0;i<256;i++)
	{
		for(int j=0;j<4;j++)
		{
			TABLE[i][j]=TEMP[i][j];
			fprintf(fw,"%x",TABLE[i][j]);
		}
		fprintf(fw,"\n");
	}
}


int main()
{
	initLookupTable();
	calculateMagicInteger_s();
	unsigned char K[]={0x0f,0x15,0x71,0xc9,0x47,0xd9,0xe8,0x59,0x0c,0xb7,0xad,0xd6,0xaf,0x7f,0x67,0x98};
	generateRoundKeys(&K);
	
	//cout<<dec;
	//cout<<RAT[0]<<endl;
	//cout<<RAT[1]<<endl;
	//cout<<RAT[2]<<endl;
	//cout<<RAT[3]<<endl;
	//cout<<dec;
	for(int M=4;M<=4;M++){
	
	int it=0,index=0,blocks=0,threshold=M*1024*64,max=M*1024*1024;
	unsigned char* BUFFER=new unsigned char[max];
	unsigned char c;
	FILE* fr=fopen("plain.txt","r");
	FILE* fw=fopen("cipher.txt","w");
	while(true){
		c=fgetc(fr);
		if(blocks>threshold)break;
		BUFFER[index++]=c;

		if(it==15)
			blocks++;
		it=(it+1)%16;
	}
	fclose(fw);
	fclose(fr);

	//mixItUp();

		for(int z=0;z<10;z++){
			const clock_t begin_time = clock();
			std::cout << "Starting..." <<endl;

			for(index=0;index<max;index+=16){
				encrypt(&BUFFER[index]);
			}

			std::cout << dec;
			std::cout << (z+1) << " " << float( clock () - begin_time ) /  CLOCKS_PER_SEC<<endl;
		}
	}
	return 0;

	unsigned char PLAIN[]=
	{
		0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10
	};
	encrypt(&PLAIN);
	protectedEncrypt(&PLAIN);
	
	
	

	
	return 0;
}