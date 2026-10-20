#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace miniptx {
struct ObjHeader { uint32_t magic=0x4D4F424A; uint32_t version=1; };
struct Symbol { std::string name; bool defined=true; uint32_t section=0; uint32_t offset=0; };
struct Reloc { uint32_t offset=0; std::string symbol; };
struct Section { std::string name; std::vector<uint8_t> data; };
struct ObjectFile { ObjHeader header; std::vector<Section> sections; std::vector<Symbol> symbols; std::vector<Reloc> relocs; };
bool writeObject(const ObjectFile& o, const std::string& path);
bool readObject(ObjectFile& o, const std::string& path, std::string& err);
}
