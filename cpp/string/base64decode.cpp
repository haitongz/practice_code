#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <sys/time.h>
#include <stdint.h>
#include "modp_b64.h"

using namespace std;
const int LOOP_COUNT = 100000;
// http://blog.csdn.net/morewindows/article/details/11871429
static const uint8_t kBase64DecodeTable[256] ={
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x3e,0xff,0xff,0xff,0x3f,
0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,
0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0xff,0xff,0xff,0xff,0xff,
0xff,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
};
static const uint8_t *kBase64EncodeTable = (const uint8_t *)
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void  base64_encode(const uint8_t *in, uint32_t len, uint8_t *buf) {
  buf[0] = kBase64EncodeTable[(in[0] >> 2) & 0x3f];
  if (len == 3) {
    buf[1] = kBase64EncodeTable[((in[0] << 4) & 0x30) | ((in[1] >> 4) & 0x0f)];
    buf[2] = kBase64EncodeTable[((in[1] << 2) & 0x3c) | ((in[2] >> 6) & 0x03)];
    buf[3] = kBase64EncodeTable[in[2] & 0x3f];
  } else if (len == 2) {
    buf[1] = kBase64EncodeTable[((in[0] << 4) & 0x30) | ((in[1] >> 4) & 0x0f)];
    buf[2] = kBase64EncodeTable[(in[1] << 2) & 0x3c];
  } else  { // len == 1
    buf[1] = kBase64EncodeTable[(in[0] << 4) & 0x30];
  }
}

void base64_decode(uint8_t *buf, uint32_t len) {
  buf[0] = (kBase64DecodeTable[buf[0]] << 2) |
           (kBase64DecodeTable[buf[1]] >> 4);
  if (len > 2) {
    buf[1] = ((kBase64DecodeTable[buf[1]] << 4) & 0xf0) |
              (kBase64DecodeTable[buf[2]] >> 2);
    if (len > 3) {
      buf[2] = ((kBase64DecodeTable[buf[2]] << 6) & 0xc0) |
                (kBase64DecodeTable[buf[3]]);
    }
  }
}

std::string Base64Decode(const std::string& src)
{
  std::string buf(src.begin(), src.end());
  uint8_t *bytes = (uint8_t *) buf.c_str();
  uint32_t len = buf.size();

  std::stringstream ss;

  while (len >= 4)
  {
    base64_decode(bytes, 4);

    for (uint8_t i = 0; i < 3; i++)
      ss << bytes[i];
    bytes += 4;
    len -= 4;
  }

  if (len > 1)
  {
    base64_decode(bytes, len);
    for (uint8_t i = 0; i < len - 1; i++)
      ss << bytes[i];
  }

  return ss.str();
}

std::string Base64Decode1(const std::string& src)
{
  std::string buf(src.begin(), src.end());
  uint32_t len = buf.size();
  char* c = const_cast<char *>(buf.data());
  char* d = c;

  int leftover = len % 4;
  if (leftover == 1)
  {
    buf.clear();
    return buf;
  }
  uint32_t decode_len = leftover == 0 ? len/4*3 : len/4*3 + (leftover - 1);

  while (len >= 4)
  {
    base64_decode((uint8_t *)c, 4);

    for (int i = 0; i < 3; ++i)
    {
      *d++ = *c++;
    }
    c++;
    len -= 4;
  }

  if (len > 1)
  {
    base64_decode((uint8_t *)c, len);
    for (uint8_t i = 0; i < len - 1; i++)
    {
      *d++ = *c++;
    }
  }
  buf.erase(decode_len, std::string::npos);

  return buf;
}

// use resize
std::string Base64Decode2(const std::string& src)
{
  std::string buf(src.begin(), src.end());
  uint32_t len = buf.size();
  char* c = const_cast<char *>(buf.data());
  char* d = c;

  int leftover = len % 4;
  if (leftover == 1)
  {
    buf.clear();
    return buf;
  }
  uint32_t decode_len = leftover == 0 ? len/4*3 : len/4*3 + (leftover - 1);

  while (len >= 4)
  {
    base64_decode((uint8_t *)c, 4);

    for (int i = 0; i < 3; ++i)
    {
      *d++ = *c++;
    }
    c++;
    len -= 4;
  }

  if (len > 1)
  {
    base64_decode((uint8_t *)c, len);
    for (uint8_t i = 0; i < len - 1; i++)
    {
      *d++ = *c++;
    }
  }
  buf.resize(decode_len);

  return buf;
}



int main(int argc, char *argv[])
{
  //std::string s = "zqus387489tid'23	tck=e02fce03a933b5c8fffdf46442a2e8e9ts"
    //"1461052882959uunid4tid=23&tck=e02fce03a933b5c8fffdf46442a2e8e9&ts=1461052882959&ver=1ip:106.39.88.82hostname:tf41dg.prod.mediav.comagent:Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_4)"
    //"AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2623.110 Safari/537.36lang:en,zh-CN;q=0.8,zh;q=0.6,zh-TW;q=0.4referer:http://ckmap.mediav.com/b?type=10proxy:106.39.88.82, 202.79.203.111_mvctn191270=_mvsrc=118216_523698_1043440&_mvcam=191270_1241961_11970810_58740969_0&osr=oqdT0qhmgzj0&time=1449650466&rdom=anonymous; _mvctn165564=_mvsrc=118478_524001_1044112&_mvcam=165564_1224948_11775153_58841999_0&osr=Zg4c0cS49V00&time=1451040575&rdom=anonymous; v=)he((]^XdqB2fOE=[AjT; _jzqa=1.4336274511317232000.1447337529.1447337529.1461045516.2; _jzqc=1; _jzqckmp=1; ckmts=PUPtXAzi,P6PtXAzi,RGPtXAzi,R6PtXAzi,U6PtXAzi,JGPtXAzi,JrPtXAzi,J6PtXAzi,bUPtXAziver10:0|0:0|0:0|0:10p!"
    //"version1.01";
  //s = "MoreWindows - http://blog.csdn.net/morewindows?viewmode=contents ~!@#$%\r\n";
  //s = "ab";
  //std::string s = "zqus387489tid'23	tck=e02fce03a933b5c8fffdf46442a2e8e9tsaaa"
    //"1461052882959uunid4tid=23&tck=e02fce03a933b5c8fffdf46442a2e8e9&ts=1461052882959&ver=1ip:106.39.88.82hostname:tf41dg.prod.mediav.com";
  //std::string s = "zqus387489tid'23	tck=e02fce03a933b5c8fffdf46442a2e8e9ts1461052882959uunid4tid=23&tck=e02fce03a933b5c8fffdf46442a2e8e9&ts=1461052882959&ver=1ip:106.39.88.82hostname:tf41dg.prod.mediav.comaag";
  string s = "enF1cwYzODc0ODkDdGlkJzIzCXRjaz1lMDJmY2UwM2E5MzNiNWM4ZmZmZGY0NjQ0MmEyZThlOQJ0czE0NjEwNTI4ODI5NTkFdXVuaWQDNHRpZD0yMyZ0Y2s9ZTAyZmNlMDNhOTMzYjVjOGZmZmRmNDY0NDJhMmU4ZTkmdHM9MTQ2MTA1Mjg4Mjk1OSZ2ZXI9MWlwOjEwNi4zOS44OC44MgFob3N0bmFtZTp0ZjQxZGcucHJvZC5tZWRpYXYuY29tAWFnZW50Ok1vemlsbGEvNS4wIChNYWNpbnRvc2g7IEludGVsIE1hYyBPUyBYIDEwXzExXzQpQXBwbGVXZWJLaXQvNTM3LjM2IChLSFRNTCwgbGlrZSBHZWNrbykgQ2hyb21lLzQ5LjAuMjYyMy4xMTAgU2FmYXJpLzUzNy4zNgFsYW5nOmVuLHpoLUNOO3E9MC44LHpoO3E9MC42LHpoLVRXO3E9MC40AXJlZmVyZXI6aHR0cDovL2NrbWFwLm1lZGlhdi5jb20vYj90eXBlPTEwAXByb3h5OjEwNi4zOS44OC44MiwgMjAyLjc5LjIwMy4xMTFfbXZjdG4xOTEyNzA9X212c3JjPTExODIxNl81MjM2OThfMTA0MzQ0MCZfbXZjYW09MTkxMjcwXzEyNDE5NjFfMTE5NzA4MTBfNTg3NDA5NjlfMCZvc3I9b3FkVDBxaG1nemowJnRpbWU9MTQ0OTY1MDQ2NiZyZG9tPWFub255bW91czsgX212Y3RuMTY1NTY0PV9tdnNyYz0xMTg0NzhfNTI0MDAxXzEwNDQxMTImX212Y2FtPTE2NTU2NF8xMjI0OTQ4XzExNzc1MTUzXzU4ODQxOTk5XzAmb3NyPVpnNGMwY1M0OVYwMCZ0aW1lPTE0NTEwNDA1NzUmcmRvbT1hbm9ueW1vdXM7IHY9KWhlKChdXlhkcUIyZk9FPVtBalQ7IF9qenFhPTEuNDMzNjI3NDUxMTMxNzIzMjAwMC4xNDQ3MzM3NTI5LjE0NDczMzc1MjkuMTQ2MTA0NTUxNi4yOyBfanpxYz0xOyBfanpxY2ttcD0xOyBja210cz1QVVB0WEF6aSxQNlB0WEF6aSxSR1B0WEF6aSxSNlB0WEF6aSxVNlB0WEF6aSxKR1B0WEF6aSxKclB0WEF6aSxKNlB0WEF6aSxiVVB0WEF6aQN2ZXIBMQsCEDA6MHwwOjB8MDowfDA6MTALcCECCw8BB3ZlcnNpb24LAQMxLjA";
  //string s = "YWJjZGVm";
  string a;
  struct timeval start, end;
  gettimeofday(&start, NULL);
  for (int i = 0; i < LOOP_COUNT; ++i)
  {
    a = Base64Decode(s);
  }
  //std::cout << "base64encoded: " << a << std::endl;
  gettimeofday(&end, NULL);
  std::cout << "Base64Decode timeuse: " << 1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) << std::endl;
  //std::cout << "base64decoded: " << a << std::endl;

  string a1;
  gettimeofday(&start, NULL);
  for (int i = 0; i < LOOP_COUNT; ++i)
  {
    a1 = Base64Decode1(s);
  }
  //std::cout << "base64encoded: " << a << std::endl;
  gettimeofday(&end, NULL);
  std::cout << "Base64Decode1 timeuse: " << 1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) << std::endl;
  //std::cout << "base64decoded1: " << a << std::endl;
  string a2;
  gettimeofday(&start, NULL);
  for (int i = 0; i < LOOP_COUNT; ++i)
  {
    a2 = Base64Decode2(s);
  }
  //std::cout << "base64encoded: " << a << std::endl;
  gettimeofday(&end, NULL);
  std::cout << "Base64Decode2 timeuse: " << 1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) << std::endl;

  if(a == a1 && a1 == a2)
  {
    std::cout << "They are same!!" << std::endl;
  }
  else
  {
    std::cout << "a: " << a << std::endl;
    std::cout << "a1: " << a1 << std::endl;
    std::cout << a.length()
              << a1.length();
  }
  return 0;
}
