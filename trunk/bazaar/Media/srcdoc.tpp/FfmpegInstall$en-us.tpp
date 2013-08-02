topic "Ffmpeg install";
[ $$0,0#00000000000000000000000000000000:Default]
[a83;*R6 $$1,0#31310162474203024125188417583966:caption]
[{_}%EN-US 
[s1; [+184 Ffmpeg install]&]
[s0; [2 Ffmpeg is a great library but it does not include complete 
installing instructions. Here are some useful links.]&]
[s0;2 &]
[s0;2 &]
[s0; [* In Linux]&]
[s0;2 &]
[s0; [2 Install ffmpeg packages:]&]
[s0;i150;O0; [2 libavcodec52 or libavcodec`-extra`-52]&]
[s0;i150;O0; [2 libavcodec`-dev]&]
[s0;i150;O0; [2 libavformat52 or libavformat`-extra`-52]&]
[s0;i150;O0; [2 libavformat`-dev]&]
[s0;i150;O0; [2 libavdevice52 or libavdevice`-extra`-52]&]
[s0;i150;O0; [2 libavdevice`-dev]&]
[s0;i150;O0; [2 libavfilter0 or libavfilter`-extra`-0]&]
[s0;i150;O0; [2 libavfilter`-dev]&]
[s0;i150;O0; [2 libavutil49 or libavutil`-extra`-49]&]
[s0;i150;O0; [2 libavutil`-dev]&]
[s0;i150;O0; [2 libpostproc51 or libpostproc`-extra`-51]&]
[s0;i150;O0; [2 libpostproc`-dev]&]
[s0;i150;O0; [2 libswscale0 or libswscale`-extra`-0]&]
[s0;i150;O0; [2 libswscale`-dev]&]
[s0;2 &]
[s0; [2 Other possible packages are:]&]
[s0;i150;O0; [2 libdc1394`-22`-dev]&]
[s0;i150;O0; [2 libfaad`-dev]&]
[s0;i150;O0; [2 libgsm1`-dev]&]
[s0;i150;O0; [2 libogg`-dev]&]
[s0;i150;O0; [2 libraw1394`-dev]&]
[s0;i150;O0; [2 libschroedinger`-dev]&]
[s0;i150;O0; [2 libspeex`-dev]&]
[s0;i150;O0; [2 libtheora`-dev ]&]
[s0;i150;O0; [2 libvorbis`-dev]&]
[s0;2 &]
[s0;2 &]
[s0; [2 It is also possible to compile it from sources, but I have 
not found a reliable way to do it.]&]
[s0;2 &]
[s0;2 &]
[s0; [* In Windows]&]
[s0;2 &]
[s0; [2 Windows users are fortunate. They have three possibilities: 
safe (recommended!!), medium and hard ways.]&]
[s0;2 &]
[s0;2 &]
[s0; [*2 Safe way]&]
[s0;2 &]
[s0;# [2 Download ][^https`:`/`/sourceforge`.net`/projects`/upp`/files`/GPL`-sources`/Ffmpeg`%20plugin`_LGPL`.7z`/download^2 f
fmpeg][2  and ][^https`:`/`/sourceforge`.net`/projects`/upp`/files`/GPL`-sources`/SDL`%20plugin`_LGPL`.7z`/download^2 S
DL][2  from U`+`+ Sourceforge following the links. They include 
.dll, .lib, .a, .h and source code for recent versions.]&]
[s0;#2 &]
[s0;# [2 Copy the downloaded folders in upp folder. You will get a 
folder structure like this:]&]
[s0; [2 drive:`\upp`\uppsrc`\plugin`\ffmpeg]&]
[s0; [2 drive:`\upp`\uppsrc`\plugin`\SDL]&]
[s0;2 &]
[s0; [2 In Setup/Build Methods menu include the next folders:]&]
[s0;%- &]
[ {{3371:3564:3065 [s0;= [2 PATH `- executable directories]]
:: [s0;= [2 INCLUDE directories]]
:: [s0;= [2 LIB directories]]
:: [s0; [2 drive:`\upp]]
:: [s0; [2 drive:`\upp`\uppsrc`\plugin`\SDL`\include]&]
[s0; [2 drive:`\upp`\uppsrc`\plugin`\ffmpeg`\include]]
:: [s0; [2 drive:`\upp`\uppsrc`\plugin`\SDL`\lib]&]
[s0; [2 drive:`\upp`\uppsrc`\plugin`\ffmpeg`\lib]]}}&]
[s0;2 &]
[s0; [2 In addition, MSC9 and MSC10 require an additional include folder]&]
[s0;2 &]
[ {{3211:6789 [s0;= [2 Compiler]]
:: [s0;= [2 INCLUDE directories]]
:: [s0; [2 MSC9]]
::|1 [s0; [2 drive:`\upp`\uppsrc`\plugin`\ffmpeg`\include`_msc]]
:: [s0; [2 MSC10]]
:: [s0;%- ]}}&]
[s0;2 &]
[s0;2 &]
[s0; [*2 Medium]&]
[s0;2 &]
[s0; [2 All ffmpeg files in `"safe way`" have been got from the great 
daily updated ][^http`:`/`/ffmpeg`.arrozcru`.org`/autobuilds`/^2 `"Automated 
Ffmpeg Builds`" by Ramiro Polla][2 .]&]
[s0;2 &]
[s0; [2 To get the bleeding edge compiled files, download the files 
labeled `"shared`" and `"shared`-dev`".]&]
[s0;2 &]
[s0; [2 You can set them in your own folder structure or just follow 
the `"Safe way`" one. Do not forget to delete inttipes.h and 
stdint.h when compiling with MinGW.]&]
[s0;2 &]
[s0; [2 Remember that these files are supplied `"as is`" and they do 
not usually represent any `"stable`" version.]&]
[s0;2 &]
[s0;2 &]
[s0; [*2 Hard way]&]
[s0;2 &]
[s0; [2 Ramiro Polla web also includes ][^http`:`/`/ffmpeg`.arrozcru`.org`/wiki`/index`.php`?title`=Links^2 i
nstructions to build ffmpeg from sources][2 . Only for very experts.]&]
[s0; ]