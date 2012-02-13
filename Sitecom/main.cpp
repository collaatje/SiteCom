#include <MAUtil/Moblet.h>
#include <mautil/connection.h>
#include <mastdlib.h>
#include <conprint.h>
#include <MAUtil/String.h>
#define CONNECTION_BUFFER_SIZE 1024
class MyMoblet : public MAUtil::Moblet, private MAUtil::HttpConnectionListener
{
public:
    MyMoblet();

    void httpFinished(MAUtil::HttpConnection *conn, int result);
    void connRecvFinished(MAUtil::Connection *conn, int result);
    void connReadFinished(MAUtil::Connection *conn, int result);
    void connWriteFinished(MAUtil::Connection *conn, int result);
    void keyPressEvent(int keyCode);
private:
    void initiateConnection(const char* url);
    char mBuffer[CONNECTION_BUFFER_SIZE];
    MAUtil::HttpConnection mHttp;
    bool mIsConnected;
    MAHandle mLogin;
    MAUtil::String mData;
};
MyMoblet::MyMoblet() : mHttp(this)
, mIsConnected(false)
{
    printf("http connection tutorial.\n");
    printf("press softkeys to send http requests.\n");
    printf("press 0 to exit\n");
}
// connect to the given url if not other connection is active
void MyMoblet::initiateConnection(const char* url) {
    if(mIsConnected) {
        printf("already connected\n..");
        return;
    }
    printf("\nconnecting to %s", url);

    mData = "name=Tim";

    //mLogin = maCreatePlaceholder();

    //maCreateData(mLogin, data.length());
    //maWriteData(mLogin, data.c_str(), 0, data.length());

    int res = mHttp.create(url, HTTP_POST);

    char len[16];
    itoa(mData.length(),len,8);
    mHttp.setRequestHeader("content-length", len);
    mHttp.write("name%3dTim", mData.length());

    if(res < 0) {
        printf("unable to connect - %i\n", res);
    } else {

        mIsConnected = true;
    }
}
void MyMoblet::httpFinished(MAUtil::HttpConnection* http, int result) {
    printf("HTTP %i\n", result);

    MAUtil::String contentLengthStr;
    int responseBytes = mHttp.getResponseHeader("content-length", &contentLengthStr);
    int contentLength = 0;
    if(responseBytes == CONNERR_NOHEADER)
    printf("no content-length response header\n");
    else {
        printf("content-length : %s\n", contentLengthStr.c_str());
        contentLength = atoi(contentLengthStr.c_str());
    }
    if(contentLength >= CONNECTION_BUFFER_SIZE || contentLength == 0) {
        printf("Receive in chunks..\n");
        mHttp.recv(mBuffer, CONNECTION_BUFFER_SIZE);
    } else {
        mBuffer[contentLength] = 0;
        mHttp.read(mBuffer, contentLength);
    }

}
void MyMoblet::connWriteFinished(MAUtil::Connection* conn, int result) {
	mHttp.finish();
}
void MyMoblet::connReadFinished(MAUtil::Connection* conn, int result) {
    if(result >= 0)
    printf("connReadFinished %i\n", result);
    else
    printf("connection error %i\n", result);
    mHttp.close();

    mIsConnected = false;
}
void MyMoblet::connRecvFinished(MAUtil::Connection * conn, int result){
    if(result >= 0) {
        printf("connRecvFinished %i\n", result);
        mHttp.recv(mBuffer, CONNECTION_BUFFER_SIZE);
        return;
    }
    else if(result == CONNERR_CLOSED) {
        printf("Receive finished!\n");
    } else {
        printf("connection error %i\n", result);
    }
    mHttp.close();
    mIsConnected = false;
}
// Press 0 to exit. Soft left and soft right will initiate new connections
void MyMoblet::keyPressEvent(int keyCode) {
    switch(keyCode) {

    case MAK_0:
        maExit(0);
        break;

    case MAK_SOFTLEFT:
        initiateConnection("http://192.168.0.1/");
        break;
    case MAK_SOFTRIGHT:
        initiateConnection("http://cminnovations.nl/test.php");
        break;
    }
}
extern "C" int MAMain() {
    InitConsole();
    gConsoleLogging = 1;
    MAUtil::Moblet::run(new MyMoblet());
}
