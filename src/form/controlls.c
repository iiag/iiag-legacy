//
// controlls.c
//

void readFile(const char* filename){
	FILE* file=fopen(filename,"rb");
	
	if(!fshade){
    fprintf(stderr,"unable to open file '%s'\n",filename);
    return NULL;
  }
  
  fseek(file, 0, SEEK_END);
  long filesize=ftell(file);
  fseek(file, 0, SEEK_SET);
  
  const char* source=new char[filesize];
  fread(source,1,filesize,file);
 	
 	fclose(file); 
  source[filesize]=0;
  
	return const_cast<const char*>(source);
}



void parseSource(const char* source){
  int len=strlen(source);
  char* temp=(char *)malloc(4*sizeof(char));
  control=(charInfo *)malloc(sizeof(control));
  int j=0;
	for(int i=0;i<len;i++){
		temp[i]=source[i];
		temp=(char *)realloc(temp,4*sizeof(char));
		if(source[i]=='='){
			control[j].nameType=temp;
			control[j].ch=source[i++];
			control=(charInfo *)realloc(sizeof(control));
			temp=[0];
			j++;
		}
	}
	control=(charInfo *)realloc(sizeof(control));
	control[j].nameType="NULL";
  control[j].ch='?';
}

void createFile(const char* filename){
	FILE* file=fopen(filename,"wb");
	while(control->ch!='?'){
		fprintf(file,"%s=%c\n",control->nameType,control->ch);
	}
	fclose(file);
}

