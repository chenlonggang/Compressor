/*============================================
# Filename: CSA.cpp
# Ver 1.0 2014-06-09
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
=============================================*/
#include"CSA.h"
#include"divsufsort.h"
#include<cstdlib>
#include<cmath>
CSA::CSA(i32 bits){
	if((bits>31)|| (bits<0)){
		cerr<<"Error bits.the wanted is [1,30]"<<endl;
		exit(0);
	}
	this->chunk_size=(1<<bits);
	this->csa=NULL;
	this->chunk_num=0;
	this->n=0;
}

i32 CSA::Compress(const char *source,const char * destation){
	FILE *fp=fopen(source,"r+");
	if(fp==NULL){
		cerr<<"Error fopen. Be sure the file is available"<<endl;
		exit(0);
	}
	fseek(fp,0,SEEK_END);
	n=ftell(fp);
	fseek(fp,0,SEEK_SET);
	chunk_num=n/chunk_size;
	if(n%chunk_size!=0)
		chunk_num++;
	i32 len=0;
	uchar *T=new uchar[chunk_size+1];
	savekit s(destation);
	s.writeu64(198809102510);
	s.writei32(n);
	s.writei32(chunk_num);

	i32 readed=0;
	for(i32 i=0;i<chunk_num;i++){
		memset(T,0,sizeof(uchar)*(chunk_size+1));
		len=min(chunk_size,n-i*chunk_size);
		readed=fread(T,sizeof(uchar),len,fp);
		if(readed!=len){
			cerr<<"Fread error"<<endl;
			exit(0);
		}
		csa=new CSA_chunk();
		csa->Compress(T,len);
		csa->Save(s);
		delete csa;
		csa=NULL;
	}
	delete [] T;
	T=NULL;
	fclose(fp);
	s.close();
	return 0;
}

i32 CSA::Decompress(const char *source,const char *destation){
	loadkit s(source);
	savekit d(destation);
	u64 magicnum;
	s.loadu64(magicnum);
	if(magicnum!=198809102510){
		cerr<<"Not a csa file"<<endl;
		s.close();
		d.close();
		exit(0);
	}
	s.loadi32(this->n);
	s.loadi32(this->chunk_num);
	i32 len=0;
	uchar * T=new uchar[chunk_size+1];
	for(int i=0;i<chunk_num;i++){
		csa=new CSA_chunk();
		csa->Load(s);
		len=csa->GetN();
		memset(T,0,sizeof(uchar)*(chunk_size+1));
		csa->Decompress(T);
		d.writebytes(T,len);
		delete csa;
	}
	delete [] T;
	T=NULL;
	csa=NULL;
	s.close();
	d.close();
	return 0;
}
