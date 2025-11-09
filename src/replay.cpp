#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <string>
#include <cstdint>
int main(int argc, char** argv){
  std::string input="data/golden/itch_1m.bin";
  double skew_ppm=0.0; double gap_rate=0.0; double corrupt_rate=0.0; int burst_ms=0;
  for(int i=1;i<argc;i++){
    std::string a=argv[i];
    if((a=="--input"||a=="-i") && i+1<argc) input=argv[++i];
    else if(a=="--skew-ppm" && i+1<argc) skew_ppm=std::stod(argv[++i]);
    else if(a=="--gap-rate" && i+1<argc) gap_rate=std::stod(argv[++i]);
    else if(a=="--corrupt-rate" && i+1<argc) corrupt_rate=std::stod(argv[++i]);
    else if(a=="--burst-ms" && i+1<argc) burst_ms=std::stoi(argv[++i]);
  }
  auto t0=std::chrono::steady_clock::now();
  std::ifstream f(input, std::ios::binary);
  if(!f){ std::cerr<<"no input: "<<input<<"\n"; return 2; }
  std::vector<char> buf(1<<20);
  uint64_t rolling=1469598103934665603ULL; // FNV offset
  while(f){
    f.read(buf.data(), buf.size());
    std::streamsize n=f.gcount();
    for(std::streamsize i=0;i<n;i++) rolling = (rolling ^ (uint8_t)buf[i])*1099511628211ULL;
  }
  auto t1=std::chrono::steady_clock::now();
  double ms=std::chrono::duration<double,std::milli>(t1-t0).count();
  std::cout<<"digest=0x"<<std::hex<<rolling<<std::dec<<" elapsed_ms="<<ms
           <<" gap_rate="<<gap_rate<<" corrupt_rate="<<corrupt_rate
           <<" burst_ms="<<burst_ms<<" skew_ppm="<<skew_ppm<<"\n";
  return 0;
}
