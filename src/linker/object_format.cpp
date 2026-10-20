#include "miniptx/object_format.h"
#include <fstream>
#include <sstream>

namespace miniptx {
bool writeObject(const ObjectFile& o, const std::string& path){
  std::ofstream f(path, std::ios::binary);
  if(!f) return false;
  f.write(reinterpret_cast<const char*>(&o.header), sizeof(o.header));
  // sections count
  uint32_t n=o.sections.size(); f.write(reinterpret_cast<const char*>(&n),4);
  for(auto&s:o.sections){
    uint32_t len=s.name.size(); f.write(reinterpret_cast<const char*>(&len),4);
    f.write(s.name.data(),len);
    uint32_t dlen=s.data.size(); f.write(reinterpret_cast<const char*>(&dlen),4);
    f.write(reinterpret_cast<const char*>(s.data.data()),dlen);
  }
  uint32_t sn=o.symbols.size(); f.write(reinterpret_cast<const char*>(&sn),4);
  for(auto&s:o.symbols){
    uint32_t len=s.name.size(); f.write(reinterpret_cast<const char*>(&len),4);
    f.write(s.name.data(),len);
    f.write(reinterpret_cast<const char*>(&s.defined),1);
    f.write(reinterpret_cast<const char*>(&s.section),4);
    f.write(reinterpret_cast<const char*>(&s.offset),4);
  }
  uint32_t rn=o.relocs.size(); f.write(reinterpret_cast<const char*>(&rn),4);
  for(auto&r:o.relocs){
    f.write(reinterpret_cast<const char*>(&r.offset),4);
    uint32_t len=r.symbol.size(); f.write(reinterpret_cast<const char*>(&len),4);
    f.write(r.symbol.data(),len);
  }
  return true;
}
bool readObject(ObjectFile& o, const std::string& path, std::string& err){
  std::ifstream f(path, std::ios::binary);
  if(!f){ err="cannot open"; return false; }
  f.read(reinterpret_cast<char*>(&o.header), sizeof(o.header));
  if(o.header.magic!=0x4D4F424A){ err="bad magic"; return false; }
  uint32_t n; f.read(reinterpret_cast<char*>(&n),4);
  o.sections.resize(n);
  for(uint32_t i=0;i<n;i++){
    uint32_t len; f.read(reinterpret_cast<char*>(&len),4);
    o.sections[i].name.resize(len); f.read(o.sections[i].name.data(),len);
    uint32_t dlen; f.read(reinterpret_cast<char*>(&dlen),4);
    o.sections[i].data.resize(dlen); f.read(reinterpret_cast<char*>(o.sections[i].data.data()),dlen);
  }
  uint32_t sn; f.read(reinterpret_cast<char*>(&sn),4);
  o.symbols.resize(sn);
  for(uint32_t i=0;i<sn;i++){
    uint32_t len; f.read(reinterpret_cast<char*>(&len),4);
    o.symbols[i].name.resize(len); f.read(o.symbols[i].name.data(),len);
    f.read(reinterpret_cast<char*>(&o.symbols[i].defined),1);
    f.read(reinterpret_cast<char*>(&o.symbols[i].section),4);
    f.read(reinterpret_cast<char*>(&o.symbols[i].offset),4);
  }
  uint32_t rn; f.read(reinterpret_cast<char*>(&rn),4);
  o.relocs.resize(rn);
  for(uint32_t i=0;i<rn;i++){
    f.read(reinterpret_cast<char*>(&o.relocs[i].offset),4);
    uint32_t len; f.read(reinterpret_cast<char*>(&len),4);
    o.relocs[i].symbol.resize(len); f.read(o.relocs[i].symbol.data(),len);
  }
  return true;
}
}
