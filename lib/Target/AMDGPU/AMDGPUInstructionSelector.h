//===- AMDGPUInstructionSelector --------------------------------*- C++ -*-==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
/// This file declares the targeting of the InstructionSelector class for
/// AMDGPU.
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AMDGPU_AMDGPUINSTRUCTIONSELECTOR_H
#define LLVM_LIB_TARGET_AMDGPU_AMDGPUINSTRUCTIONSELECTOR_H

#include "AMDGPU.h"
#include "AMDGPUArgumentUsageInfo.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/GlobalISel/InstructionSelector.h"

namespace {
#define GET_GLOBALISEL_PREDICATE_BITSET
#define AMDGPUSubtarget GCNSubtarget
#include "AMDGPUGenGlobalISel.inc"
#undef GET_GLOBALISEL_PREDICATE_BITSET
#undef AMDGPUSubtarget
}

namespace llvm {

class AMDGPUInstrInfo;
class AMDGPURegisterBankInfo;
class GCNSubtarget;
class MachineInstr;
class MachineOperand;
class MachineRegisterInfo;
class SIInstrInfo;
class SIMachineFunctionInfo;
class SIRegisterInfo;

class AMDGPUInstructionSelector : public InstructionSelector {
public:
  AMDGPUInstructionSelector(const GCNSubtarget &STI,
                            const AMDGPURegisterBankInfo &RBI,
                            const AMDGPUTargetMachine &TM);

  bool select(MachineInstr &I, CodeGenCoverage &CoverageInfo) const override;
  static const char *getName();

private:
  struct GEPInfo {
    const MachineInstr &GEP;
    SmallVector<unsigned, 2> SgprParts;
    SmallVector<unsigned, 2> VgprParts;
    int64_t Imm;
    GEPInfo(const MachineInstr &GEP) : GEP(GEP), Imm(0) { }
  };

  bool isInstrUniform(const MachineInstr &MI) const;
  /// tblgen-erated 'select' implementation.
  bool selectImpl(MachineInstr &I, CodeGenCoverage &CoverageInfo) const;

  MachineOperand getSubOperand64(MachineOperand &MO, unsigned SubIdx) const;
  bool selectCOPY(MachineInstr &I) const;
  bool selectG_TRUNC(MachineInstr &I) const;
  bool selectG_CONSTANT(MachineInstr &I) const;
  bool selectG_ADD(MachineInstr &I) const;
  bool selectG_EXTRACT(MachineInstr &I) const;
  bool selectG_GEP(MachineInstr &I) const;
  bool selectG_IMPLICIT_DEF(MachineInstr &I) const;
  bool selectG_INSERT(MachineInstr &I) const;
  bool selectG_INTRINSIC(MachineInstr &I, CodeGenCoverage &CoverageInfo) const;
  bool selectG_INTRINSIC_W_SIDE_EFFECTS(MachineInstr &I,
                                        CodeGenCoverage &CoverageInfo) const;
  bool selectG_ICMP(MachineInstr &I) const;
  bool hasVgprParts(ArrayRef<GEPInfo> AddrInfo) const;
  void getAddrModeInfo(const MachineInstr &Load, const MachineRegisterInfo &MRI,
                       SmallVectorImpl<GEPInfo> &AddrInfo) const;
  bool selectSMRD(MachineInstr &I, ArrayRef<GEPInfo> AddrInfo) const;
  bool selectG_LOAD(MachineInstr &I) const;
  bool selectG_SELECT(MachineInstr &I) const;
  bool selectG_STORE(MachineInstr &I) const;

  InstructionSelector::ComplexRendererFns
  selectVCSRC(MachineOperand &Root) const;

  InstructionSelector::ComplexRendererFns
  selectVSRC0(MachineOperand &Root) const;

  InstructionSelector::ComplexRendererFns
  selectVOP3Mods0(MachineOperand &Root) const;
  InstructionSelector::ComplexRendererFns
  selectVOP3OMods(MachineOperand &Root) const;
  InstructionSelector::ComplexRendererFns
  selectVOP3Mods(MachineOperand &Root) const;

  InstructionSelector::ComplexRendererFns
  selectSmrdImm(MachineOperand &Root) const;
  InstructionSelector::ComplexRendererFns
  selectSmrdImm32(MachineOperand &Root) const;
  InstructionSelector::ComplexRendererFns
  selectSmrdSgpr(MachineOperand &Root) const;

  const SIInstrInfo &TII;
  const SIRegisterInfo &TRI;
  const AMDGPURegisterBankInfo &RBI;
  const AMDGPUTargetMachine &TM;
  const GCNSubtarget &STI;
  bool EnableLateStructurizeCFG;
#define GET_GLOBALISEL_PREDICATES_DECL
#define AMDGPUSubtarget GCNSubtarget
#include "AMDGPUGenGlobalISel.inc"
#undef GET_GLOBALISEL_PREDICATES_DECL
#undef AMDGPUSubtarget

#define GET_GLOBALISEL_TEMPORARIES_DECL
#include "AMDGPUGenGlobalISel.inc"
#undef GET_GLOBALISEL_TEMPORARIES_DECL
};

} // End llvm namespace.
#endif
