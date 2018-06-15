#include "loadTexture.h"
#include<string.h>
#include<png.h>

//加载TGA图像（无压缩格式），生成纹理
GLuint LoadTGATexture(const char *fileName)         // Loads A TGA File Into Memory
{
    TextureImage *texture = new TextureImage;
    GLubyte     TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};    // Uncompressed TGA Header
    GLubyte     TGAcompare[12];                             // Used To Compare TGA Header
    GLubyte     header[6];                                  // First 6 Useful Bytes From The Header
    GLuint      bytesPerPixel;                              // Holds Number Of Bytes Per Pixel Used In The TGA File
    GLuint      imageSize;                                  // Used To Store The Image Size When Setting Aside Ram
    GLuint      temp;                                       // Temporary Variable
    GLuint      type=GL_RGBA;                               // Set The Default GL Mode To RBGA (32 BPP)

    FILE *file = fopen(fileName, "rb");                     // Open The TGA File

    if( file==NULL ||                                       // Does File Even Exist?
        fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||  // Are There 12 Bytes To Read?
        memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0               ||  // Does The Header Match What We Want?
        fread(header,1,sizeof(header),file)!=sizeof(header))                // If So Read Next 6 Header Bytes
    {
        if (file == NULL)                                   // Did The File Even Exist? *Added Jim Strong*
            return false;                                   // Return False
        else
        {
            fclose(file);                                   // If Anything Failed, Close The File
            return false;                                   // Return False
        }
    }

    texture->width  = header[1] * 256 + header[0];           // Determine The TGA Width  (highbyte*256+lowbyte)
    texture->height = header[3] * 256 + header[2];           // Determine The TGA Height (highbyte*256+lowbyte)

    //OpenGL中纹理只能使用24位或者32位的TGA图像
    if( texture->width   <=0  ||                              // Is The Width Less Than Or Equal To Zero
        texture->height  <=0  ||                              // Is The Height Less Than Or Equal To Zero
        (header[4]!=24 && header[4]!=32))                   // Is The TGA 24 or 32 Bit?
    {
        fclose(file);                                       // If Anything Failed, Close The File
        return false;                                       // Return False
    }

    texture->bpp = header[4];                            // Grab The TGA's Bits Per Pixel (24 or 32)
    bytesPerPixel   = texture->bpp/8;                        // Divide By 8 To Get The Bytes Per Pixel
    imageSize       = texture->width*texture->height*bytesPerPixel;   // Calculate The Memory Required For The TGA Data

    texture->imageData=(GLubyte *)malloc(imageSize);     // Reserve Memory To Hold The TGA Data

    if( texture->imageData==NULL ||                          // Does The Storage Memory Exist?
        fread(texture->imageData, 1, imageSize, file)!=imageSize)    // Does The Image Size Match The Memory Reserved?
    {
        if(texture->imageData!=NULL)                     // Was Image Data Loaded
            free(texture->imageData);                        // If So, Release The Image Data

        fclose(file);                                       // Close The File
        return false;                                       // Return False
    }

    //RGB数据格式转换，便于在OpenGL中使用
    for(GLuint i=0; i<int(imageSize); i+=bytesPerPixel)      // Loop Through The Image Data
    {                                                       // Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
        temp=texture->imageData[i];                          // Temporarily Store The Value At Image Data 'i'
        texture->imageData[i] = texture->imageData[i + 2];    // Set The 1st Byte To The Value Of The 3rd Byte
        texture->imageData[i + 2] = temp;                    // Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
    }

    fclose (file);                                          // Close The File

    // Build A Texture From The Data
    glGenTextures(1, &texture->texID);                    // Generate OpenGL texture IDs

    glBindTexture(GL_TEXTURE_2D, texture->texID);         // Bind Our Texture
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // Linear Filtered
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // Linear Filtered

    if (texture->bpp==24)                                 // Was The TGA 24 Bits
    {
        type=GL_RGB;                                        // If So Set The 'type' To GL_RGB
    }

    glTexImage2D(GL_TEXTURE_2D, 0, type, texture->width, texture->height, 0, type, GL_UNSIGNED_BYTE, texture->imageData);
    GLuint id = texture->texID;
    delete texture;
    return id;                                            // Texture Building Went Ok, Return True
}



TextureImage * ReadPNGFromFile(const char *fileName)
{
    TextureImage *texinfo;
    png_byte magic[8];
    png_structp png_ptr;
    png_infop info_ptr;
    int bit_depth, color_type;
    FILE *fp = NULL;
    png_bytep *row_pointers = NULL;
    png_uint_32 w, h;
    int i;
    /* Open image file */
    fp = fopen(fileName, "rb");
    if (!fp)
    {
        fprintf(stderr, "error: couldn't open \"%s\"!\n", fileName);
        return NULL;
    }
    /* Read magic number */
    fread(magic, 1, sizeof(magic), fp);
    /* Check for valid magic number */
    if (!png_check_sig(magic, sizeof(magic)))
    {
        fprintf(stderr, "error: \"%s\" is not a valid PNG image!\n", fileName);
        fclose(fp);
        return NULL;
    }
    /* Create a png read struct */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        fclose(fp);
        return NULL;
    }
    /* Create a png info struct */
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return NULL;
    }
    /* Create our OpenGL texture object */
    texinfo = (TextureImage *)malloc(sizeof(TextureImage));
    /* Initialize the setjmp for returning properly after a libpng error occured */
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        if (row_pointers) free(row_pointers);
        if (texinfo) {
            if (texinfo->imageData)
                free(texinfo->imageData);
            free(texinfo);
        }
        return NULL;
    }
    /* Setup libpng for using standard C fread() function with our FILE pointer */
    png_init_io(png_ptr, fp);
    /* Tell libpng that we have already read the magic number */
    png_set_sig_bytes(png_ptr, sizeof(magic));
    /* Read png info */
    png_read_info(png_ptr, info_ptr);
    /* Get some usefull information from header */
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    /* Convert index color images to RGB images */
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);
    /* Convert 1-2-4 bits grayscale images to 8 bits grayscale. */
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand_gray_1_2_4_to_8(png_ptr);

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);
    if (bit_depth == 16) png_set_strip_16(png_ptr);
    else if (bit_depth < 8) png_set_packing(png_ptr);
    /* Update info structure to apply transformations */
    png_read_update_info(png_ptr, info_ptr);
    /* Retrieve updated information */
    png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type, NULL, NULL, NULL);
    texinfo->width = w;
    texinfo->height = h;
    /* Get image format and components per pixel */
    GetPNGtextureInfo(color_type, texinfo);
    /* We can now allocate memory for storing pixel data */
    texinfo->imageData= (GLubyte *)malloc(sizeof(GLubyte) * texinfo->width * texinfo->height * texinfo->internalFormat);
    /* Setup a pointer array. Each one points at the begening of a row. */
    row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * texinfo->height);
    for (i = 0; i < texinfo->height; ++i)
    {
        row_pointers[i] = (png_bytep)(texinfo->imageData+ ((texinfo->height - (i + 1)) * texinfo->width * texinfo->internalFormat));
    }
    /* Read pixel data using row pointers */
    png_read_image(png_ptr, row_pointers);
    /* Finish decompression and release memory */
    png_read_end(png_ptr, NULL);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    /* We don't need row pointers anymore */
    free(row_pointers);
    fclose(fp);
    return texinfo;
}
GLuint loadPNGTexture(const char *fileName)
{
    TextureImage *png_tex = NULL;
    GLuint tex_id = 0;
    GLint alignment;
    png_tex = ReadPNGFromFile(fileName);
    if (png_tex && png_tex->imageData)
    {
        /* Generate texture */
        glGenTextures(1, &png_tex->texID);
        glBindTexture(GL_TEXTURE_2D, png_tex->texID);
        /* Setup some parameters for texture filters and mipmapping */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, png_tex->internalFormat, png_tex->width, png_tex->height, 0, png_tex->format, GL_UNSIGNED_BYTE, png_tex->imageData);
        glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        tex_id = png_tex->texID;
        /* OpenGL has its own copy of texture data */
        free(png_tex->imageData);
        free(png_tex);
    }
    return tex_id;
}

void GetPNGtextureInfo(int color_type,  TextureImage *texinfo)
{
    switch (color_type)
    {
    case PNG_COLOR_TYPE_GRAY:
        texinfo->format = GL_LUMINANCE;
        texinfo->internalFormat = 1;
        break;

    case PNG_COLOR_TYPE_GRAY_ALPHA:
        texinfo->format = GL_LUMINANCE_ALPHA;
        texinfo->internalFormat = 2;
        break;

    case PNG_COLOR_TYPE_RGB:
        texinfo->format = GL_RGB;
        texinfo->internalFormat = 3;
        break;

    case PNG_COLOR_TYPE_RGB_ALPHA:
        texinfo->format = GL_RGBA;
        texinfo->internalFormat = 4;
        break;

    default:
        /* Badness */
        break;
    }
}
int power_of_two(int n)
{
    if (n <= 0)
        return 0;
    return (n & (n - 1)) == 0;
}

/* 函数load_texture
* 读取一个BMP文件作为纹理
* 如果失败，返回0，如果成功，返回纹理编号
*/
GLuint loadBMPTexture(const char* fileName)
{
    GLint width, height, total_bytes;
    GLubyte* pixels = 0;

    GLint last_texture_ID;//上一次绑定的纹理编号
    GLuint texture_ID = 0;//纹理编号

                          // 打开参数中传入的纹理文件，如果失败，返回
    FILE* pFile = fopen(fileName, "rb");
    if (pFile == 0)
        return 0;

    // 读取文件中图象的宽度和高度
    fseek(pFile, 0x0012, SEEK_SET);
    fread(&width, 4, 1, pFile);//int32 8位4个字节
    fread(&height, 4, 1, pFile);
    fseek(pFile, BMP_Header_Length, SEEK_SET);

    // 计算每行像素所占字节数，并根据此数据计算总像素字节数
    {
        GLint line_bytes = width * 3;
        while (line_bytes % 4 != 0)
            ++line_bytes;
        total_bytes = line_bytes * height;
    }

    // 根据总像素字节数分配内存
    pixels = (GLubyte*)malloc(total_bytes);
    if (pixels == 0)
    {
        fclose(pFile);
        return 0;
    }

    // 读取像素数据
    if (fread(pixels, total_bytes, 1, pFile) <= 0)
    {
        free(pixels);
        fclose(pFile);
        return 0;
    }

    // 在旧版本的OpenGL中
    // 如果图象的宽度和高度不是的整数次方，则需要进行缩放
    // 这里并没有检查OpenGL版本，出于对版本兼容性的考虑，按旧版本处理
    // 另外，无论是旧版本还是新版本，
    // 当图象的宽度和高度超过当前OpenGL实现所支持的最大值时，也要进行缩放
    {
        GLint max;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);//获取支持的最大纹理

                                                 //如果长宽有一个不是整数次方，或有一个大于支持的最大纹理，就重新设置长宽重新算一遍
        if (!power_of_two(width) || !power_of_two(height) || width > max || height > max)
        {
            //设定一些用于中间计算的新的值，这些值会在稍后被替代

            const GLint new_width = 256;
            const GLint new_height = 256; // 规定缩放后新的大小为边长的正方形
            GLint new_line_bytes, new_total_bytes;
            GLubyte* new_pixels = 0;

            // 计算每行需要的字节数和总字节数
            new_line_bytes = new_width * 3;
            while (new_line_bytes % 4 != 0)
                ++new_line_bytes;
            new_total_bytes = new_line_bytes * new_height;

            // 重新分配内存
            new_pixels = (GLubyte*)malloc(new_total_bytes);
            if (new_pixels == 0)
            {
                free(pixels);
                fclose(pFile);
                return 0;
            }

            // 进行像素缩放
            gluScaleImage(GL_RGB, width, height, GL_UNSIGNED_BYTE, pixels,
                new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

            // 释放原来的像素数据，把pixels指向新的像素数据，并重新设置width和height
            free(pixels);

            pixels = new_pixels;
            width = new_width;
            height = new_height;
        }
    }

    // 分配一个新的纹理编号，并不是分配编号1，而是分配1个新的编号到textureID....
    glGenTextures(1, &texture_ID);

    if (texture_ID == 0)
    {
        free(pixels);
        fclose(pFile);
        std::cout<<"gentexture fail!"<<std::endl;
        return 0;
    }

    // 绑定新的纹理，载入纹理并设置纹理参数
    // 在绑定前，先获得原来绑定的纹理编号，以便在最后进行恢复
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID); //第一个参数，表示你要得到什么状态的值,第二个参数即输出这个值

    glBindTexture(GL_TEXTURE_2D, texture_ID);//状态机，选定当前的纹理状态，以后的操作都是基于这个纹理

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //模型比纹理小了怎么办
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //模型比纹理大了怎么办
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //x坐标越界怎么办，GL_CLAMP表示超过的1.0的都按1.0那点的坐标绘制，GL_REPEAT表示不足重复补充直至达到那个值
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //y坐标越界怎么办，重复
    GLfloat fColor[4] = { 0.0f,0.0f,0.0f,0.0f };
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, fColor);

    //载入pixels数组中的图像为当前的纹理状态
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);

    //glBindTexture(GL_TEXTURE_2D, last_texture_ID);

    // 之前为pixels分配的内存可在使用glTexImage2D以后释放
    // 因为此时像素数据已经被OpenGL另行保存了一份（可能被保存到专门的图形硬件中）
    free(pixels);
    //该ID可以唯一标识一纹理
    return texture_ID;
}
GLuint loadTexture(const char* fileName){
    if(strstr(fileName,".TGA") || strstr(fileName,".tga")){
        return LoadTGATexture(fileName);
    }
    else if(strstr(fileName,".BMP") || strstr(fileName,".bmp")){
        return loadBMPTexture(fileName);
    }
    else if(strstr(fileName,".PNG") || strstr(fileName,".png")){
        return loadPNGTexture(fileName);
    }
}
