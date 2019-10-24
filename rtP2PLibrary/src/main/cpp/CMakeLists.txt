cmake_minimum_required(VERSION 3.4.1)
#库文件位置
set(libs_src_DIR ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})
#本地源文件位置
set(code_src_DIR ${CMAKE_SOURCE_DIR}/src)

include_directories(${code_src_DIR})

#FFmpeg3 头文件位置
include_directories(${CMAKE_SOURCE_DIR}/include/rt_native)
#FFmpeg3 头文件位置
include_directories(${CMAKE_SOURCE_DIR}/include/FFmpeg3)
#runtop 头文件位置
include_directories(${CMAKE_SOURCE_DIR}/include/rtlib)

#ffmpeg module 头文件位置
include_directories(${code_src_DIR}/ffmpeg_module)
#ffmpeg module 头文件位置
include_directories(${code_src_DIR}/play_module)
#ffmpeg module 头文件位置
include_directories(${code_src_DIR}/record_file_module)

#avcodec
add_library(avcodec SHARED IMPORTED)
set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION
        ${libs_src_DIR}/libavcodec.so)
#avfilter
add_library(avfilter SHARED IMPORTED)
set_target_properties(avfilter PROPERTIES IMPORTED_LOCATION
        ${libs_src_DIR}/libavfilter.so)
#libavformat
add_library(avformat SHARED IMPORTED)
set_target_properties(avformat PROPERTIES IMPORTED_LOCATION
       ${libs_src_DIR}/libavformat.so)
#libavutil
add_library(avutil SHARED IMPORTED)
set_target_properties(avutil PROPERTIES IMPORTED_LOCATION
       ${libs_src_DIR}/libavutil.so)
#libswresample
add_library(swresample SHARED IMPORTED)
set_target_properties(swresample PROPERTIES IMPORTED_LOCATION
       ${libs_src_DIR}/libswresample.so)
#libswscale
add_library(swscale SHARED IMPORTED)
set_target_properties(swscale PROPERTIES IMPORTED_LOCATION
       ${libs_src_DIR}/libswscale.so)



#加载aac
add_library(libfdk_aac SHARED IMPORTED)
set_target_properties(libfdk_aac PROPERTIES IMPORTED_LOCATION
        ${libs_src_DIR}/libfdk-aac.so)

#加载rtp2p库
add_library(rtapi STATIC IMPORTED)
set_target_properties(rtapi PROPERTIES IMPORTED_LOCATION
        ${libs_src_DIR}/librtapi.a)

add_library(native SHARED
        ${code_src_DIR}/NativeCaller.c
        ${code_src_DIR}/NativeCallBack.c
        ${code_src_DIR}/RTMap.c
        ${code_src_DIR}/packet_queue.c
        ${code_src_DIR}/rt_p2p_live.c

        ${code_src_DIR}/ffmpeg_module/ffmpeg_parser.c
        ${code_src_DIR}/ffmpeg_module/ffmpeg_audio_decoder.c
        ${code_src_DIR}/ffmpeg_module/ffmpeg_aac_encoder.c
        ${code_src_DIR}/ffmpeg_module/ffmpeg_pcm_swr.c
        ${code_src_DIR}/ffmpeg_module/ffmpeg_video_decoder.c
        ${code_src_DIR}/ffmpeg_module/ffmpeg_video_sws.c
        ${code_src_DIR}/ffmpeg_module/ffmpeg_pcmskin.c
        ${code_src_DIR}/ffmpeg_module/ffmpeg_parser.c
        ${code_src_DIR}/ffmpeg_module/ffmpeg_audio.c

        ${code_src_DIR}/play_module/play_pcm_to_opensl_io.c
        ${code_src_DIR}/play_module/play_pcm_to_audioTrack.c
        ${code_src_DIR}/play_module/play_rgba_to_nativeWindow.c


        ${code_src_DIR}/record_file_module/record_mp4.c
        ${code_src_DIR}/record_file_module/record_picture.c        )

find_library(log-lib log)
find_library(OpenSLES-lib OpenSLES)
find_library(android-lib android)

target_link_libraries(native
        avcodec
        avfilter
        avformat
        avutil
        swresample
        swscale
        libfdk_aac
        rtapi
        OpenSLES
        ${log-lib}
        ${OpenSLES-lib}
        ${android-lib})