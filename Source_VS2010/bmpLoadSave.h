//bmpLoadSave.h
//�ő�T�C�Y
#define MAX_IMG_WIDTH  256
#define MAX_IMG_HEIGHT 256

struct Pixel
{
	unsigned char b, g, r;
};

struct PixelA
{
	unsigned char b, g, r, a;
};

struct Palette
{
	unsigned char  r, g, b, a;
};

struct FILEHEADER
{
	unsigned short Type;       //file type
	unsigned long  Size;       //file size
	unsigned short Reserved1;  //�\��
	unsigned short Reserved2;  //�\��
	unsigned long  OffBits;    //�s�N�Z���f�[�^�ւ̃I�t�Z�b�g
};
	
struct INFOHEADER
{
	unsigned long  Size;        //BMPINFOHEADER size
	long           Width;       //bitmap �̕�
	long           Height;      //bitmap �̍���
	unsigned short Planes;      //�J���[�v���[����
	unsigned short BitCount;    //�F�̐�
	unsigned long  Compression; //���k�`��
	unsigned long  SizeImage;   //image size
	long           XPixPerMeter;//�����𑜓x
	long           YPixPerMeter;//�����𑜓x
	unsigned long  ClrUsed;     //�p���b�g��
	unsigned long  ClrImportant;//
};

struct Bitmap
{
	FILEHEADER bf;
	INFOHEADER bi;
	struct Pixel* pixel;
	struct PixelA* pixelA;
};

static int getByte(FILE *f)
{
	return fgetc(f) & 0xFF;
}

static int get2Byte(FILE *f)
{
	int c1 = fgetc(f) & 0xFF;
	int c2 = fgetc(f) & 0xFF;
	return  (c1 | (c2 << 8));
//	return fgetc(f) | fgetc(f) << 8;
}

static int get4Byte(FILE *f)
{
	int c1 = fgetc(f) & 0xFF;
	int c2 = fgetc(f) & 0xFF;
	int c3 = fgetc(f) & 0xFF;
	int c4 = fgetc(f) & 0xFF;
	return (c1 | (c2 << 8) | (c3 << 16) | (c4 << 24));
//	return fgetc(f) | fgetc(f) << 8 | fgetc(f) << 16 | fgetc(f) << 24;
}
//------------------------------------------------------------------
void loadBitmap(Bitmap* bm, const char *filename)
{
	FILE *fp = fopen( filename, "rb");

	if(!fp){
		fprintf(stderr, "�w�肵���t�@�C�������Ԉ���Ă��܂� %s\n", filename);
	}

	if(get2Byte(fp) != 0x4D42)//BM��(B=42, M=4D)
	{
		fprintf(stderr, "%s ��BMP�t�@�C���ł͂Ȃ�\n", filename);
	}

	bm->bf.Size = get4Byte(fp);
	bm->bf.Reserved1 = get2Byte(fp);
	bm->bf.Reserved2 = get2Byte(fp);
	bm->bf.OffBits = get4Byte(fp);
	bm->bi.Size = get4Byte(fp);
	bm->bi.Width = get4Byte(fp);
	bm->bi.Height = get4Byte(fp);
	bm->bi.Planes = get2Byte(fp);
	bm->bi.BitCount = get2Byte(fp);
	bm->bi.Compression = get4Byte(fp);
	bm->bi.SizeImage = get4Byte(fp);
	bm->bi.XPixPerMeter = get4Byte(fp);
	bm->bi.YPixPerMeter = get4Byte(fp);
	bm->bi.ClrUsed = get4Byte(fp);
	bm->bi.ClrImportant = get4Byte(fp);

	if(bm->bi.Compression != 0) {
		printf("���k�`���̓T�|�[�g���Ă��܂��� \n");
		return;
	}
	//padding�̃o�C�g��
	int nBite, pad;
	nBite = bm->bi.Width * bm->bi.BitCount / 8;
	pad = 4 - (nBite - 4 * (nBite / 4));
	if(pad == 4) pad = 0;

	if(bm->bi.BitCount <= 8)
	{
		bm->pixel = (Pixel*)malloc(3 * (bm->bi.Width) * abs(bm->bi.Height));
		int paletteSize;//�F��
		if(bm->bi.ClrUsed == 0) paletteSize = 1 << bm->bi.BitCount;
		else paletteSize = bm->bi.ClrUsed;
		Palette* plt = (Palette*)malloc(sizeof(struct Palette) * paletteSize);
		int i, x, y;
		for(i = 0; i < paletteSize; i++){
			plt[i].b = fgetc(fp);
			plt[i].g = fgetc(fp);
			plt[i].r = fgetc(fp);
			plt[i].a = fgetc(fp);
		}
		if(bm->bi.BitCount == 1)//1bit mono
		{
			//padding�̃r�b�g,�o�C�g��
			int padBite;
			int mBite = bm->bi.Width / 8;
			int mBit = bm->bi.Width - 8 * mBite;
			if(mBit == 0) padBite = 4 - ( mBite - 4 * (mBite / 4));
			else padBite = 4 - (mBite + 1 - 4 * ((mBite + 1) / 4));
			if(padBite == 4) padBite = 0;

			int c, d, e, k;
			int n = 0;
			for(y = 0; y < abs(bm->bi.Height); y++)
			{
				for(x = 0; x < bm->bi.Width / 8; x++)
				{
					c = fgetc(fp);
					for(k = 0; k < 8; k++)
					{	
						d = c & 0x80;//�ŏ�ʃr�b�g
						if(d == 0) e = 0;
						else e = 255;
						bm->pixel[n].b = (unsigned char)e;
						bm->pixel[n].g = (unsigned char)e;
						bm->pixel[n].r = (unsigned char)e;
						n++;
						c = c << 1;//1�r�b�g���V�t�g
					}
				}
				//���܂蕪
				if(mBit != 0)
				{
					c = fgetc(fp);
					for(k = 0; k < mBit; k++)
					{	
						d = c & 0x80;//�ŏ�ʃr�b�g
						if(d == 0) e = 0;
						else e = 255;
						bm->pixel[n].b = (unsigned char)e;
						bm->pixel[n].g = (unsigned char)e;
						bm->pixel[n].r = (unsigned char)e;
						n++;
						c = c << 1;//1�r�b�g���V�t�g
					}
				}
				for(x = 0; x < padBite; x++) c = fgetc(fp);//����ǂ�
			}
		}
		else if(bm->bi.BitCount == 8)//8bit mono or color
		{
			unsigned char c;
			for(y = 0; y < abs(bm->bi.Height); y++)
			{
				for(x = 0; x < bm->bi.Width; x++)
				{
					c = getByte(fp) ;
					Palette p = plt[c];
					bm->pixel[x + y * bm->bi.Width].b = p.b;
					bm->pixel[x + y * bm->bi.Width].g = p.g;
					bm->pixel[x + y * bm->bi.Width].r = p.r;
				}
				//padding��ǂݔ�΂�
			  for(int x = 0; x < pad; x++) c = getByte(fp);
			}
		}
		free(plt);
	}
	else if(bm->bi.BitCount == 24)//24bit color
	{
		bm->pixel = (Pixel*)malloc(3 * bm->bi.Width * abs(bm->bi.Height));

		for(int y = 0; y < abs(bm->bi.Height); y++)
		{
			for(int x = 0; x < bm->bi.Width; x++)
			{
				bm->pixel[x + y * bm->bi.Width].b = getByte(fp);
				bm->pixel[x + y * bm->bi.Width].g = getByte(fp);
				bm->pixel[x + y * bm->bi.Width].r = getByte(fp);
			}
			//padding��ǂݔ�΂�
			unsigned char c;
			for(int x = 0; x < pad; x++) c = getByte(fp);
		}
	}

	else//32bit color
	{
		bm->pixelA = (PixelA*)malloc(4 * bm->bi.Width * abs(bm->bi.Height));

		fread(bm->pixelA, 1, 4 * bm->bi.Width * abs(bm->bi.Height), fp);
	}
	fclose(fp);
}

//�摜�f�[�^��ǂݍ���Ŕz��Ɋi�[(Color�p�j
void loadColorData( GLubyte image[][3], int *imgWidth, int *imgHeight,const char *filename)
{
  Bitmap* bm = new Bitmap();
  loadBitmap(bm, filename);

	int width = bm->bi.Width;
	int height = abs(bm->bi.Height);

	if(width > MAX_IMG_WIDTH || height > MAX_IMG_HEIGHT)
	{
		printf("�摜�T�C�Y���傫�����܂� \n");
		return;
	}
	*imgWidth = width;
	*imgHeight = height;

  int i, j;

  for(j = 0; j < height; j++)
  {
		for(i = 0; i < width; i++)
		{
			if(bm->bi.BitCount <= 24)
			{
				image[i + j * width][0] = (bm->pixel[i+j*bm->bi.Width]).r;
				image[i + j * width][1] = (bm->pixel[i+j*bm->bi.Width]).g;
				image[i + j * width][2] = (bm->pixel[i+j*bm->bi.Width]).b;
			}
			else
			{
				image[i + j * width][0] = (bm->pixelA[i+j*bm->bi.Width]).r;
				image[i + j * width][1] = (bm->pixelA[i+j*bm->bi.Width]).g;
				image[i + j * width][2] = (bm->pixelA[i+j*bm->bi.Width]).b;
			}
		}
  }
if(bm->bi.BitCount <= 24) free(bm->pixel);
else free(bm->pixelA);
}

//�摜�f�[�^��ǂݍ���Ŕz��Ɋi�[(32bitColor�p�j
void loadColorData32( GLubyte image[][4], int *imgWidth, int *imgHeight,const char *filename)
{
  Bitmap* bm = new Bitmap();
  loadBitmap(bm, filename);

	int width = bm->bi.Width;
	int height = abs(bm->bi.Height);

	if(width > MAX_IMG_WIDTH || height > MAX_IMG_HEIGHT)
	{
		printf("�摜�T�C�Y���傫�����܂� \n");
		return;
	}
	*imgWidth = width;
	*imgHeight = height;

  int i, j;

  for(j = 0; j < height; j++)
  {
		for(i = 0; i < width; i++)
    {
			if(bm->bi.BitCount <= 24)
			{
				image[i + j * width][0] = (bm->pixel[i+j*bm->bi.Width]).r;
				image[i + j * width][1] = (bm->pixel[i+j*bm->bi.Width]).g;
				image[i + j * width][2] = (bm->pixel[i+j*bm->bi.Width]).b;
			}
			else
			{
				image[i + j * width][0] = (bm->pixelA[i+j*bm->bi.Width]).r;
				image[i + j * width][1] = (bm->pixelA[i+j*bm->bi.Width]).g;
				image[i + j * width][2] = (bm->pixelA[i+j*bm->bi.Width]).b;
				image[i + j * width][3] = (bm->pixelA[i+j*bm->bi.Width]).a;
			}
    }
  }
if(bm->bi.BitCount <= 24) free(bm->pixel);
else free(bm->pixelA);
}

//color�摜�f�[�^��ǂݍ����grayscale�z��Ɋi�[
void loadGrayData(GLubyte image[], int *imgWidth, int *imgHeight, const char *filename)
{
  Bitmap* bm = new Bitmap();
  loadBitmap(bm, filename);

	int width = bm->bi.Width;
	int height = bm->bi.Height;

	if(width > MAX_IMG_WIDTH || height > MAX_IMG_HEIGHT)
	{
		printf("�摜�T�C�Y���傫�����܂� \n");
		return;
	}
	*imgWidth = width;
	*imgHeight = height;

  int i, j;
	float r, g,b;

  for(j = 0; j < height; j++)
  {
		for(i = 0; i < width; i++)
    {
			if(bm->bi.BitCount <= 24)
			{
				r = (float)(bm->pixel[i+j*bm->bi.Width]).r;
				g = (float)(bm->pixel[i+j*bm->bi.Width]).g;
				b = (float)(bm->pixel[i+j*bm->bi.Width]).b;
			}
			else//32bit
			{
				r = (float)(bm->pixelA[i+j*bm->bi.Width]).r;
				g = (float)(bm->pixelA[i+j*bm->bi.Width]).g;
				b = (float)(bm->pixelA[i+j*bm->bi.Width]).b;
			}
			image[i + j * width] = (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);
    }
  }
if(bm->bi.BitCount <= 24) free(bm->pixel);
else free(bm->pixelA);
}

//�ȉ��͉摜�ۑ�
//1bit(2�~���P�F�j�p�̕ۑ�
void saveBitmap1(int imageWidth, int imageHeight, GLubyte image[], const char *filename)
{
	//padding�̃r�b�g,�o�C�g��
	int padBite;
	int mBite = imageWidth / 8;
	int mBit = imageWidth - 8 * mBite;
	if(mBit == 0) padBite = 4 - ( mBite - 4 * (mBite / 4));
	else padBite = 4 - (mBite + 1 - 4 * ((mBite + 1) / 4));
	if(padBite == 4) padBite = 0;

	int i, j, k;
  unsigned char c;

	FILE *fp = fopen( filename, "wb");
  if(fp == NULL)
  {
		printf("not file opened\n");
		return;
  }

	Bitmap* bm = new Bitmap();

//�o�͗p��BITMAPFILEHEADER�̍쐬 
  bm->bf.Type = 0x4D42;
	bm->bf.Size = (unsigned long)54 + 8 + (imageWidth * imageHeight)/8 ;//8�Ŋ����ăo�C�g���ɕς��A�p���b�g�f�[�^8�o�C�g������
	bm->bf.OffBits = 54+8;//�p���b�g�f�[�^8�o�C�g������
  bm->bf.Reserved1 = 0;
  bm->bf.Reserved2 = 0;

  fwrite(&bm->bf.Type,sizeof(unsigned short),1,fp);
  fwrite(&bm->bf.Size,sizeof(unsigned long),1,fp);
  fwrite(&bm->bf.Reserved1,sizeof(unsigned short),1,fp);
  fwrite(&bm->bf.Reserved2,sizeof(unsigned short),1,fp);
  fwrite(&bm->bf.OffBits,sizeof(unsigned long),1,fp);
	
//�o�͗pBITMAPINFOHEADER�̍쐬
  bm->bi.Size = 40;
  bm->bi.Width = imageWidth;
  bm->bi.Height = imageHeight;
  bm->bi.Planes = 1;
  bm->bi.BitCount=1;
  bm->bi.Compression = 0;//�����k
  bm->bi.SizeImage = 0;
  bm->bi.XPixPerMeter = 0;
  bm->bi.YPixPerMeter = 0;
	bm->bi.ClrUsed = 0;
	bm->bi.ClrImportant = 0;
	fwrite(&bm->bi, sizeof(INFOHEADER), 1, fp);

	//�f�[�^�̏�������
	//�p���b�g���(�P�F2�~���j
	fputc(0, fp); fputc(0, fp); fputc(0, fp); fputc(0, fp);//����RGB�ƃ_�~�[
	fputc(255, fp); fputc(255, fp); fputc(255, fp); fputc(0, fp);//����RGB�ƃ_�~�[
	//image data�̏o��
	for (j = 0;j < imageHeight; j++)
	{
		for (i = 0;i < imageWidth/8; i++) 
		{
			c = 0;
			//1byte���̃f�[�^�쐬
			for(k = 0; k < 8; k++)
			{
				c = c << 1;//1bit���փV�t�g
				c += (image[j * imageWidth + i*8 + k] + 127)/255; 
			}
			fputc(c, fp); 
		}
		//���܂蕪
		if(mBit != 0)
		{
			for(k = 0; k < mBit; k++)
			{	
				c = c << 1;//1bit���փV�t�g
				c += (image[j * imageWidth + i*8 + k] + 127)/255; 
			}
			fputc(c, fp); 
		}
		for(i = 0; i < padBite; i++) fputc(c,fp);//padding
	}
	fclose(fp);
}

//-----------------------------------------------
//8bit�i�P�F256�K���j
void saveBitmap8(int imageWidth, int imageHeight, GLubyte image[], const char *filename)
{
	int i, j, k;

	FILE *fp = fopen( filename, "wb");
  if(fp == NULL)
  {
		printf("not file opened\n");
		return;
  }

	Bitmap* bm = new Bitmap();

//�o�͗p��BITMAPFILEHEADER�̍쐬 
  bm->bf.Type = 0x4D42;
	bm->bf.Size = 54 + 1024 + imageWidth * imageHeight;
	bm->bf.OffBits = 54 + 1024;
  bm->bf.Reserved1 = 0;
  bm->bf.Reserved2 = 0;

  fwrite(&bm->bf.Type,sizeof(unsigned short),1,fp);
  fwrite(&bm->bf.Size,sizeof(unsigned long),1,fp);
  fwrite(&bm->bf.Reserved1,sizeof(unsigned short),1,fp);
  fwrite(&bm->bf.Reserved2,sizeof(unsigned short),1,fp);
  fwrite(&bm->bf.OffBits,sizeof(unsigned long),1,fp);
//�o�͗pBITMAPINFOHEADER�̍쐬
  bm->bi.Size = 40;
  bm->bi.Width = imageWidth;
  bm->bi.Height = imageHeight;
  bm->bi.Planes = 1;
	bm->bi.BitCount = 8;
  bm->bi.Compression = 0;//�����k
  bm->bi.SizeImage = 0;
  bm->bi.XPixPerMeter = 0;
  bm->bi.YPixPerMeter = 0;
	bm->bi.ClrUsed = 0;
	bm->bi.ClrImportant = 0;
	fwrite(&bm->bi, sizeof(INFOHEADER), 1, fp);

	//padding
	int nBite, pad;
	nBite = bm->bi.Width * bm->bi.BitCount / 8;
	pad = 4 - nBite + 4 * (nBite / 4);
	if(pad == 4) pad = 0;

	//�f�[�^�̏�������
	//�p���b�g���(�P�F256�K��)
	for(k = 0; k < 256; k++)
	{
		fputc((unsigned char)k, fp);//r
		fputc((unsigned char)k, fp);//g
		fputc((unsigned char)k, fp);//b
		fputc((unsigned char)0, fp);//dummy
	}
	//image data
	for (j = 0;j < imageHeight; j++)
	{
		for(i = 0; i < imageWidth; i++)
		{
			fputc(image[j * imageWidth + i], fp);
		}
		for(i = 0; i < pad; i++) fputc(0, fp);
	}
	fclose(fp);
}

//-----------------------------------------------
//24bit(RGB�e256�K��)
void saveBitmap24(int imageWidth, int imageHeight, GLubyte image[][3], const char *filename)
{
	int i, j;

	FILE *fp = fopen( filename, "wb");
  if(fp == NULL)
  {
		printf("not file opened\n");
		return;
  }

  Bitmap* bm = new Bitmap();

//�o�͗p��BITMAPFILEHEADER�̍쐬 
  bm->bf.Type = 0x4D42;
	bm->bf.Size = 54 + imageWidth * imageHeight * 3;
//printf("bfSize = %d \n", (int)bm->bf.Size);
	bm->bf.OffBits = 54;
  bm->bf.Reserved1 = 0;
  bm->bf.Reserved2 = 0;

  fwrite(&bm->bf.Type,sizeof(unsigned short),1,fp);
  fwrite(&bm->bf.Size,sizeof(unsigned long),1,fp);
  fwrite(&bm->bf.Reserved1,sizeof(unsigned short),1,fp);
  fwrite(&bm->bf.Reserved2,sizeof(unsigned short),1,fp);
  fwrite(&bm->bf.OffBits,sizeof(unsigned long),1,fp);

//�o�͗pBITMAPINFOHEADER�̍쐬
  bm->bi.Size = 40;
  bm->bi.Width = imageWidth;
  bm->bi.Height = imageHeight;
  bm->bi.Planes = 1;
	bm->bi.BitCount = 24;
  bm->bi.Compression = 0;//�����k
  bm->bi.SizeImage = 0;
  bm->bi.XPixPerMeter = 0;
  bm->bi.YPixPerMeter = 0;
	bm->bi.ClrUsed = 0;
	bm->bi.ClrImportant = 0;
	fwrite(&bm->bi, sizeof(INFOHEADER), 1, fp);

	//padding
	int nBite, pad;
	nBite = bm->bi.Width * bm->bi.BitCount / 8;
	pad = 4 - nBite + 4 * (nBite / 4);
	if(pad == 4) pad = 0;

	//image data�̏�������
	for (j = 0;j < imageHeight; j++)
	{
		for(i = 0; i < imageWidth; i++)
		{
			fputc(image[j * imageWidth + i][2], fp);//b
			fputc(image[j * imageWidth + i][1], fp);//g
			fputc(image[j * imageWidth + i][0], fp);//r
		}
		for(i = 0; i < pad; i++) fputc(0, fp);
	}
	fclose(fp);
}

//-----------------------------------------------
//32bit(RGBA�e256�K��)
void saveBitmap32(int imageWidth, int imageHeight,
  							GLubyte image[][4], const char *filename)
{
	int i, j;

	FILE *fp = fopen( filename, "wb");
  if(fp == NULL)
  {
		printf("not file opened\n");
		return;
  }

  Bitmap* bm = new Bitmap();

//�o�͗p��BITMAPFILEHEADER�̍쐬 
  bm->bf.Type = 0x4D42;
	bm->bf.Size = 54 + imageWidth * imageHeight * 4;
	bm->bf.OffBits = 54;
	bm->bi.BitCount = 32;

  bm->bf.Reserved1 = 0;
  bm->bf.Reserved2 = 0;

  fwrite(&bm->bf.Type,sizeof(unsigned short),1,fp);
  fwrite(&bm->bf.Size,sizeof(unsigned long),1,fp);
  fwrite(&bm->bf.Reserved1,sizeof(unsigned short),1,fp);
  fwrite(&bm->bf.Reserved2,sizeof(unsigned short),1,fp);
  fwrite(&bm->bf.OffBits,sizeof(unsigned long),1,fp);

//�o�͗pBITMAPINFOHEADER�̍쐬
  bm->bi.Size = 40;
  bm->bi.Width = imageWidth;
  bm->bi.Height = imageHeight;
  bm->bi.Planes = 1;
	bm->bi.BitCount = 32;
  bm->bi.Compression = 0;//�����k
  bm->bi.SizeImage = 0;
  bm->bi.XPixPerMeter = 0;
  bm->bi.YPixPerMeter = 0;
	bm->bi.ClrUsed = 0;
	bm->bi.ClrImportant = 0;
	fwrite(&bm->bi, sizeof(INFOHEADER), 1, fp);

	//image data�̏�������
	for (j = 0;j < imageHeight; j++)
	{
		for(i = 0; i < imageWidth; i++)
		{
			fputc(image[j * imageWidth + i][2], fp);//b
			fputc(image[j * imageWidth + i][1], fp);//g
			fputc(image[j * imageWidth + i][0], fp);//r
			fputc(image[j * imageWidth + i][3], fp);//a
		}
	}
	fclose(fp);
}

