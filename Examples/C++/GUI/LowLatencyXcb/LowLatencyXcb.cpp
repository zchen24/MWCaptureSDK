#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "MWFOURCC.h"
#include "LibMWCapture/MWCapture.h"
#include "videorenderxcb/videorenderxcb.h"
#define CAPTURE_WIDTH  1920
#define CAPTURE_HEIGHT 1080
#define WIN_WIDTH 960
#define NUM_PARTIAL_LINE 64

static sem_t g_Sem;

void PrintVersion()
{
    BYTE byMaj, byMin;
    WORD wBuild;
    MWGetVersion(&byMaj, &byMin, &wBuild);
    printf("Magewell MWCapture SDK V%d.%d.1.%d - LowLatencyPreview\n",byMaj,byMin,wBuild);
    printf("Only pro Capture Devices are supported\n");
    printf("Usage:\n");
    printf("LowLatencyPreview <channel index>\n");
    printf("LowLatencyPreview <board id>:<channel id>\n");
}

int get_id(char c)
{
    if(c >= '0' && c <= '9')
        return (int)(c - '0');
    if(c >= 'a' && c <= 'f')
        return (int)(c - 'a' + 10);
    if(c >= 'A' && c <= 'F')
        return (int)(c - 'F' + 10);
    return 0;
}


HCHANNEL OpenChannel(int argc, char* argv[])
{
    HCHANNEL hChannel = NULL;
    int nChannelCount = MWGetChannelCount();

    if (0 == nChannelCount) {
        printf("ERROR: Can't find channels!\n");
        return NULL;
    }
    printf("Find %d channels!\n",nChannelCount);
    int nProDevCount = 0;
    int nProDevChannel[32] = {-1};
    for (int i = 0; i < nChannelCount; i++){
        MWCAP_CHANNEL_INFO info;
        MW_RESULT mr = MWGetChannelInfoByIndex(i, &info);
        if (0 == strcmp(info.szFamilyName, "Pro Capture")){
            nProDevChannel[nProDevCount] = i;
            nProDevCount++;
        }
    }
    if (nProDevCount <= 0){
        printf("\nERROR: Can't find pro channels!\n");
        return NULL;
    }

    printf("Find %d pro channels.\n", nProDevCount);

    // Get <board id > <channel id> or <channel index>
    int byBoardId = -1;
    int byChannelId = -1;
    int nDevIndex = -1;
    BOOL bIndex = FALSE;

    MWCAP_CHANNEL_INFO videoInfo = { 0 };
    if (argc == 1) {
        if (MW_SUCCEEDED != MWGetChannelInfoByIndex(nProDevChannel[0], &videoInfo)) {
            printf("ERROR: Can't get channel info!\n");
            return NULL;
        }

        bIndex = TRUE;
        nDevIndex = 0;
    }
    else if (NULL == strstr(argv[1], ":")){
        bIndex = TRUE;
        if ((strlen(argv[1]) > 2) || (argv[1][0] > '9') || argv[1][0] < '0') {
            printf("\nERROR: Invalid params!\n");
            return NULL;
        }
        nDevIndex = atoi(argv[1]);
        if(nDevIndex >= nProDevCount){
            printf("ERROR: just have %d channel!\n", nProDevCount);
            return NULL;
        }
    }
    else{
        bIndex = FALSE;
        if (strlen(argv[1]) == 3){
            if ((argv[1][0] >= '0' && argv[1][0] <= '9') || (argv[1][0] >= 'a' && argv[1][0] <= 'f') || (argv[1][0] >= 'A' && argv[1][0] <= 'F')){
                byBoardId = get_id(argv[1][0]);//atoi(argv[1]);
            }

            if ((argv[1][2] >= '0' && argv[1][2] <= '3')){
                byChannelId = get_id(argv[1][2]);//atoi(&argv[1][2]);
            }
        }

        if (-1 == byBoardId || -1 == byChannelId) {
            printf("\nERROR: Invalid params!\n");
            return NULL;
        }
    }

    // Open channel
    if (bIndex == TRUE){
        char path[128] = {0};
        MWGetDevicePath(nProDevChannel[nDevIndex], path);
        hChannel = MWOpenChannelByPath(path);
        if (hChannel == NULL) {
            printf("ERROR: Open channel %d error!\n", nDevIndex);
            return NULL;
        }
    }
    else{
        hChannel = MWOpenChannel(byBoardId, byChannelId);
        if (hChannel == NULL) {
            printf("ERROR: Open channel %X:%d error!\n", byBoardId, byChannelId);
            return NULL;
        }
    }

    if (MW_SUCCEEDED != MWGetChannelInfo(hChannel, &videoInfo)) {
        printf("ERROR: Can't get channel info!\n");
        return NULL;
    }

    printf("Open channel - BoardIndex = %X, ChannelIndex = %d.\n", videoInfo.byBoardIndex, videoInfo.byChannelIndex);
    printf("Product Name: %s\n", videoInfo.szProductName);
    printf("Board SerialNo: %s\n\n", videoInfo.szBoardSerialNo);
    return hChannel;
}

int main(int argc, char* argv[])
{
    int32_t i,iRet;
    uint32_t uiFourCC;
    uint32_t *puiFourCCList;
    int iListNum;
    uint32_t uiStartCapture = 0;
    HCHANNEL hChannel;
    MWCAP_PTR hCaptureEvent = 0;
    MWCAP_PTR hNotifyEvent = 0;
    HNOTIFY hNotify = 0;
    ULONGLONG ullStatusBits = 0;
    pthread_t pidViewThread;
    MWCAP_VIDEO_BUFFER_INFO VideoBufferInfo;
    MWCAP_VIDEO_FRAME_INFO VideoFrameInfo;
    MWCAP_VIDEO_CAPTURE_STATUS captureStatus;
    unsigned char *pucFrmBuf = NULL;
    DWORD dwMinStride;
    DWORD dwImageSize;
    VideoRenderXCB *render = new VideoRenderXCB;
    render->Init(CAPTURE_WIDTH, CAPTURE_HEIGHT);
    puiFourCCList = render->GetSupportedFourcc(iListNum);
    if(iListNum <= 0){
        delete render;
        return -1;
    }
    uiFourCC = *puiFourCCList;
    delete puiFourCCList;
    dwMinStride = FOURCC_CalcMinStride(uiFourCC, CAPTURE_WIDTH, 4);
    dwImageSize = FOURCC_CalcImageSize(uiFourCC, CAPTURE_WIDTH, CAPTURE_HEIGHT, dwMinStride);
    render->SetFrameFormat(CAPTURE_WIDTH, CAPTURE_HEIGHT, uiFourCC);
    PrintVersion();
    if(!MWCaptureInitInstance()){
        printf("InitilizeFailed\n");
        return -1;
    } 
    
    if(sem_init(&g_Sem, 0, 0) == -1) {
        printf("sem initial fail\n");
        return -1;
    }

    MWRefreshDevice();
    pucFrmBuf = (unsigned char*)malloc(dwImageSize);
    if(NULL == pucFrmBuf){
        goto end_and_free;
    }
    hChannel = OpenChannel(argc, argv);
    if(hChannel == NULL){
        goto end_and_free;
    }

    hCaptureEvent = MWCreateEvent();
    if (hCaptureEvent == 0){
        printf("Error: Create capture event error\n");
        goto end_and_free;
    }
    
    hNotifyEvent = MWCreateEvent();
    if (hNotifyEvent == 0){
        printf("Error: Create capture event error\n");
        goto end_and_free;
    }
    
    if (MWStartVideoCapture(hChannel, hCaptureEvent) != MW_SUCCEEDED) {
        printf("Error: Open Video Capture error!\n");
        goto end_and_free;
    }
    uiStartCapture = 1;

    hNotify = MWRegisterNotify(hChannel, hNotifyEvent, MWCAP_NOTIFY_VIDEO_FRAME_BUFFERING);
    
    MWPinVideoBuffer(hChannel, (MWCAP_PTR)pucFrmBuf, dwImageSize);
    while(1) { 
        if (MWWaitEvent(hNotifyEvent, 1000) <= 0){
            continue;
        }  
        if ((MWGetNotifyStatus(hChannel, hNotify, &ullStatusBits) != MW_SUCCEEDED)){
            continue;
        }
        if (MWGetVideoBufferInfo(hChannel, &VideoBufferInfo) != MW_SUCCEEDED){
            continue;
        }
        if (MWGetVideoFrameInfo(hChannel, VideoBufferInfo.iNewestBuffering, &VideoFrameInfo) != MW_SUCCEEDED){
            continue;
        }
        MWCaptureVideoFrameToVirtualAddressEx(hChannel,
            VideoBufferInfo.iNewestBuffering,
            pucFrmBuf,
            dwImageSize,
            dwMinStride,
            0,
            0,
            uiFourCC,
            CAPTURE_WIDTH,
            CAPTURE_HEIGHT,
            0,
            NUM_PARTIAL_LINE,
            0,
            0,
            0,
            100,
            0,
            100,
            0,
            MWCAP_VIDEO_DEINTERLACE_BLEND, 
            MWCAP_VIDEO_ASPECT_RATIO_IGNORE, 
            0,
            0,
            0,
            0,
            MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN,
            MWCAP_VIDEO_QUANTIZATION_UNKNOWN,
            MWCAP_VIDEO_SATURATION_UNKNOWN);
           
        do {
            MWWaitEvent(hCaptureEvent, 1000);
        } while ((MWGetVideoCaptureStatus(hChannel, &captureStatus) == MW_SUCCEEDED) && (!captureStatus.bFrameCompleted));
        
        if(render->DisplayFrame(pucFrmBuf,dwImageSize) < 0){
            break;
        }
    }
    MWUnpinVideoBuffer(hChannel, pucFrmBuf);
end_and_free:
    if(hNotify){
        MWUnregisterNotify(hChannel, hNotify);
        hNotify=0;
    }
    if(uiStartCapture){
        MWStopVideoCapture(hChannel);
    }

    if(hNotifyEvent!=0){
        MWCloseEvent(hNotifyEvent);
        hNotifyEvent=0;
    }

    if(hCaptureEvent!=0){
        MWCloseEvent(hCaptureEvent);
        hCaptureEvent=0;
    }
    
    if (hChannel != NULL){
        MWCloseChannel(hChannel);
        hChannel=NULL;
    }
    
    if(pucFrmBuf){
        free(pucFrmBuf);
    }
    if(render){
        delete render;
    }
    sem_destroy(&g_Sem);
    MWCaptureExitInstance();
}

