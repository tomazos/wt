#pragma once

#include "ElfHeader.h"
#include "X86Instruction.h"

struct X86Position {
  size_t index;
  size_t address;
};

struct X86Program {
  vector<X86Instruction> instructions;
  size_t nbytes = 0;

  ElfHeader elf_header;
  ProgramSegmentHeader program_segment_header;

  void update_header() {
    instructions[0] = XI_RAW(to_byte_vector(elf_header));
    instructions[1] = XI_RAW(to_byte_vector(program_segment_header));
  }

  X86Program() {
    add_instruction(XI_RAW(to_byte_vector(elf_header)));
    add_instruction(XI_RAW(to_byte_vector(program_segment_header)));
  }

  X86Position add_instruction(X86Instruction instruction) {
    X86Position position;
    position.index = instructions.size();
    position.address = nbytes;

    instructions.push_back(instruction);
    nbytes += instruction.size();

    return position;
  }

  struct ImmediateFixup {
    size_t index;
    string label;
    signed long int delta;
  };

  vector<ImmediateFixup> immediate_fixups;

  void add_immediate_fixup(string label, signed long int delta) {
    immediate_fixups.push_back(
        ImmediateFixup{instructions.size() - 1, label, delta});
  }

  void pad_to_alignment(size_t alignment) {
    size_t extra = nbytes % alignment;

    if (extra != 0) add_instruction(XI_RAW(vector<byte>(alignment - extra, 0)));
  }

  X86Instruction& operator[](size_t idx) { return instructions[idx]; }

  void fixup_labels(const unordered_map<string, size_t>& label_addresses) {
    for (auto fixup : immediate_fixups) {
      auto it = label_addresses.find(fixup.label);

      if (it == label_addresses.end())
        throw logic_error("label not found: " + fixup.label);

      size_t addr = it->second + 0x400000 + fixup.delta;

      instructions[fixup.index].fixup(addr);
    }

    if (label_addresses.count("start"))
      elf_header.entry = 0x400000 + label_addresses.at("start");
  }

  vector<byte> output() {
    program_segment_header.filesz = nbytes;
    program_segment_header.memsz = nbytes;
    update_header();

    vector<byte> machine_codes;

    for (auto instruction : instructions) {
      vector<byte> machine_code = instruction.get_machine_code();
      machine_codes.insert(machine_codes.end(), machine_code.begin(),
                           machine_code.end());
    }

    return machine_codes;
  }
};
